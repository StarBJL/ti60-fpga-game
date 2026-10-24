from pathlib import Path
import sys,unittest,tempfile
sys.path.insert(0,str(Path(__file__).resolve().parents[2]/'tools/control_panel'))
from server import Session,command_wire,Collector
class VolumeTests(unittest.TestCase):
 def test_relative_wire_and_rejected_values(self):
  self.assertEqual(command_wire(3,'volume_delta',-1),b':3 volume_delta -1\n')
  self.assertEqual(command_wire(4,'volume_delta',1),b':4 volume_delta 1\n')
  for bad in [0,2,None,True]:
   with self.assertRaises(ValueError):command_wire(5,'volume_delta',bad)
 def test_old_firmware_is_not_sent_new_command(self):
  class Wire:
   def __init__(self):self.sent=[]
   def write(self,data):self.sent.append(data)
  with tempfile.TemporaryDirectory() as d:
   s=Session(d);w=Wire();s.serial=w;s.collector.compatible=True
   with self.assertRaises(ValueError):s.command('volume_delta',1)
   self.assertEqual(w.sent,[])
   s.collector.feed('E5 id=1 accel_supported=1 volume_step_supported=1')
   s.command('volume_delta',-1);s.command('volume_delta',-1);s.command('volume_delta',1)
   self.assertEqual([x.split()[1:] for x in w.sent],[[b'volume_delta',b'-1'],[b'volume_delta',b'-1'],[b'volume_delta',b'1']])
   s.collector.feed('=== TI60-R5.2 reboot ===')
   self.assertNotIn('volume_step_supported',s.collector.state)
   s.serial=None
if __name__=='__main__':unittest.main(verbosity=2)
