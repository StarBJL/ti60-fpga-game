"""Export a verified Windows build to a private CI artifact branch.

The ASCII chunks allow authenticated GitHub connectors that cannot transfer
binary responses to retrieve the exact ZIP. They are not runtime dependencies.
"""
from pathlib import Path
import base64
import hashlib
import json
import zipfile

ROOT = Path(__file__).resolve().parents[2]
transport = ROOT / '_artifact_transport'
transport.mkdir(exist_ok=True)
archive = ROOT / 'dist/Ti60_R5_Windows_Console.zip'
files = sorted((ROOT / 'dist/windows').glob('*')) + [ROOT / 'verification/r5/windows_console/windows_exe_results.json']
with zipfile.ZipFile(archive, 'w', zipfile.ZIP_DEFLATED) as output:
    for path in files:
        if path.is_file(): output.write(path, path.relative_to(ROOT))
data = archive.read_bytes()
chunks = []
for index, offset in enumerate(range(0, len(data), 180000)):
    block = data[offset:offset + 180000]
    name = f'archive-{index:04d}.b64'
    (transport / name).write_text(base64.b64encode(block).decode('ascii'), encoding='ascii')
    chunks.append({'path':'_artifact_transport/' + name, 'bytes':len(block), 'sha256':hashlib.sha256(block).hexdigest()})
manifest = {'archive_path':str(archive.relative_to(ROOT)), 'archive_bytes':len(data), 'archive_sha256':hashlib.sha256(data).hexdigest(), 'chunks':chunks,
            'files': [{'path':str(path.relative_to(ROOT)), 'bytes':path.stat().st_size, 'sha256':hashlib.sha256(path.read_bytes()).hexdigest()} for path in files if path.is_file()]}
(transport / 'manifest.json').write_text(json.dumps(manifest, indent=2), encoding='utf-8')
print('Exported verified artifact:', len(data), 'bytes;', len(chunks), 'transport chunks')
