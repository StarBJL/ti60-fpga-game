"""Build isolated diagnostic firmware without editing the board application.

no_uart: keep sampling and the HUD, disable periodic UART reports.
no_stats: disable window sampling/reports and the extra queue probe; keep
          drawing, blit_wait(), HDMI flip, UART input, and the normal driver.
"""
from pathlib import Path
import hashlib
import os
import subprocess

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
GAME = ROOT / "par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game"
SDK = Path("E:/efinity-riscv-ide-2026.1")
MAKE = SDK / "build_tools/bin/make.exe"
MAIN = GAME / "src/main.c"
original = MAIN.read_bytes()
source = original.decode("utf-8")


def replace_once(text, old, new):
    if text.count(old) != 1:
        raise RuntimeError(f"Expected one matching block: {old!r}")
    return text.replace(old, new, 1)


no_uart = replace_once(
    source,
    "            print_window(&ws, &info, &gfx);",
    "            /* Diagnostic variant: periodic UART report disabled. */",
)

no_stats = replace_once(source, "        if (stats_reset) {", "        if (0 && stats_reset) {")
no_stats = replace_once(
    no_stats,
    "        if (use_hw) {\n            f.qend  = blit_queue_used();",
    "        if (0 && use_hw) {\n            f.qend  = blit_queue_used();",
)
no_stats = replace_once(
    no_stats,
    "        win_add(&hud_w, &f);\n        win_add(&uart_w, &f);",
    "        /* Diagnostic variant: measurement-window accumulation disabled. */",
)
no_stats = replace_once(
    no_stats,
    "        if (tend - hud_w.t0 >= STAT_PERIOD) {",
    "        if (0 && tend - hud_w.t0 >= STAT_PERIOD) {",
)
no_stats = replace_once(
    no_stats,
    "        if (tend - uart_w.t0 >= UART_PERIOD) {",
    "        if (0 && tend - uart_w.t0 >= UART_PERIOD) {",
)

# The frame-completion and presentation sequence must stay byte-for-byte equal.
start = source.index("        /* 5. tail_wait:")
end = source.index("        win_add(&hud_w, &f);", start)
presentation = source[start:end]
for name, text in (("no_uart", no_uart), ("no_stats", no_stats)):
    if presentation not in text:
        raise RuntimeError(f"Frame completion/presentation changed in {name}")
    if "game_tick(&game, in);" not in text:
        raise RuntimeError(f"Game update missing in {name}")
    text = replace_once(
        text,
        "    bsp_init();",
        f'    bsp_init();\n    bsp_printf("\\r\\n[FLICKER PROBE: {name}]\\r\\n");',
    )
    destination = HERE / name
    destination.mkdir(exist_ok=True)
    (destination / "main.c").write_bytes(text.encode("utf-8"))
    sources = [p.relative_to(GAME).as_posix() for p in sorted((GAME / "src").glob("*.c")) if p.name != "main.c"]
    sources += [Path(os.path.relpath(destination / "main.c", GAME)).as_posix(), "../common/start.S"]
    objdir = Path(os.path.relpath(destination / "build", GAME)).as_posix()
    env = os.environ.copy()
    env["PATH"] = str(SDK / "toolchain/bin") + os.pathsep + str(MAKE.parent) + os.pathsep + env.get("PATH", "")
    subprocess.run(
        [str(MAKE), "-j4", "DEBUG=no", "BENCH=yes", f"OBJDIR={objdir}", "CFLAGS_ARGS=-Isrc", "SRCS=" + " ".join(sources)],
        cwd=GAME, env=env, check=True,
    )
    digest = hashlib.sha256((destination / "build/game.bin").read_bytes()).hexdigest()
    print(f"{name}: SHA256 {digest}", flush=True)

if MAIN.read_bytes() != original:
    raise RuntimeError("Production main.c changed during the probe build")
print("PASS: production main.c unchanged; both variants built successfully", flush=True)
