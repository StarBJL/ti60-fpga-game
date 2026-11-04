#!/usr/bin/env python3
"""Non-destructive Efinity path, Markdown link and release checksum audit.

This is static validation, not synthesis, simulation or a board test.
"""
from __future__ import annotations
import argparse, hashlib, json, re, sys, xml.etree.ElementTree as ET
from pathlib import Path
from urllib.parse import unquote

ROOT = Path(__file__).resolve().parents[1]
PROJECT = ROOT / "par/ddr_demo_ti60/ddr_demo_ti60.xml"
GAME = ROOT / "par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game"
LINK = re.compile(r"(?<!!)\[[^\]]+\]\(([^)]+)\)")
errors = []
checks = 0

def require(ok, label):
    global checks
    checks += 1
    if not ok:
        errors.append(label)

def file_hash(path):
    h = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            h.update(block)
    return h.hexdigest()

def project_paths():
    require(PROJECT.is_file(), "missing Efinity project XML")
    if not PROJECT.is_file():
        return
    root = ET.parse(PROJECT)
    n = 0
    for item in root.iter():
        tag = item.tag.rsplit("}", 1)[-1]
        if tag not in ("design_file", "sdc_file", "peri_file", "ip"):
            continue
        rel = item.attrib.get("path" if tag == "ip" else "name")
        if rel:
            n += 1
            require((PROJECT.parent / rel).is_file(), f"missing {tag}: {rel}")
    for rel in (
        "rtl/ddr3_example_top.v",
        "rtl/blit/blit_engine.v",
        "rtl/blit/blit_axi_rd_arb.v",
        "rtl/hdmi/hdmi_fb.v",
        "rtl/hdmi/hdmi_island.v",
        "rtl/audio/audio_mixer.v",
        "par/ddr_demo_ti60/ip/soc/soc.v",
        "par/ddr_demo_ti60/embedded_sw/soc/bsp/efinix/EfxSapphireSoc/include/soc.h",
        "par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/game.ld",
    ):
        require((ROOT / rel).is_file(), "missing essential file: " + rel)
    print(f"Efinity: checked {n} project references and critical files")

def documentation():
    paths = [ROOT / "README.md", ROOT / "README_EN.md"]
    paths += sorted((ROOT / "docs").glob("[0-9][0-9]_*.md"))
    paths += [ROOT / "docs/MAINTENANCE_AND_SCOPE.md",
              ROOT / "docs/evidence/README.md"]
    for doc in paths:
        require(doc.is_file(), "missing document: " + str(doc))
        if not doc.is_file():
            continue
        for link in LINK.findall(doc.read_text(encoding="utf-8")):
            target = link.strip().split(" ", 1)[0].strip("<>")
            if target.startswith(("https:", "http:", "mailto:", "#")) or not target:
                continue
            target = unquote(target.split("#", 1)[0])
            if target:
                require((doc.parent / target).exists(),
                        f"broken link in {doc.relative_to(ROOT)}: {target}")
    print(f"Documentation: checked {len(paths)} Markdown files and local links")

def release_hashes():
    data = json.loads((ROOT / "CURRENT_RELEASE.json").read_text(encoding="utf-8"))
    targets = [(data["default_bit"], data["default_bit_sha256"])]
    targets += [(name, meta["sha256"]) for name, meta in data["compiled_elfs"].items()]
    targets += [(str((GAME / "sound/sound_bank.bin").relative_to(ROOT)),
                 data["voice_bank_sha256"])]
    for rel, expected in targets:
        p = ROOT / rel
        require(p.is_file(), "missing artifact: " + rel)
        if p.is_file():
            require(file_hash(p) == expected, "SHA256 mismatch: " + rel)
    # Match the byte ordering and path names of tools/build_verified.py.
    h = hashlib.sha256()
    paths = sorted((GAME / "src").glob("*")) + [
        GAME / "makefile", GAME / "sound/sound_bank.bin", GAME / "sound/sound_bank.S"
    ]
    for p in paths:
        if p.is_file():
            h.update(str(p.relative_to(GAME)).encode())
            with p.open("rb") as stream:
                for block in iter(lambda: stream.read(1024 * 1024), b""):
                    h.update(block)
    require(h.hexdigest() == data["source_digest"], "firmware source digest mismatch")
    print(f"Release: checked {len(targets)} artifact digests and one source digest")

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--hash", action="store_true", help="verify release artifacts and firmware digest")
    args = parser.parse_args()
    project_paths()
    documentation()
    if args.hash:
        release_hashes()
    if errors:
        for error in errors:
            print("FAIL: " + error, file=sys.stderr)
        print(f"FAIL: {len(errors)} errors / {checks} checks", file=sys.stderr)
        return 1
    print(f"PASS: {checks} static checks (not synthesis or board verification)")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
