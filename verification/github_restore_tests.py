#!/usr/bin/env python3
"""Temporary-directory safety tests for the checked large-file restore utility."""
from pathlib import Path
import gzip
import hashlib
import json
import os
import stat
import subprocess
import sys
import tempfile
import unittest

SCRIPT = Path(__file__).resolve().parents[1] / 'tools/restore_large_files.py'


def sha(data):
    return hashlib.sha256(data).hexdigest()


class RestoreTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix='ti60-github-restore-test-')
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name) / 'repo'
        (self.root / 'tools').mkdir(parents=True)
        self.payload = bytes(range(256)) * 2048 + b'preserve every final byte\x00\xff'
        self.name = 'PROJECT/outflow/generated.map.v'
        self.make_manifest()

    def make_manifest(self, payload=None, mode='100644'):
        compressed = gzip.compress(self.payload if payload is None else payload, mtime=0)
        # Deliberately split the gzip header itself and a later data segment.
        cuts = (0, 5, 17, len(compressed) // 2, len(compressed))
        parts = []
        for index, (start, end) in enumerate(zip(cuts, cuts[1:])):
            content = compressed[start:end]
            name = f'.github_large_files/test/part{index:03d}.gz'
            path = self.root / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(content)
            parts.append({'path': name, 'size': len(content), 'sha256': sha(content)})
        self.manifest = {'version': 1, 'files': [{
            'path': self.name, 'size': len(self.payload), 'sha256': sha(self.payload),
            'git_sha': hashlib.sha1(b'blob ' + str(len(self.payload)).encode() + b'\0' + self.payload).hexdigest(),
            'mode': mode, 'archive_size': len(compressed), 'archive_sha256': sha(compressed), 'parts': parts}]}
        self.save_manifest()

    def save_manifest(self):
        (self.root / 'tools/large_files_manifest.json').write_text(json.dumps(self.manifest))

    def run_tool(self, *args, ok=True):
        result = subprocess.run([sys.executable, str(SCRIPT), '--root', str(self.root), *args],
                                text=True, capture_output=True, timeout=30)
        self.assertEqual(result.returncode, 0 if ok else 1, result.stdout + result.stderr)
        self.assertNotIn('Traceback', result.stderr)
        return result

    def write_original(self, data):
        path = self.root / self.name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(data)
        return path

    def corrupt_part(self):
        part = self.root / self.manifest['files'][0]['parts'][2]['path']
        old = part.read_bytes()
        part.write_bytes(bytes([old[0] ^ 0x55]) + old[1:])

    def test_multi_part_stream_restore_and_verify(self):
        result = self.run_tool()
        self.assertIn('RESTORED', result.stdout)
        self.assertEqual((self.root / self.name).read_bytes(), self.payload)
        self.assertIn('VERIFIED', self.run_tool('--verify').stdout)

    def test_correct_existing_file_is_skipped(self):
        path = self.write_original(self.payload)
        before = path.stat().st_mtime_ns
        self.assertIn('SKIP', self.run_tool().stdout)
        self.assertEqual(path.stat().st_mtime_ns, before)
        self.assertIn('SKIP', self.run_tool('--force').stdout)

    def test_modified_existing_requires_force(self):
        path = self.write_original(b'user modifications')
        self.assertIn('--force', self.run_tool(ok=False).stderr)
        self.assertEqual(path.read_bytes(), b'user modifications')
        self.run_tool('--force')
        self.assertEqual(path.read_bytes(), self.payload)

    def test_corrupt_part_does_not_replace_existing_file(self):
        path = self.write_original(b'keep user data')
        self.corrupt_part()
        self.assertIn('SHA-256 mismatch', self.run_tool('--force', ok=False).stderr)
        self.assertEqual(path.read_bytes(), b'keep user data')
        self.assertFalse(list(path.parent.glob('.*.restore-*.tmp')))

    def test_corrupt_combined_archive_digest(self):
        self.manifest['files'][0]['archive_sha256'] = '0' * 64
        self.save_manifest()
        self.assertIn('combined archive', self.run_tool(ok=False).stderr)
        self.assertFalse((self.root / self.name).exists())

    def test_decompressed_hash_mismatch_is_atomic(self):
        path = self.write_original(b'keep old version')
        other = self.payload[:-1] + b'Z'
        self.make_manifest(payload=other)
        result = self.run_tool('--force', ok=False)
        self.assertIn('(decompressed): SHA-256 mismatch', result.stderr)
        self.assertEqual(path.read_bytes(), b'keep old version')
        self.assertFalse(list(path.parent.glob('.*.restore-*.tmp')))

    def test_decompressed_size_is_bounded(self):
        path = self.write_original(b'old')
        self.manifest['files'][0]['size'] = 7
        self.save_manifest()
        self.assertIn('exceeds declared size 7', self.run_tool('--force', ok=False).stderr)
        self.assertEqual(path.read_bytes(), b'old')
        self.assertFalse(list(path.parent.glob('.*.restore-*.tmp')))

    def test_gzip_corruption_after_correct_archive_hash(self):
        path = self.write_original(b'keep original')
        raw = b'not a gzip stream'
        part = self.manifest['files'][0]['parts'][0]
        (self.root / part['path']).write_bytes(raw)
        part.update(size=len(raw), sha256=sha(raw))
        self.manifest['files'][0].update(parts=[part], archive_size=len(raw), archive_sha256=sha(raw))
        self.save_manifest()
        self.run_tool('--force', ok=False)
        self.assertEqual(path.read_bytes(), b'keep original')

    def test_reject_path_traversal_absolute_git_and_windows_aliases(self):
        for path in ['../escape.bin', '/tmp/escape.bin', '.git/config', 'dir/../escape',
                     'C:/escape.bin', 'C:\\escape.bin', 'safe/.GIT/config', 'NUL.bin', 'data:stream']:
            with self.subTest(path=path):
                self.manifest['files'][0]['path'] = path
                self.save_manifest()
                self.run_tool('--force', ok=False)
        self.assertFalse((Path(self.temp.name) / 'escape.bin').exists())

    def test_reject_archive_path_traversal(self):
        self.manifest['files'][0]['parts'][0]['path'] = '../outside.gz'
        self.save_manifest()
        self.run_tool(ok=False)
        self.assertFalse((self.root / self.name).exists())

    @unittest.skipUnless(hasattr(os, 'symlink'), 'symlink unavailable')
    def test_symlink_parent_escape(self):
        outside = Path(self.temp.name) / 'outside'
        outside.mkdir()
        try:
            (self.root / 'PROJECT').symlink_to(outside, target_is_directory=True)
        except OSError as exc:
            self.skipTest(f'symlink creation unavailable: {exc}')
        self.assertIn('symlink', self.run_tool('--force', ok=False).stderr)
        self.assertEqual(list(outside.iterdir()), [])

    @unittest.skipUnless(hasattr(os, 'symlink'), 'symlink unavailable')
    def test_symlink_destination_and_archive_are_rejected(self):
        outside = Path(self.temp.name) / 'outside.bin'
        outside.write_bytes(b'do not touch')
        target = self.root / self.name
        target.parent.mkdir(parents=True)
        try:
            target.symlink_to(outside)
        except OSError as exc:
            self.skipTest(f'symlink creation unavailable: {exc}')
        self.run_tool('--force', ok=False)
        self.assertEqual(outside.read_bytes(), b'do not touch')
        target.unlink()
        part = self.root / self.manifest['files'][0]['parts'][0]['path']
        part.unlink()
        part.symlink_to(outside)
        self.run_tool(ok=False)
        self.assertFalse(target.exists())

    def test_verify_missing_original_still_checks_archive(self):
        self.corrupt_part()
        result = self.run_tool('--verify', ok=False)
        self.assertIn('[original]', result.stderr)
        self.assertIn('[archive]', result.stderr)
        self.assertFalse((self.root / self.name).exists())

    def test_mode_755_and_reject_other_mode_before_any_restore(self):
        self.make_manifest(mode='100755')
        self.run_tool()
        if os.name != 'nt':
            self.assertEqual(stat.S_IMODE((self.root / self.name).stat().st_mode), 0o755)
        (self.root / self.name).unlink()
        unsafe = dict(self.manifest['files'][0], path='bad-mode.bin', mode='100777')
        self.manifest['files'].append(unsafe)
        self.save_manifest()
        self.run_tool(ok=False)
        self.assertFalse((self.root / self.name).exists())

    def test_parts_sum_and_duplicate_destinations_are_rejected(self):
        self.manifest['files'][0]['archive_size'] += 1
        self.save_manifest()
        self.run_tool(ok=False)
        self.manifest['files'][0]['archive_size'] -= 1
        self.manifest['files'].append(dict(self.manifest['files'][0]))
        self.save_manifest()
        self.run_tool(ok=False)


if __name__ == '__main__':
    unittest.main(verbosity=2)
