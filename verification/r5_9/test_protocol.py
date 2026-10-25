from pathlib import Path
import sys
sys.path.insert(0,str(Path(__file__).resolve().parents[2]/'tools/control_panel'))
from server import Collector,command_wire
for name,value in [('input_diag',1),('matrix_cal',1),('matrix_bind',254)]:assert command_wire(7,name,value)==f':7 {name} {value}\n'.encode()
assert command_wire(7,'keyboard_sync')==b':7 keyboard_sync\n'
for name,value in [('input_diag',2),('matrix_cal',4),('matrix_bind',256),('matrix_bind',-1)]:
 try:command_wire(7,name,value)
 except ValueError:pass
 else:raise AssertionError((name,value))
c=Collector();c.feed('V5 supported=1 resolution=0');c.feed('I5 supported=1 matrix_keys=16384 keyboard_reports=0');c.feed('I9 input_diag_supported=1 matrix_events=1 matrix_last=16384 matrix_map=x0123456789abcdef keyboard_bytes=11 keyboard_tail=x57ab0100001a0000000000 keyboard_faults=0 keyboard_down=1')
c.feed('I5 supported=1 matrix_keys=0 keyboard_reports=1');c.feed('M4 seq=1 count=100');c.feed('F4 seq=1 valid=1 present10=600');c.feed('P4 seq=1 profile=0');row=c.feed('C4 seq=1 track=bgm_title')
assert row['matrix_keys']==0 and row['matrix_last']==16384 and row['matrix_events']==1
assert row['matrix_map']=='x0123456789abcdef' and row['keyboard_bytes']==11 and row['keyboard_down']==1
assert row['keyboard_tail']=='x57ab0100001a0000000000' and c.dualvideo
c.feed('=== TI60-R5.9-INPUT-CAL-20261025 ===');assert 'matrix_events' not in c.state
print('PASS input command bounds, sticky short-press history, leading-zero-safe raw maps/bytes, CSV propagation and reboot reset')
