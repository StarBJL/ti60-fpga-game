from pathlib import Path
import re,json,statistics
v=Path(__file__).parent;root=Path('D:/FPGA_competition/Ti60_720p_SQUARE20K_MERGE/logs')
result=[]
for f in sorted(root.glob('*/*.txt')):
 table=True;window=None;records=[];ab=[]
 for i,line in enumerate(f.read_text(encoding='utf-8').splitlines(),1):
  if line.startswith('[square packed table '):table=' ON]' in line
  if 'square A/B' in line:ab.append(dict(line=i,text=line))
  if line.startswith('fps '):window=dict(line=i,header=line,table=table)
  if window and line.startswith('  squares '):
   m=re.search(r'squares (\d+) (\d+)x(\d+): motion (\w+) avail (\d) err (\d) tick (\d+) cycles engine (\d+)',line)
   if m:
    a=m.groups();window.update(count=int(a[0]),motion=a[3],tick=int(a[6]),engine=int(a[7]));records.append(window)
 groups={}
 for r in records:
  h=r['header'];key=f"{r['count']} squares / {'HW draw' if '| HW' in h else 'CPU draw'} / motion {r['motion']} / table {r['table']} / {'FX' if ' fx ' in h else 'opaque'}"
  fps=float(re.search(r'fps (\d+\.\d+)',h)[1]);groups.setdefault(key,[]).append(fps)
 result.append(dict(file=str(f),ab=ab,windows={k:dict(n=len(a),min=min(a),max=max(a),median=statistics.median(a)) for k,a in groups.items()}))
(v/'r1_windows.json').write_text(json.dumps(result,ensure_ascii=False,indent=2),encoding='utf-8')
print(json.dumps(result[0],ensure_ascii=False,indent=2))
