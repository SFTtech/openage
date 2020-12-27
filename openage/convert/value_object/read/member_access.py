# Copyright 2014-2020 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C

from enum import Enum


# global member type modifiers
class MemberAccess(Enum):
    # pylint doesn't understand that this Enum doesn't need any members.
    # pylint: disable=too-few-public-methods

    READ          = "READ"
    READ_GEN      = "READ_GEN"
    NOREAD_EXPORT = "NOREAD_EXPORT"
    READ_UNKNOWN  = "READ_UNKNOWN"
    SKIP          = "SKIP"


# TODO those values are made available in the module's global namespace
#      for legacy reasons, to avoid breaking other modules that import
#      them this way.

READ = MemberAccess.READ                        # Reads the data
READ_GEN = MemberAccess.READ_GEN
NOREAD_EXPORT = MemberAccess.NOREAD_EXPORT
READ_UNKNOWN = MemberAccess.READ_UNKNOWN        # For unknown chunks of data
SKIP = MemberAccess.SKIP
