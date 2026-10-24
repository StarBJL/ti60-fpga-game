"""Small workload study using the repository's Python RTL simulator.

This is an ideal-slave model, not a measurement of the SoC/DDR controller.
APB commands are supplied faster than software; HDMI requests are disabled.
"""
from pathlib import Path
import sys
here = Path(__file__).resolve().parent
sys.path.insert(0, str(here.parents[1] / 'sim/blit'))
from tb_blit import TB, golden, check, R_DST, R_CMD, SRC_BASE, A_BASE

for kind, width, height, op, alpha in (
    ('pixel_fill', 1, 1, 0, 255),
    ('bullet_copy', 9, 9, 1, 255),
    ('bullet_alpha', 9, 9, 1, 128),
):
    tb = TB(123, 'ideal')
    tb.reset()
    tb.hd_next = 10**9
    # reset() does not use step(); no outstanding fake HDMI is expected.
    assert tb.hd_pending is None and not tb.hd_expect
    ncmd = 24
    c = dict(w=width,h=height,dst=0x1000,dstr=1280,src=SRC_BASE,sstr=18,
             aad=A_BASE,astr=9,color=0x1234,key=0xF81F,op=op,
             key_en=op,apl=0,ga=alpha,sync=0)
    tb.push(c)
    for i in range(1,ncmd):
        c['dst'] = 0x1000+i*20
        tb.apb_write(R_DST,c['dst'])
        tb.apb_write(R_CMD,op | (op << 1) | (alpha << 8))
        golden(tb.ref,c)
    cycles = tb.run_until_idle()
    check(tb,kind)
    print('%s n=%d cycles=%d cycles_per_cmd=%.2f AR=%d AW=%d rbeats=%d wbeats=%d PASS' %
          (kind,ncmd,cycles,cycles/ncmd,tb.stats['ar'],tb.stats['aw'],
           tb.stats['rbeats'],tb.stats['wbeats']),flush=True)
