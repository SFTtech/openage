# Copyright 2026 the openage authors. See copying.md for legal info.

"""
Checks the Python code with ruff (linting and formatting).

The tools are looked up in PATH. When the checker is run through
'uv run' (see the Makefile), this resolves to the versions pinned in
uv.lock; otherwise the system-installed tools are used.
"""

import shutil
import subprocess
from collections.abc import Callable, Generator
from pathlib import Path

from .util import Issue


def find_tool(name: str) -> str | None:
    """
    Return the path of a tool, or None if it is not installed.
    """
    return shutil.which(name)


def _run_tool(
    tool: str,
    args: list[str],
    paths: list[Path],
    title: str,
    fix_args: list[str] | None = None,
) -> Generator[Issue]:
    """
    Invokes a tool on the given locations (files or directories).

    Every diagnostic is reported as its own issue, parsed from the tool's
    'github' output format:
    ::error title=...,file=...,line=...::message

    If fix_args is given, they are used to run the tool's fix mode; the
    yielded issues then carry a fix callback that applies it.
    """
    if not paths:
        return

    result = subprocess.run(
        [tool, *args, *paths],
        capture_output=True,
        text=True,
        check=False,
    )

    for line in result.stdout.splitlines():
        if not line.startswith("::error "):
            continue

        fields = dict(field.split("=", 1) for field in line[len("::error ") :].split("::")[0].split(","))
        # the other checkers report paths relative to the project root
        filename = Path(fields["file"]).absolute().relative_to(Path.cwd()) if "file" in fields else None
        lineno = fields.get("line", "?")
        message = line.split("::", 2)[-1].replace("%0A", "\n")

        fix = None
        if fix_args is not None and filename is not None:
            fix = _create_fix(tool, fix_args, [filename])

        yield (title, filename, f"line: {lineno}\n{message}", fix)

    if result.returncode not in (0, 1):
        # the tool itself failed, e.g. on invalid configuration
        yield (
            title,
            None,
            f"{tool} failed with exit code {result.returncode}\n{result.stderr.rstrip()}",
            None,
        )


def _create_fix(tool: str, fix_args: list[str], filenames: list[Path]) -> Callable[[], str]:
    """
    Create a function that, when called, runs the tool's fix mode on the
    files and returns a report.
    """

    def fix() -> str:
        result = subprocess.run(
            [tool, *fix_args, *filenames],
            capture_output=True,
            text=True,
            check=False,
        )

        if result.returncode != 0:
            return f"fixing {filenames} failed:\n\t" + "\n\t".join(result.stderr.splitlines())

        return f"fixed {len(filenames)} file(s) with {tool}"

    return fix


def find_issues(
    locations: list[Path],
) -> Generator[Issue]:
    """Invokes the external utilities."""

    ruff = find_tool("ruff")
    if ruff is None:
        yield ("ruff missing", None, "no ruff found in PATH; run 'uv run <command>' or install ruff", None)
        return

    yield from _run_tool(
        ruff,
        ["check", "--output-format=github"],
        locations,
        "python lint issue",
        fix_args=["check", "--fix", "--quiet"],
    )
    yield from _run_tool(
        ruff,
        ["format", "--check", "--output-format=github"],
        locations,
        "python format issue",
        fix_args=["format"],
    )
