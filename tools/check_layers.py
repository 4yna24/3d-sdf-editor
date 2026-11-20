#!/usr/bin/env python3
"""Check the layer rules of ai/code-structure.md.

1. A module includes only itself and the modules in its USES list
   (the list in its src/<module>/CMakeLists.txt).
2. Outside libraries are included only where they are allowed (SDL in platform, ...).

Exit code 1 and one line per problem when a rule is broken.
"""

import re
import sys
from pathlib import Path

SRC = Path(__file__).resolve().parent.parent / "src"

# Include prefix -> modules that may include it.
OUTSIDE = {
    "SDL3/": {"platform"},
    "glad/": {"gpu"},
    "imgui": {"ui", "platform"},  # platform holds ImGui's SDL backend
}

INCLUDE = re.compile(r'^\s*#\s*include\s*[<"]([^>"]+)[>"]', re.M)
ADD_MODULE = re.compile(r"sdfe_add_module\(\s*(\w+)(.*?)\)", re.S)
USES = re.compile(r"\bUSES\b(.*?)(?=\b(?:SOURCES|LIBS|PUBLIC_LIBS)\b|$)", re.S)


def read_uses():
    uses = {}
    for cmake in SRC.glob("*/CMakeLists.txt"):
        m = ADD_MODULE.search(cmake.read_text())
        if not m:
            continue
        u = USES.search(m.group(2))
        uses[m.group(1)] = set(u.group(1).split()) if u else set()
    return uses


def main():
    uses = read_uses()
    modules = {p.name for p in SRC.iterdir() if p.is_dir()}
    errors = []

    for module in sorted(modules):
        everything = module == "app"  # app may include every module
        allowed = uses.get(module, set()) | {module}
        for path in sorted((SRC / module).rglob("*")):
            if path.suffix not in {".cc", ".hh", ".glsl"}:
                continue
            for inc in INCLUDE.findall(path.read_text()):
                where = f"{path.relative_to(SRC.parent)}: #include {inc}"
                first = inc.split("/")[0]
                if first in modules and not everything and first not in allowed:
                    errors.append(f"{where}  ('{module}' may not use '{first}')")
                for prefix, users in OUTSIDE.items():
                    if inc.startswith(prefix) and module not in users:
                        errors.append(f"{where}  (only {sorted(users)} may use it)")

    for e in errors:
        print(e)
    if errors:
        print(f"check_layers: {len(errors)} problem(s)")
        return 1
    print(f"check_layers: {len(modules)} modules OK")
    return 0


if __name__ == "__main__":
    sys.exit(main())
