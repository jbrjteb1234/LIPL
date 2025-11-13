# amalgate.py — build ALL.c from your tree (locals only), Python 3.8 compatible
import re
import os
from pathlib import Path
from typing import Optional

ROOT = Path(__file__).resolve().parent
LOCAL_INC_RE = re.compile(r'^[ \t]*#[ \t]*include[ \t]*"([^"]+)"[ \t]*$', re.M)

def read_text(p: Path) -> str:
    return p.read_text(encoding='utf-8', errors='replace')

# ---- resolve & inline local headers once --------------------------------
_emitted = set()  # type: set

def _resolve_local(inc: str, base: Path) -> Optional[Path]:
    # 1) relative to including file
    p = (base.parent / inc).resolve()
    if p.exists():
        return p
    # 2) relative to project root
    p = (ROOT / inc).resolve()
    if p.exists():
        return p
    # 3) best-effort by basename anywhere under ROOT
    matches = list(ROOT.rglob(os.path.basename(inc)))
    return matches[0] if matches else None

def inline_once(h: Path) -> str:
    h = h.resolve()
    if h in _emitted:
        return "/* already inlined: {} */\n".format(h.relative_to(ROOT))
    _emitted.add(h)

    txt = read_text(h)

    def repl(m):
        inc = m.group(1)
        tgt = _resolve_local(inc, base=h)
        if not tgt:
            return '/* missing local header: "{}" */'.format(inc)
        return inline_once(tgt)

    txt = LOCAL_INC_RE.sub(repl, txt)
    rel = h.relative_to(ROOT)
    return (
        "/* ===== BEGIN HEADER: {} ===== */\n".format(rel) +
        '#line 1 "{}"\n'.format(rel) +
        txt + "\n" +
        "/* ===== END HEADER: {} ===== */\n".format(rel)
    )

# ---- headers & sources (from your screenshot) ----------------------------
headers = [
    # ast
    "ast/routines.h",
    "ast/slist_functions.h",
    "ast/table_management/table.h",
    "ast/token_converter.h",
    "ast/parser.h",
    "ast/reducer.h",
    "ast/states.h",
    # grammar
    "grammar/grammar.h",
    # lexer
    "lexer/lexeme.h",
    "lexer/lexer_main.h",
    "lexer/token_dictionary.h",
    "lexer/token_type.h",
    "lexer/token.h",
    # utility
    "utility/data_pool.h",
    "utility/safe_memory.h",
    "utility/stack.h",
]

sources = [
    # ast
    "ast/routines.c",
    "ast/slist_functions.c",
    "ast/table_management/table.c",
    "ast/token_converter.c",
    "ast/reducer.c",
    "ast/states.c",
    "ast/parser.c",
    # lexer
    "lexer/lexeme.c",
    "lexer/lexer_main.c",
    "lexer/token_dictionary.c",
    # utility
    "utility/data_pool.c",
    "utility/safe_memory.c",
    "utility/stack.c",
    # app
    "main.c",
]

# ---- build ALL.c ---------------------------------------------------------
parts = ["/* AMALGAMATED VIEW (project-local headers only) */\n"]

for h in headers:
    p = ROOT / h
    if p.exists():
        parts.append(inline_once(p))
    else:
        parts.append("/* header not found, skipped: {} */\n".format(h))

for s in sources:
    p = ROOT / s
    if p.exists():
        src = read_text(p)
        # strip local includes from .c files (headers already inlined)
        src = LOCAL_INC_RE.sub(lambda m: '/* local include removed: "{}" */'.format(m.group(1)), src)
        rel = p.relative_to(ROOT)
        parts.append(
            "\n/* ===== BEGIN SOURCE: {} ===== */\n#line 1 \"{}\"\n{}\n/* ===== END SOURCE: {} ===== */\n"
            .format(rel, rel, src, rel)
        )
    else:
        parts.append("/* source not found, skipped: {} */\n".format(s))

out = ROOT / "ALL.c"
out.write_text("".join(parts), encoding="utf-8")
print("Wrote {}".format(out))
