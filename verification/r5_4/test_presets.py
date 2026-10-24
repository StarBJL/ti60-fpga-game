from pathlib import Path
import sys,threading,time,unittest,tempfile
sys.path.insert(0,str(Path(__file__).resolve().parents[2]/'tools/control_panel'))
from server import Session
from r2_presets import PROFILES,command_plan

class Board:
    """In-memory protocol device; never opens a physical serial port."""
    def __init__(self,session):
        self.s=session;self.writes=[];self.sequence=1;self.epoch=1;self.ticking=True;self.alive=True
        self.nak=None;self.no_ack=None;self.ignore_band=False;self.omit_tag=None;self.old_epoch=False;self.reboot_at=None;self.bad_epoch=False
        self.seen=threading.Event()
        self.values=dict(scene=0,phase=4,count=100,step=1,bullet=25,square=40,alpha=0,vsync=1,hw=0,auto=1,
          stats=0,verbose=0,motion=0,table=0,bgm=1,tone=1,volume=90,resolution=1,output_w=1920,output_h=1080,
          pending=0,bars=1,submit_mode=3,band=0,cache=0,overlap=0,overlap_active=0,seed_table=0,seed_build=0,
          sort=0,kpath=0,rd_bursts=1,wr_bursts=2,rd_words=128,wide=1,profile=1)
        self.s.serial=self;self.s.running=True
        with self.s.lock:self.reply(99);self.sample()
        self.thread=threading.Thread(target=self.clock,daemon=True);self.thread.start()
    def emit(self,line):self.s._ingest(line)
    def reply(self,rid):
        v=self.values
        groups={'S4':['scene','phase','count','step','alpha','vsync','hw','auto','stats','bullet','square'],
                'A4':['motion','table','bgm','tone','volume','verbose'],
                'V5':['resolution','output_w','output_h','pending','bars'],
                'E5':['submit_mode','band','cache','overlap','overlap_active','seed_table','seed_build','sort','kpath','rd_bursts','wr_bursts','rd_words','wide']}
        for tag,fields in groups.items():
            if tag==self.omit_tag:continue
            extra=' supported=1' if tag=='V5' else ' accel_supported=1' if tag=='E5' else ''
            self.emit(f'{tag} id={rid} epoch={"invalid" if self.bad_epoch else self.epoch}'+extra+' '+' '.join(f'{k}={v[k]}' for k in fields))
    def sample(self):
        self.sequence+=1;n=self.sequence;v=self.values
        self.emit(f'M4 seq={n} epoch={0 if self.old_epoch else self.epoch} target={v["count"]} '+' '.join(f'{k}={x}' for k,x in v.items() if k not in ['profile','resolution','output_w','output_h','pending','bars']))
        self.emit(f'F4 seq={n} render10=600 logic10=600 present10=600 repeat10=0 scan10=600 valid=1')
        self.emit(f'P4 seq={n} profile={v["profile"]} logic_us=1000 submit_us=1000 wait_us=1000')
        self.emit(f'C4 seq={n} auto={v["auto"]} stats={v["stats"]} verbose={v["verbose"]} tone={v["tone"]} volume={v["volume"]}')
    def clock(self):
        while self.alive:
            time.sleep(.015)
            with self.s.lock:
                if self.ticking and self.s.serial is self:self.sample()
    def write(self,wire):
        parts=wire.decode().strip().split();rid=int(parts[0][1:]);name=parts[1];value=int(parts[2]) if len(parts)>2 else None
        self.writes.append((name,value));v=self.values
        if name==self.reboot_at:self.emit('=== TI60-R5.3 reboot ===');return len(wire)
        if name==self.no_ack:self.seen.set();return len(wire)
        if name==self.nak:self.emit(f'ACK id={rid} ok=0 code=invalid_or_unavailable');return len(wire)
        if name not in ('status','stop','move'):self.epoch+=1
        if name=='fast':v.update(hw=1,submit_mode=2,band=0 if self.ignore_band else 1,cache=1,overlap=1,seed_table=1,seed_build=1,sort=1,kpath=1,rd_bursts=16,wr_bursts=128,rd_words=64,wide=16781312,motion=1,table=1)
        elif name=='stress':v.update(phase=2,count=40000)
        elif name in v:v[name]=value
        if name=='scene':v['phase']=0
        if name=='resolution':v.update(output_w=1920 if value else 1280,output_h=1080 if value else 720)
        v['overlap_active']=int(v['hw'] and v['overlap'] and v['scene']!=2 and not v['resolution'])
        self.emit(f'ACK id={rid} ok=1 code=ok')
        if name not in ('move','stop'):self.reply(rid)
        return len(wire)
    def close(self):self.alive=False

class PresetTests(unittest.TestCase):
    def setUp(self):
        self.temp=tempfile.TemporaryDirectory();self.s=Session(self.temp.name)
        self.s.preset_ack_timeout=.18;self.s.preset_verify_timeout=.18;self.b=Board(self.s)
    def tearDown(self):
        self.s.disconnect();self.b.close();self.b.thread.join(.5)
        if self.s.preset_thread:self.s.preset_thread.join(.5)
        self.temp.cleanup()
    def settle(self):
        end=time.monotonic()+2
        while self.s.preset_busy() and time.monotonic()<end:time.sleep(.005)
        self.assertFalse(self.s.preset_busy());return self.s.preset['status']
    def test_all_six_from_degraded_configuration(self):
        for key,p in PROFILES.items():
            self.s.start_preset(key);self.assertEqual(self.settle(),'applied',self.s.preset)
            st=self.b.values
            self.assertEqual((st['hw'],st['submit_mode'],st['band'],st['cache'],st['kpath']),(1,2,1,1,1))
            self.assertEqual((st['rd_bursts'],st['wr_bursts'],st['rd_words']),(16,128,64))
            self.assertEqual((st['scene'],st['count'],st['vsync']),(p['scene'],p['count'],p['vsync']))
            self.assertEqual((st['auto'],st['alpha'],st['resolution'],st['bars'],st['bgm'],st['tone']),(0,1,0,0,0,0))
            self.assertEqual((st['stats'],st['verbose']),(1,1))
            self.assertTrue(self.s.preset_snapshot()['conditions_match'])
        self.assertFalse(any(n=='key' for n,_ in self.b.writes))
    def test_invalid_profile_sends_nothing(self):
        with self.assertRaises(ValueError):self.s.start_preset('not-a-profile')
        self.assertEqual(self.b.writes,[])
    def test_release_and_missing_telemetry_rejected(self):
        self.b.ticking=False
        with self.s.lock:self.s.collector.samples[-1]['profile']=0
        with self.assertRaises(ValueError):self.s.start_preset('squares_8800_free')
        self.assertEqual(self.b.writes,[])
    def test_reboot_invalidates_old_ready_sample(self):
        self.b.ticking=False
        with self.s.lock:self.s._ingest('=== TI60-R5.3 reboot ===');self.b.reply(100)
        with self.assertRaises(ValueError):self.s.start_preset('squares_8800_free')
        self.assertEqual(self.b.writes,[])
    def test_negative_ack_stops_remaining_commands(self):
        self.b.nak='fast';self.s.start_preset('carrot_16000_free')
        self.assertEqual(self.settle(),'failed');self.assertEqual(self.b.writes,[('stop',None),('fast',None)])
    def test_ack_timeout_and_concurrent_controls(self):
        self.b.no_ack='fast';self.s.start_preset('carrot_15000_free');self.assertTrue(self.b.seen.wait(.5))
        with self.assertRaises(ValueError):self.s.command('alpha',0)
        with self.assertRaises(ValueError):self.s.start_preset('squares_8800_free')
        self.s.command('status');self.s.command('move',0)
        self.assertEqual(self.settle(),'failed');self.assertNotIn(('resolution',0),self.b.writes)
    def test_old_matching_sample_cannot_confirm(self):
        self.b.ticking=False;self.s.start_preset('squares_8900_free')
        self.assertEqual(self.settle(),'failed');self.assertIn('测量窗口',self.s.preset['error'])
    def test_malformed_status_epoch_fails_instead_of_hanging(self):
        self.b.bad_epoch=True;self.s.start_preset('squares_8800_free')
        self.assertEqual(self.settle(),'failed');self.assertIn('epoch',self.s.preset['error'])
    def test_partial_status_cannot_confirm(self):
        self.b.omit_tag='E5';self.s.start_preset('squares_8800_free')
        self.assertEqual(self.settle(),'failed');self.assertIn('状态回报不完整',self.s.preset['error'])
    def test_wrong_effective_band_detected(self):
        self.b.ignore_band=True;self.s.start_preset('carrot_16000_free')
        self.assertEqual(self.settle(),'failed');self.assertIn('分带',self.s.preset['error'])
    def test_old_epoch_cannot_confirm(self):
        self.b.old_epoch=True;self.s.start_preset('carrot_15000_free')
        self.assertEqual(self.settle(),'failed')
    def test_disconnect_cancels_without_later_writes(self):
        self.b.no_ack='fast';self.s.start_preset('squares_8800_free');self.assertTrue(self.b.seen.wait(.5))
        self.s.disconnect();count=len(self.b.writes);time.sleep(.05)
        self.assertEqual(self.s.preset['status'],'cancelled');self.assertEqual(len(self.b.writes),count)
    def test_firmware_reboot_stops_sequence(self):
        self.b.reboot_at='fast';self.s.start_preset('squares_8800_free')
        self.assertEqual(self.settle(),'failed');self.assertIn('重启',self.s.preset['error'])
    def test_quantity_refinement_and_condition_drift_are_distinct(self):
        self.s.start_preset('carrot_15000_free');self.assertEqual(self.settle(),'applied')
        self.s.command('count',15001);time.sleep(.04)
        status=self.s.preset_snapshot();self.assertTrue(status['conditions_match']);self.assertEqual(status['current_target'],15001)
        self.assertEqual(self.s.collector.samples[-1]['r2_conditions_match'],1)
        self.assertEqual(self.s.collector.samples[-1]['r2_initial_target'],15000)
        self.s.command('alpha',0);time.sleep(.04)
        self.assertFalse(self.s.preset_snapshot()['conditions_match']);self.assertEqual(self.s.collector.samples[-1]['r2_conditions_match'],0)

if __name__=='__main__':unittest.main(verbosity=2)
