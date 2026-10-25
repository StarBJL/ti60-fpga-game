from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[2]/'tools/control_panel'))
from server import Collector,command_wire
c=Collector();c.feed('V5 supported=1 resolution=0 output_w=1280 output_h=720');c.feed('I5 id=0 supported=0 held=0 matrix_keys=0 keyboard_reports=0')
assert c.dualvideo and c.state['supported']==1 and c.state['input_supported']==0
assert command_wire(7,'coop',1)==b':7 coop 1\n'
assert command_wire(8,'video_recover')==b':8 video_recover\n'
assert command_wire(9,'video_dvi',1)==b':9 video_dvi 1\n'
for name,val in [('coop',2),('video_dvi',2)]:
 try:command_wire(10,name,val)
 except ValueError:pass
 else:raise AssertionError(name)
print('PASS typed co-op/recovery commands and independent video/input capability fields')
