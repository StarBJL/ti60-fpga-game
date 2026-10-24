from pathlib import Path
from types import SimpleNamespace as Port
from unittest.mock import patch
import sys,tempfile,unittest
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/'tools/control_panel'))
import serial_ports,server

def port(device,location=None,vid=0x0403,pid=0x6011,hwid='USB VID:PID=0403:6011',description='Quad RS232-HS',**extra):
    return Port(device=device,vid=vid,pid=pid,location=location,hwid=hwid,description=description,**extra)

class PortTests(unittest.TestCase):
    def describe(self,p,platform='linux'):
        return serial_ports.describe_port(p,platform,sys_tty='/does-not-exist-r5-test')
    def test_current_board_with_jtag_missing_and_system_ports_first(self):
        ports=[port('/dev/ttyS31',vid=None,pid=None,hwid='n/a',description='n/a'),port('/dev/ttyUSB0','3-4:1.0'),port('/dev/ttyUSB3','3-4:1.3'),port('/dev/ttyUSB2','3-4:1.2')]
        result=serial_ports.enumerate_ports(ports,'linux','/does-not-exist-r5-test')
        self.assertEqual(result[0]['device'],'/dev/ttyUSB2')
        self.assertEqual([p['device'] for p in result if p['recommended']],['/dev/ttyUSB2'])
        self.assertEqual(len(result),3)
    def test_usb_number_is_not_channel(self):
        self.assertTrue(self.describe(port('/dev/ttyUSB7','3-4:1.2'))['recommended'])
        self.assertFalse(self.describe(port('/dev/ttyUSB2','3-4:1.0'))['recommended'])
        self.assertFalse(self.describe(port('/dev/ttyUSB2',None))['recommended'])
    def test_real_interface_file_precedes_location(self):
        with tempfile.TemporaryDirectory() as d:
            root=Path(d);target=root/'devices'/'3-4:1.2';target.mkdir(parents=True)
            (target/'bInterfaceNumber').write_text('02\n')
            tty=root/'tty'/'ttyUSB9';tty.mkdir(parents=True);(tty/'device').symlink_to(target,target_is_directory=True)
            self.assertEqual(serial_ports.channel_number(port('/dev/ttyUSB9','3-4:1.0'),'linux',root/'tty'),2)
    def test_windows_explicit_channel_identifiers(self):
        for p in [port('COM12',hwid='USB\\VID_0403&PID_6011&MI_02'),port('COM8',hwid='FTDIBUS\\VID_0403+PID_6011+FT12345C\\0000'),port('COM7',description='FTDI Interface C')]:
            self.assertTrue(self.describe(p,'win32')['recommended'])
    def test_unknown_windows_port_is_not_guessed(self):
        self.assertFalse(self.describe(port('COM5',serial_number='FT12345C'),'win32')['recommended'])
    def test_other_usb_adapter_not_misclassified(self):
        p=port('/dev/ttyUSB2','3-4:1.2',vid=0x10c4,pid=0xea60)
        self.assertEqual(self.describe(p)['role'],'usb')
        self.assertFalse(self.describe(p)['recommended'])
    def test_two_boards_keep_both_candidates(self):
        result=serial_ports.enumerate_ports([port('/dev/ttyUSB7','3-4:1.2'),port('/dev/ttyUSB2','2-1:1.2')],'linux','/none')
        self.assertEqual(len([p for p in result if p['recommended']]),2)
    def test_duplicate_nodes_removed_natural_sort(self):
        result=serial_ports.enumerate_ports([port('COM10',vid=1),port('COM2',vid=1),port('COM2',vid=1)],'win32')
        self.assertEqual([x['device'] for x in result],['COM2','COM10'])
    def test_enumeration_never_opens_ports(self):
        with patch('serial.Serial',side_effect=AssertionError('must not open')),patch('serial.tools.list_ports.comports',return_value=[port('/dev/ttyUSB7','3-4:1.2')]):
            self.assertTrue(server.Session.ports()[0]['recommended'])
    def test_known_non_uart_rejected_before_open(self):
        p=self.describe(port('/dev/ttyUSB0','3-4:1.0'))
        with patch.object(server.Session,'ports',return_value=[p]),patch('serial.Serial') as opened:
            with self.assertRaisesRegex(ValueError,'通道 C'):server.Session('/tmp/unused').connect('/dev/ttyUSB0')
            opened.assert_not_called()
    def test_manual_pty_allowed(self):
        class Fake:
            def close(self):pass
        s=server.Session('/tmp/unused')
        with patch.object(server.Session,'ports',return_value=[]),patch('serial.Serial',return_value=Fake()) as opened,patch.object(s,'new_record'),patch('threading.Thread.start'):
            s.connect('/dev/pts/123');self.assertEqual(opened.call_args.kwargs['port'],'/dev/pts/123')
            s.serial.close();s.serial=None

if __name__=='__main__':unittest.main(verbosity=2)
