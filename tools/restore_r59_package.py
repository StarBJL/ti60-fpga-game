#!/usr/bin/env python3
"""Reassemble the R5.9 archive and restore exact bit/ELF files after Git clone.
Only named, SHA256-verified board artifacts are copied, never arbitrary tar
members. Existing different files are left untouched and reported as errors.
"""
from pathlib import Path
import hashlib,json,os,tempfile,tarfile
r=Path(__file__).resolve().parents[1];m=json.loads((r/'dist/r59_package_parts.json').read_text());target=r/m['path']
def sha(data):return hashlib.sha256(data).hexdigest()
def install(path,data,want):
 assert sha(data)==want,'SHA256 mismatch: '+str(path)
 if path.exists():
  if sha(path.read_bytes())!=want:raise SystemExit('Existing file differs; move it aside before restoring: '+str(path))
  return
 path.parent.mkdir(parents=True,exist_ok=True)
 with tempfile.NamedTemporaryFile(dir=path.parent,prefix='r59-restore-',delete=False) as f:
  temp=Path(f.name)
  try:
   f.write(data);f.flush();os.fsync(f.fileno());os.replace(temp,path)
  finally:
   if temp.exists():temp.unlink()
if target.exists():
 if sha(target.read_bytes())!=m['sha256']:raise SystemExit('Existing archive differs; move it aside before restoring.')
else:
 data=bytearray()
 for part in m['parts']:
  chunk=(r/part['path']).read_bytes();assert len(chunk)==part['bytes'] and sha(chunk)==part['sha256'];data.extend(chunk)
 assert len(data)==m['bytes'];install(target,data,m['sha256'])
manifest=json.loads((r/'BOARD_TEST/r5_9/manifest.json').read_text())
with tarfile.open(target) as archive:
 for rel,info in manifest['artifacts'].items():
  if Path(rel).suffix not in ['.bit','.elf']:continue
  dest=r/'BOARD_TEST/r5_9'/rel
  assert not Path(rel).is_absolute() and '..' not in Path(rel).parts
  member=archive.getmember('Ti60_R5_9_INPUT_CAL/BOARD_TEST/r5_9/'+rel)
  assert member.isfile() and member.size==info['bytes'];data=archive.extractfile(member).read()
  install(dest,data,info['sha256']);print('Verified:',dest.relative_to(r))
print('R5.9 archive and all matched bit/ELF files restored and SHA256 verified.')
