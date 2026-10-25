"""Describe ports without opening them; Ti60 V4 uses FT4232H channel C for UART.

Board schematic: CDBUS0/1 -> UART_TX_3V3/UART_RX_3V3; channel B is
OpenOCD JTAG. Device numbers (ttyUSB2, COM7, etc.) are never channel IDs.
USB identity is a connection hint, not proof that R5 firmware is running.
"""
from pathlib import Path, PosixPath
import re
import sys

CONSOLE_VERSION = 'R5.9.1-COOP-INPUT'
FT4232 = (0x0403, 0x6011)

def channel_number(port, platform=None, sys_tty='/sys/class/tty'):
    if (getattr(port,'vid',None),getattr(port,'pid',None)) != FT4232:
        return None
    platform = sys.platform if platform is None else platform
    if platform.startswith('linux'):
        # bInterfaceNumber is authoritative, even if other USB serial adapters
        # have changed the /dev/ttyUSB numbering or JTAG has detached channel B.
        device=str(getattr(port,'device',''))
        if re.fullmatch(r'/dev/ttyUSB\d+',device):
            path=PosixPath(sys_tty)/device.rsplit('/',1)[-1]/'device'
            try:
                resolved=path.resolve()
                for parent in (resolved,*resolved.parents):
                    number=parent/'bInterfaceNumber'
                    if number.is_file():
                        value=int(number.read_text().strip(),16)
                        return value if 0<=value<4 else None
            except (OSError,ValueError):
                pass
        location=str(getattr(port,'location','') or '')
        match=re.fullmatch(r'[\d.]+-[\d.]+:\d+\.([0-3])',location)
        if match:return int(match[1])
    hwid=str(getattr(port,'hwid','') or '')
    match=re.search(r'\bMI_0([0-3])(?:\b|[\\&])',hwid,re.I)
    if match:return int(match[1])
    # Windows FTDI hardware IDs can explicitly append the channel to the FTDI
    # device instance. Do not infer it from a bare serial number ending in C.
    match=re.search(r'FTDIBUS\\VID_0403\+PID_6011\+[^\\\s]+([ABCD])\\',hwid,re.I)
    if match:return ord(match[1].upper())-ord('A')
    for name in ('interface','description'):
        match=re.search(r'\b(?:interface|channel)\s*([ABCD])\b',str(getattr(port,name,'') or ''),re.I)
        if match:return ord(match[1].upper())-ord('A')
    return None

def describe_port(port,platform=None,sys_tty='/sys/class/tty'):
    device=str(port.device)
    description=str(getattr(port,'description','') or '未知设备')
    vid,pid=getattr(port,'vid',None),getattr(port,'pid',None)
    channel=channel_number(port,platform,sys_tty)
    legacy=bool(re.fullmatch(r'/dev/ttyS\d+',device)) and description in ('n/a','未知设备') and getattr(port,'hwid',None) in ('n/a',None,'')
    role='system' if legacy else 'usb' if vid is not None else 'unknown'
    hint='系统串口；不是自动识别到的 Ti60 日志口' if legacy else description
    if (vid,pid)==FT4232:
        if channel==2:
            role='ti60_uart_candidate';hint='FT4232H 通道 C · Ti60 V4 日志口候选'
        elif channel is not None:
            role='ftdi_non_uart'
            hint='FT4232H 通道 '+chr(65+channel)+(' · JTAG / OpenOCD，非本板日志口' if channel==1 else ' · 非 Ti60 V4 日志口')
        else:
            role='ftdi_unknown';hint='FT4232H · 通道未知，请核对通道 C；不能按端口编号判断'
    return {'device':device,'description':description,'label':hint,'role':role,
            'channel':chr(65+channel) if channel is not None else None,
            'vid':vid,'pid':pid,'serial_number':getattr(port,'serial_number',None),
            'location':getattr(port,'location',None),'recommended':role=='ti60_uart_candidate'}

def enumerate_ports(ports=None,platform=None,sys_tty='/sys/class/tty'):
    if ports is None:
        from serial.tools import list_ports
        ports=list_ports.comports()
    result=[];seen=set()
    for port in ports:
        item=describe_port(port,platform,sys_tty)
        if item['role']=='system' or item['device'] in seen:continue
        seen.add(item['device']);result.append(item)
    priority={'ti60_uart_candidate':0,'usb':1,'unknown':2,'ftdi_unknown':3,'ftdi_non_uart':4}
    def natural(device):return tuple((1,int(p)) if p.isdigit() else (0,p.lower()) for p in re.split(r'(\d+)',device))
    return sorted(result,key=lambda p:(priority.get(p['role'],5),natural(p['device'])))
