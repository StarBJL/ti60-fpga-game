from pathlib import Path
import sys,unittest,tempfile
from unittest.mock import patch
r=Path(__file__).resolve().parents[2];sys.path.insert(0,str(r/'tools/control_panel'));import server
class R6Console(unittest.TestCase):
 def test_wire_and_bounds(self):
  for name,values in {'vpipe':[0,1],'kmode':[0,1,2],'square_path':[0,1,2,3],'stress_stage':[0,1]}.items():
   for v in values:self.assertEqual(server.command_wire(51,name,v),f':51 {name} {v}\n'.encode())
   for v in [-1,max(values)+1,True,None]:
    with self.assertRaises(ValueError):server.command_wire(51,name,v)
 def test_capabilities_recorded(self):
  c=server.Collector();c.feed('=== TI60-R6 test ===');c.feed('E5 id=9 pipeline_supported=1 vpipe=1 kblend=1 kmode=0 ftab=1 square_path=2 th_stage=1 seed_bytes=16 th_hot_bytes=16')
  c.feed('M4 seq=1 scene=2 count=17000');c.feed('F4 seq=1 valid=0');c.feed('P4 seq=1 profile=1');row=c.feed('C4 seq=1 track=bgm_title')
  for k in ['vpipe','kmode','ftab','square_path','th_stage','seed_bytes','th_hot_bytes']:
   self.assertIn(k,server.FIELDS);self.assertEqual(row[k],c.state[k])
 def test_double_click_opens_browser(self):
  class HTTP:
   server_port=12345
   def serve_forever(self,**kw):raise KeyboardInterrupt
   def server_close(self):pass
  with tempfile.TemporaryDirectory() as folder,patch.object(server.sys,'argv',['Ti60Control.exe','--log-root',folder]),patch.object(server.sys,'frozen',True,create=True),patch.object(server,'make_server',return_value=HTTP()),patch.object(server.webbrowser,'open') as op:
   server.main();op.assert_called_once_with('http://127.0.0.1:12345')
if __name__=='__main__':unittest.main()
