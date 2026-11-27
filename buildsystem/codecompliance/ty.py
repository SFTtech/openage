# Copyright 2026 the openage authors. See copying.md for legal info.

"""
Checks the Python code with ty, the type checker.

The tool is looked up in PATH. When the checker is run through
'uv run' (see the Makefile), this resolves to the version pinned in
uv.lock; otherwise the system-installed tool is used.
"""

from collections.abc import Generator
from pathlib import Path

from .ruff import _run_tool, find_tool
from .util import Issue


def find_issues(
    dirnames: list[Path],
) -> Generator[Issue]:
    """Invokes the external utility."""

    ty = find_tool("ty")
    if ty is None:
        yield ("ty missing", None, "no ty found in PATH; run 'uv run <command>' or install ty", None)
        return

    yield from _run_tool(
        ty,
        ["check", "--output-format=github"],
        dirnames,
        "python type issue",
        fix_args=["check", "--fix"],
    )
