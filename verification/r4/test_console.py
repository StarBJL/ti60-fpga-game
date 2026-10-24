"""Protocol/recording checks independent of the browser fixture."""
from pathlib import Path
import sys,unittest,json
ROOT=Path(__file__).resolve().parents[2];sys.path.insert(0,str(ROOT/'tools/control_panel'))
from server import Collector,Session,command_wire
class ConsoleTests(unittest.TestCase):
 def test_commands(self):
  for step in [100,10,1]:self.assertEqual(command_wire(12,'step',step),f':12 step {step}\n'.encode())
  for args in [('step',1000),('delta',0),('move',1024),('scene','2'),('status',1),('oops',1)]:
   with self.assertRaises(ValueError):command_wire(1,*args)
 def test_no_legacy_injection(self):
  class Fake:
   def write(self,b):raise AssertionError('legacy device received framed data')
  s=Session(ROOT/'verification/r4/unit_logs');s.serial=Fake();s.collector.feed('=== 720P-MERGE-R3-R19-FULL ===');
  with self.assertRaises(ValueError):s.command('count',100)
 def test_fragments(self):
  c=Collector();self.assertIsNone(c.feed('M4 seq=1 scene=2 count=100 target=100 step=1'))
  self.assertIsNone(c.feed('F4 seq=2 elapsed_us=1000000 render10=800 logic10=600 present10=600 scan10=600 valid=1'))
  self.assertIsNone(c.feed('P4 seq=1 work_us=1000 profile=0'))
  self.assertIsNone(c.feed('C4 seq=1 track=bgm_stress'))
  row=c.feed('F4 seq=1 elapsed_us=1000000 render10=800 logic10=600 present10=0 repeat10=0 scan10=600 valid=0')
  self.assertEqual(row['render_fps'],80);self.assertEqual(row['logic_hz'],60);self.assertIsNone(row['present_fps']);self.assertIsNone(row['bands_us'])
  self.assertEqual(len(c.samples),1);self.assertIsNone(c.feed('M4 seq=1 scene=3 count=3'))
 def test_state_target(self):
  c=Collector();c.feed('M4 seq=1 scene=1 count=123 target=14000');c.feed('S4 id=1 scene=1 count=15000 cap=40000');self.assertEqual(c.state['count'],123);self.assertEqual(c.state['target'],15000)
 def test_firmware_trace(self):
  c=Collector();rows=[]
  for line in (ROOT/'verification/r4/main_uart.log').read_text().splitlines():
   row=c.feed(line)
   if row:rows.append(row)
  self.assertTrue(rows);self.assertTrue(all(r['drops']==0 for r in rows));self.assertTrue(all(r['present_fps']<=r['scan_hz']+.2 for r in rows));self.assertTrue(all(a['ok'] for a in c.acks))
  (ROOT/'verification/r4/telemetry_validation.json').write_text(json.dumps({'passed':True,'complete_windows':len(rows),'command_acknowledgements':len(c.acks),'dropped_uart_lines':0,'model_not_hardware_fps':True},indent=2))
if __name__=='__main__':unittest.main(verbosity=2)
