"""Response isolation used by the co-op preflight; no serial device is opened."""
from pathlib import Path
import sys,json,tempfile
sys.path.insert(0,str(Path(__file__).resolve().parents[2]/'tools/control_panel'))
from server import Session
with tempfile.TemporaryDirectory() as tmp:
 s=Session(tmp);c=s.collector
 c.feed('ACK id=10 ok=1 code=ok')
 c.feed('S4 id=10 scene=1 phase=5 auto=0')
 c.feed('I5 id=10 supported=1 matrix_keys=0 keyboard_held=0')
 c.feed('I9 id=10 input_diag_supported=1 input_diag=1 matrix_cal=0 keyboard_down=0')
 c.feed('T7 id=10 scene=1 players=3')
 c.feed('I9 id=0 input_diag=0')
 snap=s.snapshot();r=snap['responses'][10]
 assert c.state['input_diag']==0 and r['values']['input_diag']==1
 assert set(r['tags'])=={'S4','I5','I9','T7'}
 assert not snap['connected'] and snap['boot']==0
 r['values']['input_diag']=9;r['tags'].append('BAD')
 assert c.responses[10]['values']['input_diag']==1 and 'BAD' not in c.responses[10]['tags']
 json.dumps(snap)
 c.feed('ACK id=11 ok=0 code=invalid_or_unavailable');assert 11 not in s.snapshot()['responses']
 c.feed('=== TI60-R5.9-INPUT-CAL-20261025 ===');snap=s.snapshot();assert snap['boot']==1 and not snap['responses']
print('PASS request-tagged snapshots, periodic-state isolation, detached JSON response values and reboot reset')
