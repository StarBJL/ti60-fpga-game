"""Post-route checks. Optional --cdc runs an audit timing session with clock
 groups removed, without editing the constraints used to build any bit file.
 This reports physical skew; it is NOT a blanket CDC functional proof.
"""
from pathlib import Path
import subprocess,json,re,sys,os,hashlib
v=Path(__file__).resolve().parent;base=v.parents[1]
ef=Path(os.environ.get('EFINITY_HOME','/home/user/fpga_contest/vivado/efinity-2026.1.132-linux-x64/efinity/2026.1'))
env=dict(os.environ,EFINITY_HOME=str(ef),EFXPT_HOME=str(ef/'pt'),EFXPGM_HOME=str(ef/'pgm'),EFXDBG_HOME=str(ef/'debugger'),EFXIPM_HOME=str(ef/'ipm'),PYTHONHOME=str(ef),PYTHONPATH=str(ef/'lib'),PYTHONNOUSERSITE='1',QT_PLUGIN_PATH=str(ef/'lib/plugins'))
env['EFINITY_USER_DIR_INI']=str(Path.home()/'.local/share/efinity/user_dir.ini')
env['PATH']=str(ef/'bin')+':'+str(ef/'scripts')+':'+env['PATH']
roots=[Path(a).resolve() for a in sys.argv[1:] if a!='--cdc'] or [base,base.parent/'clock_202',base.parent/'clock_205'];results=[]
for root in roots:
 p=root/'PROJECT/par/ddr_demo_ti60';o=root/'verification/r5_8';o.mkdir(parents=True,exist_ok=True)
 text=(p/'outflow/ddr_demo_ti60.timing.rpt').read_text()
 section=re.search(r'Clock Relationship Summary \(begin\)(.*?)Clock Relationship Summary \(end\)',text,re.S).group(1)
 relations=re.findall(r'^\s*(\S+)\s+(\S+)\s+(-?\d+\.\d+)\s+(-?\d+\.\d+)\s+\([RF]-[RF]\)',section,re.M)
 assert len(relations)>=14 and all(float(a[3])>=0 for a in relations)
 counts=(p/'outflow/ddr_demo_ti60.map.rpt').read_text();resource_lines=[s for s in counts.splitlines() if re.search(r'\b(XLR|DSP|RAM|LUT|FF)\b.*(?:/|\d)',s) and len(s)<180]
 d={'profile':root.name,'relationships':[{'launch':a,'capture':b,'constraint_ns':float(c),'slack_ns':float(s)} for a,b,c,s in relations],'worst_slack_ns':min(float(a[3]) for a in relations),'bit_sha256':hashlib.sha256((p/'outflow/ddr_demo_ti60.bit').read_bytes()).hexdigest(),'resource_report_lines':resource_lines,'hardware_tested':False}
 for name,path in [('retime',base/'PROJECT/sim/blit/check_retime.py'),('rd_words',base/'PROJECT/sim/blit/check_netlist_cone.py'),('rf_res',base/'verification/r5/blit_check_rf_res_netlist.py')]:
  result=subprocess.run([sys.executable,str(path),str(p/'outflow/ddr_demo_ti60.map.v')],capture_output=True,text=True);(o/f'netlist_{name}.log').write_text(result.stdout+result.stderr);assert result.returncode==0,result.stdout[-4000:]+result.stderr[-4000:]
 if '--cdc' in sys.argv:
  sdc=(p/'sdc/ddr3.sdc').read_text()
  hz=json.loads((root/'clock_candidate.json').read_text())['actual_hz'] if (root/'clock_candidate.json').exists() else 200000000
  bound=0.9e9/hz
  net=(p/'outflow/ddr_demo_ti60.map.v').read_text()
  ff=re.findall(r'EFX_FF\s+\\(\S+)\s*\(',net)
  groups={}
  for name in ff:
   if (('Gray_buffercc/buffers_0[' in name) or ('/rgray_w1[' in name) or ('/wgray_r1[' in name)):
    endpoint=re.sub(r'\[\d+\]','*',name)+'|D'
    groups[endpoint]=groups.get(endpoint,0)+1
  assert len(groups)==18,(root,groups)
  for endpoint in groups:
   scope=endpoint.split('/pushToPopGray_')[0].split('/popToPushGray_')[0]
   if '/rgray_w1' in scope:scope=scope.split('/rgray_w1')[0]
   if '/wgray_r1' in scope:scope=scope.split('/wgray_r1')[0]
   sdc+=f'\nset_bus_skew -from [get_pins {{{scope}/*|CLK}}] -to [get_pins {{{endpoint}}}] {bound:.9f}\n'
  d['audited_gray_groups']=groups
  (o/'cdc_no_groups.sdc').write_text('\n'.join(s for s in sdc.splitlines() if not s.lstrip().startswith('set_clock_groups'))+'\n')
  (o/'cdc.tcl').write_text(f'reset_timing\nread_sdc {{{o}/cdc_no_groups.sdc}}\nreport_bus_skew -setup -npaths 20 -file clock_cdc_setup.rpt\nreport_bus_skew -hold -npaths 20 -file clock_cdc_hold.rpt\n')
  with (o/'cdc.log').open('w') as f:subprocess.run([str(ef/'bin/python3'),str(ef/'scripts/efx_run.py'),'ddr_demo_ti60.xml','--prj','-f','sta_tclsh','--tcl_script',str(o/'cdc.tcl')],cwd=p,env=env,stdout=f,stderr=subprocess.STDOUT,check=True)
  d['bus_skew']={}
  for k in ['setup','hold']:
   a=(p/f'outflow/clock_cdc_{k}.rpt').read_text();(o/f'clock_cdc_{k}.rpt').write_text(a)
   nums=re.findall(r'^\s*\|\s*\|\s+(?:Slow|Fast)\s+\|\s+([\d.]+)\s+\|\s+([\d.]+)\s+\|\s+(-?[\d.]+)',a,re.M)
   d['bus_skew'][k]={'reported_groups':len(nums),'requirements_ns':[float(x) for x,y,z in nums],'skew_ns':[float(y) for x,y,z in nums],'slack_ns':[float(z) for x,y,z in nums]}
   assert len(nums)==len(groups) and all(float(z)>=0 for x,y,z in nums),(root,k,nums)
 (o/'clock_qualification.json').write_text(json.dumps(d,indent=2)+'\n');results.append(d);print(root.name,'PASS',d['worst_slack_ns'],flush=True)
(v/'clock_summary.json').write_text(json.dumps(results,indent=2)+'\n')
