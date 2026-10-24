#!/usr/bin/env python3
"""Restore byte-identical large files from checked gzip parts in this repository.

Default: restore missing files and skip existing correct files. Modified originals
are an error unless --force is supplied. --verify changes nothing and checks both
originals and archives. Only Python's standard library is required.
"""
from __future__ import annotations

import argparse
from dataclasses import dataclass
import gzip
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import stat
import sys
import tempfile
from typing import BinaryIO
import zlib

CHUNK = 1024 * 1024
MANIFEST = 'tools/large_files_manifest.json'


class RestoreError(Exception):
    """A malformed manifest, unsafe path, or integrity mismatch."""


@dataclass(frozen=True)
class Part:
    path: str
    size: int
    sha256: str


@dataclass(frozen=True)
class Record:
    path: str
    size: int
    sha256: str
    mode: int
    archive_size: int
    archive_sha256: str
    parts: tuple[Part, ...]


def relative_path(value: object) -> str:
    if not isinstance(value, str) or not value or '\\' in value:
        raise RestoreError('paths must be nonempty repository-relative POSIX paths')
    if any(ord(c) < 32 for c in value) or re.match(r'^[A-Za-z]:', value):
        raise RestoreError(f'unsafe path: {value!r}')
    path = PurePosixPath(value)
    if path.is_absolute() or any(c in ('', '.', '..') for c in value.split('/')):
        raise RestoreError(f'absolute, empty, dot, or parent path component: {value!r}')
    if any(c.casefold() == '.git' for c in path.parts):
        raise RestoreError(f'.git paths are forbidden: {value!r}')
    # Windows must not interpret ADS, wildcard, device, or trailing-dot aliases.
    for component in path.parts:
        if any(c in component for c in ':*?"<>|') or component.endswith((' ', '.')):
            raise RestoreError(f'nonportable path component: {value!r}')
        stem = component.split('.')[0].upper()
        if stem in {'CON', 'PRN', 'AUX', 'NUL', *(f'COM{i}' for i in range(1, 10)),
                    *(f'LPT{i}' for i in range(1, 10))}:
            raise RestoreError(f'reserved device name: {value!r}')
    return path.as_posix()


def safe_path(root: Path, relative: str) -> Path:
    """Reject symlink components, including dangling links, before any file use."""
    relative = relative_path(relative)
    path = root
    for component in PurePosixPath(relative).parts:
        path = path / component
        try:
            info = path.lstat()
        except FileNotFoundError:
            continue
        if stat.S_ISLNK(info.st_mode):
            raise RestoreError(f'symlink components are not supported: {relative!r}')
    try:
        path.resolve(strict=False).relative_to(root)
    except ValueError as exc:
        raise RestoreError(f'path escapes repository: {relative!r}') from exc
    return path


def open_regular(root: Path, relative: str) -> BinaryIO:
    path = safe_path(root, relative)
    flags = os.O_RDONLY | getattr(os, 'O_BINARY', 0) | getattr(os, 'O_NOFOLLOW', 0)
    descriptor = os.open(path, flags)
    try:
        if not stat.S_ISREG(os.fstat(descriptor).st_mode):
            raise RestoreError(f'not a regular file: {relative!r}')
        # Recheck the path after open, including parent components.
        safe_path(root, relative)
        return os.fdopen(descriptor, 'rb')
    except BaseException:
        os.close(descriptor)
        raise


def size_field(value: object, name: str) -> int:
    if type(value) is not int or value < 0:
        raise RestoreError(f'{name} must be a nonnegative integer')
    return value


def hash_field(value: object, name: str) -> str:
    if not isinstance(value, str) or not re.fullmatch(r'[0-9a-fA-F]{64}', value):
        raise RestoreError(f'{name} must be a SHA-256 hex digest')
    return value.lower()


def load_manifest(root: Path) -> tuple[Record, ...]:
    with open_regular(root, MANIFEST) as stream:
        content = stream.read(16 * CHUNK + 1)
    if len(content) > 16 * CHUNK:
        raise RestoreError('manifest exceeds 16 MiB')
    data = json.loads(content.decode('utf-8-sig'))
    if not isinstance(data, dict) or data.get('version') != 1 or not isinstance(data.get('files'), list):
        raise RestoreError('expected manifest version 1 with a files list')
    records, seen, archives = [], set(), set()
    for entry in data['files']:
        if not isinstance(entry, dict):
            raise RestoreError('each files entry must be an object')
        name = relative_path(entry.get('path'))
        if name.casefold() in seen:
            raise RestoreError(f'duplicate destination: {name!r}')
        seen.add(name.casefold())
        raw_mode = entry.get('mode')
        if raw_mode not in ('100644', '100755', '644', '755'):
            raise RestoreError(f'unsupported mode for {name!r}: {raw_mode!r}')
        raw_parts = entry.get('parts')
        if not isinstance(raw_parts, list) or not raw_parts:
            raise RestoreError(f'nonempty parts list required for {name!r}')
        parts = []
        for part in raw_parts:
            if not isinstance(part, dict):
                raise RestoreError('each part must be an object')
            part_name = relative_path(part.get('path'))
            safe_path(root, part_name)
            archives.add(part_name.casefold())
            parts.append(Part(part_name, size_field(part.get('size'), 'part.size'),
                              hash_field(part.get('sha256'), 'part.sha256')))
        archive_size = size_field(entry.get('archive_size'), 'archive_size')
        if sum(part.size for part in parts) != archive_size:
            raise RestoreError(f'part sizes do not sum to archive_size: {name!r}')
        safe_path(root, name)
        records.append(Record(name, size_field(entry.get('size'), 'size'),
            hash_field(entry.get('sha256'), 'sha256'), int(raw_mode[-3:], 8),
            archive_size, hash_field(entry.get('archive_sha256'), 'archive_sha256'), tuple(parts)))
    if seen & archives:
        raise RestoreError('a destination must not overwrite an archive part')
    return tuple(records)


def checked_copy(stream: BinaryIO, expected_size: int, expected_hash: str,
                 label: str, output: BinaryIO | None = None,
                 archive_hash=None) -> None:
    digest, total = hashlib.sha256(), 0
    while True:
        block = stream.read(min(CHUNK, expected_size - total + 1))
        if not block:
            break
        total += len(block)
        if total > expected_size:
            raise RestoreError(f'{label}: exceeds declared size {expected_size}')
        digest.update(block)
        if archive_hash is not None:
            archive_hash.update(block)
        if output is not None:
            output.write(block)
    if total != expected_size:
        raise RestoreError(f'{label}: size {total}, expected {expected_size}')
    if digest.hexdigest() != expected_hash:
        raise RestoreError(f'{label}: SHA-256 mismatch')


def check_original(root: Path, record: Record) -> None:
    with open_regular(root, record.path) as stream:
        checked_copy(stream, record.size, record.sha256, record.path)


def check_archive(root: Path, record: Record, output: BinaryIO | None = None) -> None:
    whole = hashlib.sha256()
    for part in record.parts:
        with open_regular(root, part.path) as stream:
            checked_copy(stream, part.size, part.sha256, part.path, output, whole)
    if whole.hexdigest() != record.archive_sha256:
        raise RestoreError(f'{record.path}: combined archive SHA-256 mismatch')


def restore(root: Path, record: Record, force: bool) -> str:
    destination = safe_path(root, record.path)
    if destination.exists():
        try:
            check_original(root, record)
        except (OSError, RestoreError) as exc:
            if not force:
                raise RestoreError(f'{record.path}: existing file differs; use --force to replace it ({exc})') from exc
            if not destination.is_file():
                raise RestoreError(f'{record.path}: existing destination is not a regular file')
        else:
            return 'SKIP'
    # Snapshot the verified compressed stream before touching a destination.
    # This remains streaming and avoids part changes between validation and use.
    with tempfile.TemporaryFile(prefix='ti60-large-archive-') as archive:
        check_archive(root, record, archive)
        archive.seek(0)
        destination.parent.mkdir(parents=True, exist_ok=True)
        safe_path(root, record.path)
        temporary = None
        try:
            with tempfile.NamedTemporaryFile(prefix='.' + destination.name + '.restore-',
                    suffix='.tmp', dir=destination.parent, delete=False) as output:
                temporary = Path(output.name)
                with gzip.GzipFile(fileobj=archive, mode='rb') as decompressed:
                    checked_copy(decompressed, record.size, record.sha256,
                                 record.path + ' (decompressed)', output)
                output.flush()
                os.fsync(output.fileno())
            os.chmod(temporary, record.mode)
            # Never replace a path that has become a symlink during processing.
            safe_path(root, record.path)
            if not force and destination.exists():
                try:
                    check_original(root, record)
                except (OSError, RestoreError) as exc:
                    raise RestoreError(f'{record.path}: destination appeared or changed during restore; use --force') from exc
                return 'SKIP'
            os.replace(temporary, destination)
            temporary = None
        finally:
            if temporary is not None:
                temporary.unlink(missing_ok=True)
    return 'RESTORED'


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--root', type=Path, default=Path(__file__).resolve().parents[1],
                        help='repository root (defaults to the parent of this tools directory)')
    parser.add_argument('--verify', action='store_true', help='read-only verification of originals and archive parts')
    parser.add_argument('--force', action='store_true', help='replace modified original files only after archive verification')
    args = parser.parse_args(argv)
    if args.verify and args.force:
        parser.error('--verify and --force cannot be combined')
    root = args.root.resolve()
    if not root.is_dir():
        parser.error('repository root does not exist or is not a directory')
    try:
        records = load_manifest(root)
    except (OSError, ValueError, RestoreError) as exc:
        print(f'ERROR manifest: {exc}', file=sys.stderr)
        return 1
    failed = 0
    for record in records:
        if args.verify:
            good = True
            for label, operation in [('original', check_original), ('archive', check_archive)]:
                try:
                    operation(root, record)
                except (OSError, ValueError, RestoreError) as exc:
                    print(f'ERROR {record.path} [{label}]: {exc}', file=sys.stderr)
                    good = False
            print(('VERIFIED' if good else 'FAILED') + ' ' + record.path)
            failed += not good
        else:
            try:
                print(restore(root, record, args.force) + ' ' + record.path)
            except (OSError, EOFError, ValueError, RestoreError, zlib.error) as exc:
                print(f'ERROR {record.path}: {exc}', file=sys.stderr)
                failed += 1
    print(f'{len(records)} file(s); {failed} failed; ' + ('verification only' if args.verify else 'restore complete'))
    return 1 if failed else 0


if __name__ == '__main__':
    raise SystemExit(main())
