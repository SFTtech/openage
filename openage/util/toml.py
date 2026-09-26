# Copyright 2025-2026 the openage authors. See copying.md for legal info.

"""
TOML reading and writing, wrapped around the Python standard library.

tomllib (3.11+) can only parse; tomli_w is used to write files. Import
`load`/`loads`/`dumps`/`TomlDecodeError` from here instead of using
tomllib/tomli_w directly.
"""
from __future__ import annotations

import tomllib
from tomllib import TOMLDecodeError as TomlDecodeError  # noqa: A005

import tomli_w


def load(fileobj) -> dict:
    """
    Parse TOML from a file object opened in binary mode.
    """
    return tomllib.load(fileobj)


def loads(string: str) -> dict:
    """
    Parse a TOML string.
    """
    return tomllib.loads(string)


def dumps(data: dict) -> str:
    """
    Serialize a dict to a TOML string.
    """
    return tomli_w.dumps(data)
