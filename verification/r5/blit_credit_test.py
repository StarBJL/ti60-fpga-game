#!/usr/bin/env python3
"""Integrated credit conservation and queue bound check; real RTL AXI model."""
from pathlib import Path
import json
import sys

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "PROJECT/sim/blit"))
import tb_blit as T

tb = T.TB(105, "random")
s = tb.sim
clock = s.clock
state = dict(reserved=0, cycles=0, max_reserved=0, max_descriptors=0,
             max_list_reserved=0, issue_pop_same_cycle=0, cache_fill=0)

def checked_clock():
    if s["rst"]:
        state["reserved"] = 0
    else:
        flags = [s["u_blit." + k] for k in ("f_ar", "l_issue", "kb_fill")]
        assert sum(flags) <= 1, "AR issue classes are not exclusive"
        inc = flags[0] * s["u_blit.f_arw"] + flags[1] * s["u_blit.l_len"] + flags[2] * 8
        dec = s["u_blit.rf_pop"]
        state["reserved"] += inc - dec
        assert 0 <= state["reserved"] <= 512, state
        state["issue_pop_same_cycle"] += int(bool(inc and dec))
        state["cache_fill"] += flags[2]
    clock()
    s.comb()
    assert s["u_blit.rf_eff"] == state["reserved"], (s["u_blit.rf_eff"], state)
    assert s["u_blit.lf_inflight"] <= 32
    assert s["u_blit.bd_count"] <= 17
    state["cycles"] += 1
    state["max_reserved"] = max(state["max_reserved"], state["reserved"])
    state["max_descriptors"] = max(state["max_descriptors"], s["u_blit.bd_count"])
    state["max_list_reserved"] = max(state["max_list_reserved"], s["u_blit.lf_inflight"])

s.clock = checked_clock
tb.reset()
for ga in (0, 1, 127, 128, 254, 255):
    for apl in (0, 1):
        c = dict(dst=T.DST_BASE+2, dstr=1280, src=T.SRC_BASE+6, sstr=1280,
                 aad=T.A_BASE+3, astr=640, w=639, h=3, color=0x5a15,
                 key=0xf81f, op=1, key_en=1, ga=ga, apl=apl, cache=1,
                 sync=0, add=apl)
        tb.push(c)
tb.run_until_idle()
T.check(tb, "credit + Alpha command mix")
tb.apb_write(T.R_LADDR, T.LIST_BASE)
tb.rand_list(120)
tb.run_until_idle()
T.check(tb, "credit + list/TAB traffic")
# The integrated band suite reaches K line fills, evictions and ordered overlap.
T.band_tests(tb, {"caps": 0xffffffff})
assert state["reserved"] == 0
assert state["issue_pop_same_cycle"] > 0
assert state["cache_fill"] > 0
state["result"] = "PASS"
state["hardware_test"] = False
(ROOT / "verification/r5/blit_credit_result.json").write_text(json.dumps(state, indent=2)+"\n")
print("TEST_PASS integrated credit conservation", json.dumps(state, sort_keys=True))
