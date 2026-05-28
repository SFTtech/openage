# Copyright 2024-2026 the openage authors. See copying.md for legal info.

"""
Handling of version information for openage.
"""
from __future__ import annotations

import re
from functools import total_ordering

SEMVER_REGEX = re.compile(
    (r"^(?P<major>0|[1-9]\d*)\.(?P<minor>0|[1-9]\d*)\.(?P<patch>0|[1-9]\d*)"
     r"(?:-(?P<prerelease>(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)"
     r"(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?"
     r"(?:\+(?P<buildmetadata>[0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$"))


@total_ordering
class SemanticVersion:
    """
    Semantic versioning information.

    Precedence is determined by major, then minor, then patch, as per
    semver 2.0.0. Build metadata is ignored for ordering. Prerelease
    ordering follows the semver rule that any prerelease is lower than
    the same MAJOR.MINOR.PATCH without one; full prerelease identifier
    comparison (alpha < beta < rc, etc.) is not implemented here yet.

    >>> SemanticVersion("1.0.0") > SemanticVersion("0.5.0")
    True
    >>> SemanticVersion("2.0.0") > SemanticVersion("1.99.99")
    True
    >>> SemanticVersion("1.2.3") < SemanticVersion("1.2.4")
    True
    >>> SemanticVersion("1.0.5") < SemanticVersion("1.1.0")
    True
    >>> SemanticVersion("1.0.0") == SemanticVersion("1.0.0")
    True
    >>> SemanticVersion("1.0.0-alpha") < SemanticVersion("1.0.0")
    True
    >>> str(SemanticVersion("1.2.3-rc.1+build.7"))
    '1.2.3-rc.1+build.7'
    """

    def __init__(self, version: str) -> None:
        """
        Create a new semantic version object from a version string.

        :param version: The version string to parse.
        """
        match = SEMVER_REGEX.match(version)
        if not match:
            raise ValueError(f"Invalid semantic version: {version}")

        self.major = int(match.group("major"))
        self.minor = int(match.group("minor"))
        self.patch = int(match.group("patch"))
        self.prerelease = match.group("prerelease")
        self.buildmetadata = match.group("buildmetadata")

    def _precedence_key(self) -> tuple:
        # A version with no prerelease ranks higher than one with a
        # prerelease at the same MAJOR.MINOR.PATCH (semver 11.3). We
        # encode that by giving "no prerelease" a 1 and any prerelease
        # a 0 as the next tuple element. The prerelease string itself
        # is included so two prereleases compare stably, but note this
        # is a lexicographic fallback, not the full semver 11.4
        # identifier comparison.
        return (
            self.major,
            self.minor,
            self.patch,
            1 if self.prerelease is None else 0,
            self.prerelease or "",
        )

    def __lt__(self, other: SemanticVersion) -> bool:
        return self._precedence_key() < other._precedence_key()

    def __eq__(self, other: object) -> bool:
        if not isinstance(other, SemanticVersion):
            return NotImplemented
        return self._precedence_key() == other._precedence_key()

    def __hash__(self) -> int:
        return hash(self._precedence_key())

    def __str__(self) -> str:
        version = f"{self.major}.{self.minor}.{self.patch}"
        if self.prerelease:
            version += f"-{self.prerelease}"
        if self.buildmetadata:
            version += f"+{self.buildmetadata}"

        return version

    def __repr__(self) -> str:
        return f"SemanticVersion('{str(self)}')"
