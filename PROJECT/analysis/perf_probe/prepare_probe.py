"""Generate a counted copy of game.c. Original board sources are not edited."""
from pathlib import Path
import hashlib

here = Path(__file__).resolve().parent
project = here.parents[1]
source = project / 'par/ddr_demo_ti60/embedded_sw/soc/software/standalone/game/src/game.c'
raw = source.read_bytes()
text = raw.decode('utf-8')
start = text.index('static Bullet *take_limit_shot(Game *g) {')
end = text.index('static void hostile_shot(', start)
part = text[start:end]
needle = '    for (i=0;i<n;i++) {'
assert part.count(needle) == 1
part = part.replace('    int i,n,start;', '    int i,n,start;\n    ++probe_alloc_calls;', 1)
part = part.replace(needle, needle + '\n        ++probe_slot_checks;', 1)
out = ('extern unsigned long long probe_slot_checks, probe_alloc_calls;\n' +
       text[:start] + part + text[end:])
(here / 'game.instrumented.c').write_text(out, encoding='utf-8')
print('Original game.c SHA256:', hashlib.sha256(raw).hexdigest())
print('Generated:', here / 'game.instrumented.c')
