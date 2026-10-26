"""Publish the tested Windows ZIP as recoverable chunks on a private delivery branch.
GitHub's artifact download redirect is unavailable on the build workstation;
UTF-8 chunks can be retrieved through the authenticated repository connector.
Never prints authentication values; does not modify the main branch.
"""
from pathlib import Path
import base64, hashlib, json, os, urllib.request, urllib.error, zipfile
ROOT=Path(__file__).resolve().parents[2]
def main():
    token=os.environ['GITHUB_TOKEN'];repo=os.environ['GITHUB_REPOSITORY']
    run=os.environ['GITHUB_RUN_ID'];source=os.environ['GITHUB_SHA']
    def api(path,data=None,method=None):
        raw=None if data is None else json.dumps(data).encode()
        req=urllib.request.Request('https://api.github.com/repos/'+repo+'/'+path,data=raw,method=method,
            headers={'Authorization':'Bearer '+token,'Accept':'application/vnd.github+json','X-GitHub-Api-Version':'2022-11-28','Content-Type':'application/json'})
        with urllib.request.urlopen(req,timeout=90) as res:return json.load(res)
    folder=ROOT/'dist/windows';exe=folder/'Ti60Control.exe'
    test=json.loads((ROOT/'verification/r5/windows_console/windows_exe_results.json').read_text())
    assert test['passed'] and test['windows_executed'] and exe.read_bytes()[:2]==b'MZ'
    archive=ROOT/'dist/Ti60_R6_Windows.zip'
    receipt={'version':'R6','source_commit':source,'workflow_run':run,'exe_sha256':hashlib.sha256(exe.read_bytes()).hexdigest(),'native_windows_checks':test}
    (folder/'BUILD_RECEIPT.json').write_text(json.dumps(receipt,indent=2),encoding='utf-8')
    with zipfile.ZipFile(archive,'w',zipfile.ZIP_DEFLATED,compresslevel=9) as z:
        for p in sorted(folder.iterdir()):
            if p.is_file():z.write(p,p.name)
    content=archive.read_bytes();entries=[];parts=[];base='r6/'+run
    for i,offset in enumerate(range(0,len(content),512*1024)):
        part=content[offset:offset+512*1024];name=f'{base}/Ti60_R6_Windows.part{i:03d}.b64'
        blob=api('git/blobs',{'encoding':'utf-8','content':base64.b64encode(part).decode()})
        entries.append({'path':name,'mode':'100644','type':'blob','sha':blob['sha']})
        parts.append({'path':name,'blob_sha':blob['sha'],'bytes':len(part),'sha256':hashlib.sha256(part).hexdigest()})
    manifest=dict(receipt,archive=archive.name,bytes=len(content),sha256=hashlib.sha256(content).hexdigest(),parts=parts)
    entries.append({'path':base+'/manifest.json','mode':'100644','type':'blob','content':json.dumps(manifest,indent=2)})
    branch='windows-delivery'
    try:parent=api('git/ref/heads/'+branch)['object']['sha']
    except urllib.error.HTTPError as e:
        if e.code!=404:raise
        parent=None
    tree=api('git/trees',dict(tree=entries,**({'base_tree':api('git/commits/'+parent)['tree']['sha']} if parent else {})))
    commit=api('git/commits',{'message':'Windows R6 native build '+run,'tree':tree['sha'],'parents':[parent] if parent else []})
    if parent:api('git/refs/heads/'+branch,{'sha':commit['sha'],'force':False},'PATCH')
    else:api('git/refs',{'ref':'refs/heads/'+branch,'sha':commit['sha']})
    print('Native Windows package:',archive.name,'bytes:',len(content),'SHA256:',manifest['sha256'])
    print('Delivery manifest:',base+'/manifest.json','branch:',branch)
if __name__=='__main__':main()
