# Copyright 2024-2024 the openage authors. See copying.md for legal info.

"""
Handling of version information for openage.
"""
from __future__ import annotations

import re

SEMVER_REGEX = re.compile(
    (r"^(?P<major>0|[1-9]\d*)\.(?P<minor>0|[1-9]\d*)\.(?P<patch>0|[1-9]\d*)"
     r"(?:-(?P<prerelease>(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)"
     r"(?:\.(?:0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?"
     r"(?:\+(?P<buildmetadata>[0-9a-zA-Z-]+(?:\.[0-9a-zA-Z-]+)*))?$"))


class SemanticVersion:
    """
    Semantic versioning information.
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

    def __lt__(self, other: SemanticVersion) -> bool:
        if self.major < other.major:
            return True
        if self.minor < other.minor:
            return True
        if self.patch < other.patch:
            return True

        return False

    def __le__(self, other: SemanticVersion) -> bool:
        if self.major <= other.major:
            return True

        if self.minor <= other.minor:
            return True

        if self.patch <= other.patch:
            return True

        return False

    def __eq__(self, other: SemanticVersion) -> bool:
        return (self.major == other.major and
                self.minor == other.minor and
                self.patch == other.patch)

    def __ne__(self, other: SemanticVersion) -> bool:
        return not self.__eq__(other)

    def __gt__(self, other: SemanticVersion) -> bool:
        return not self.__le__(other)

    def __ge__(self, other: SemanticVersion) -> bool:
        return not self.__lt__(other)

    def __str__(self) -> str:
        version = f"{self.major}.{self.minor}.{self.patch}"
        if self.prerelease:
            version += f"-{self.prerelease}"
        if self.buildmetadata:
            version += f"+{self.buildmetadata}"

        return version

    def __repr__(self) -> str:
        return f"SemanticVersion('{str(self)}')"
