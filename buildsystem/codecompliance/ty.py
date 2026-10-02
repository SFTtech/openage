# Copyright 2026 the openage authors. See copying.md for legal info.

"""
Checks the Python code with ty, the type checker.

The tool is looked up in PATH. When the checker is run through
'uv run' (see the Makefile), this resolves to the version pinned in
uv.lock; otherwise the system-installed tool is used.
"""

from collections.abc import Callable, Generator, Iterable

from .ruff import _python_files, _run_tool, find_tool


def find_issues(
    check_files: Iterable[str] | None, dirnames: tuple[str, ...]
) -> Generator[tuple[str, str, Callable[[], str] | None]]:
    """Invokes the external utility."""

    ty = find_tool("ty")
    if ty is None:
        yield ("ty missing", "no ty found in PATH; run 'uv run <command>' or install ty", None)
        return

    yield from _run_tool(
        ty,
        ["check", "--output-format=github"],
        _python_files(check_files, dirnames),
        "python type issue",
        fix_args=["check", "--fix"],
    )
