"""Windows-targeted contract checks run on Linux; not a Windows execution claim."""
from pathlib import Path
import csv
import importlib.util
import io
import json
import os
import sys
import tempfile
import threading
import unittest
import urllib.error
import urllib.request
from unittest.mock import patch

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / 'tools/control_panel'))
import server

class PortabilityTests(unittest.TestCase):
    def test_resolution_validation_and_capability(self):
        sent = []
        class Fake:
            def write(self, data): sent.append(data)
        session = server.Session('/tmp/unused-r5-test')
        session.serial = Fake()
        session.collector.feed('R4: ready')
        with self.assertRaises(ValueError): session.command('resolution', 1)
        session.collector.feed('=== TI60-R5-DUALVIDEO-20261024 ===')
        self.assertFalse(session.collector.compatible)
        session.collector.feed('R4: ready')
        session.collector.feed('V5 resolution=0 supported=0 pending=0')
        with self.assertRaises(ValueError): session.command('resolution', 1)
        session.collector.feed('V5 resolution=0 supported=1 pending=0 nominal_hz=60')
        self.assertTrue(session.snapshot()['dualvideo'])
        self.assertEqual(session.collector.state['output_hz'], 60)
        for value in (0, 1): session.command('resolution', value)
        for value in (2, -1, True, '1'):
            with self.assertRaises(ValueError): session.command('resolution', value)
        self.assertEqual(len(sent), 2)
        session.collector.feed('=== 720P-R4-CONTROL-LINUX ===')
        self.assertFalse(session.snapshot()['dualvideo'])

    def test_unicode_space_logs_and_excel_csv(self):
        with tempfile.TemporaryDirectory(prefix='Ti60 中文 路径 ') as temp:
            class Fake: pass
            session = server.Session(Path(temp) / '测量 记录')
            session.serial = Fake()
            record = session.new_record('Windows COM12 中文 空格')
            session.writer.writerow({'timestamp': '2026-10-24', 'scene': 2, 'track': '像素晨光', 'resolution': 1})
            session.event('MARK', text='中文标记')
            session._finish_record()
            self.assertEqual(json.loads((session.log_root / 'latest.json').read_text(encoding='utf-8'))['label'], session.label)
            name, data = session.export('metrics')
            self.assertTrue(data.startswith(b'\xef\xbb\xbf'))
            row = next(csv.DictReader(io.StringIO(data.decode('utf-8-sig'))))
            self.assertEqual(row['track'], '像素晨光')
            self.assertIn('中文标记', Path(record['events']).read_text(encoding='utf-8'))

    def test_windows_serial_options(self):
        # Exercise the Windows branch without claiming to emulate a Windows driver.
        session = server.Session('/tmp/unused-r5-test')
        seen = []
        class FakeSerial:
            def __init__(self, **kwargs): seen.append(kwargs)
            def close(self): pass
        with patch('serial.Serial', FakeSerial), patch.object(session, 'new_record'), patch('threading.Thread.start'), patch.object(server.os, 'name', 'nt'):
            session.connect('COM12')
        self.assertEqual(seen[0]['port'], 'COM12')
        self.assertNotIn('exclusive', seen[0])
        self.assertEqual(seen[0]['write_timeout'], .5)
        session.serial.close()
        session.serial = None

    def test_frozen_log_location(self):
        with tempfile.TemporaryDirectory(prefix='Ti60 AppData 中文 ') as temp:
            with patch.object(sys, 'frozen', True, create=True), patch.dict(os.environ, {'LOCALAPPDATA': temp}):
                self.assertEqual(server.default_log_root(), Path(temp) / 'Ti60R5/logs')

    def test_asset_and_api_http_with_ephemeral_port(self):
        with tempfile.TemporaryDirectory() as temp:
            session = server.Session(temp)
            http = server.make_server(session, 0)
            thread = threading.Thread(target=http.serve_forever, daemon=True)
            thread.start()
            base = f'http://127.0.0.1:{http.server_port}'
            try:
                for asset in ('/', '/app.js', '/style.css'):
                    with urllib.request.urlopen(base + asset) as response:
                        self.assertEqual(response.status, 200)
                        self.assertTrue(response.read())
                with urllib.request.urlopen(base + '/api/state') as response:
                    state = json.load(response)
                    self.assertFalse(state['connected'])
                    self.assertFalse(state['dualvideo'])
                bad = urllib.request.Request(base + '/api/command', data=b'{"command":"resolution","value":1}', headers={'Content-Type':'application/json'})
                with self.assertRaises(urllib.error.HTTPError) as error:
                    urllib.request.urlopen(bad)
                self.assertEqual(error.exception.code, 403)
            finally:
                http.shutdown()
                http.server_close()
                thread.join(timeout=3)

    def test_video_fields_in_metrics(self):
        collector = server.Collector()
        collector.feed('V5 resolution=1 output_w=1920 output_h=1080 nominal_hz=30 render_w=1280 render_h=720 scale=1.5 pending=0 supported=1')
        collector.feed('M4 seq=9 scene=2 count=100 target=100 step=10')
        collector.feed('F4 seq=9 elapsed_us=1000000 render10=300 logic10=600 present10=300 repeat10=0 scan10=300 valid=1')
        collector.feed('P4 seq=9 work_us=1000 profile=0')
        row = collector.feed('C4 seq=9 track=bgm_stress w=1280 h=720')
        self.assertEqual(row['output_w'], 1920)
        self.assertEqual(row['output_hz'], 30)
        self.assertEqual(row['render_w'], 1280)
        self.assertEqual(row['present_fps'], 30)

if __name__ == '__main__':
    unittest.main(verbosity=2)
