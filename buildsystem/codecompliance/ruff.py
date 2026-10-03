# Copyright 2026 the openage authors. See copying.md for legal info.

"""
Checks the Python code with ruff (linting and formatting).

The tools are looked up in PATH. When the checker is run through
'uv run' (see the Makefile), this resolves to the versions pinned in
uv.lock; otherwise the system-installed tools are used.
"""

import shutil
import subprocess
from collections.abc import Callable, Generator, Iterable
from pathlib import Path

from .util import select_files


def find_tool(name: str) -> str | None:
    """
    Return the path of a tool, or None if it is not installed.
    """
    return shutil.which(name)


def _python_files(check_files: Iterable[Path] | None, locations: list[Path]) -> list[Path]:
    """
    Returns the list of python files to check.

    Direct-file locations (e.g. the 'configure' script) are checked
    regardless of their extension.
    """
    return list(select_files(check_files, locations, (".py",)))


def _run_tool(
    tool: str,
    args: list[str],
    filenames: list[Path],
    title: str,
    fix_args: list[str] | None = None,
) -> Generator[tuple[str, str, Callable[[], str] | None]]:
    """
    Invokes a tool on the given files.

    Every diagnostic is reported as its own issue, parsed from the tool's
    'github' output format:
    ::error title=...,file=...,line=...::message

    If fix_args is given, they are used to run the tool's fix mode; the
    yielded issues then carry a fix callback that applies it.
    """
    if not filenames:
        return

    result = subprocess.run(
        [tool, *args, *filenames],
        capture_output=True,
        text=True,
        check=False,
    )

    for line in result.stdout.splitlines():
        if not line.startswith("::error "):
            continue

        fields = dict(field.split("=", 1) for field in line[len("::error ") :].split("::")[0].split(","))
        filename = fields.get("file", "unknown file")
        lineno = fields.get("line", "?")
        message = line.split("::", 2)[-1].replace("%0A", "\n\t")

        fix = None
        if fix_args is not None:
            fix = _create_fix(tool, fix_args, [Path(filename)])

        yield (title, f"{filename}\n\tline: {lineno}\n\t{message}", fix)

    if result.returncode not in (0, 1):
        # the tool itself failed, e.g. on invalid configuration
        yield (
            title,
            f"{tool} failed with exit code {result.returncode}\n\t" + "\n\t".join(result.stderr.splitlines()),
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
    check_files: Iterable[Path] | None, locations: list[Path]
) -> Generator[tuple[str, str, Callable[[], str] | None]]:
    """Invokes the external utilities."""

    ruff = find_tool("ruff")
    if ruff is None:
        yield ("ruff missing", "no ruff found in PATH; run 'uv run <command>' or install ruff", None)
        return

    filenames = _python_files(check_files, locations)

    yield from _run_tool(
        ruff,
        ["check", "--output-format=github"],
        filenames,
        "python lint issue",
        fix_args=["check", "--fix", "--quiet"],
    )
    yield from _run_tool(
        ruff,
        ["format", "--check", "--output-format=github"],
        filenames,
        "python format issue",
        fix_args=["format"],
    )
