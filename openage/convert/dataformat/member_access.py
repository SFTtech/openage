# Copyright 2014-2015 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C

from enum import Enum


# global member type modifiers
class MemberAccess(Enum):
    # pylint doesn't understand that this Enum doesn't need any members.
    # pylint: disable=too-few-public-methods

    READ          = "binary-read_member"
    READ_EXPORT   = "binary-read-export_member"
    NOREAD_EXPORT = "noread-export_member"
    READ_UNKNOWN  = "read-unknown_member"


# TODO those values are made available in the module's global namespace
#      for legacy reasons, to avoid breaking other modules that import
#      them this way.

READ = MemberAccess.READ                        # Reads the data
READ_EXPORT = MemberAccess.READ_EXPORT          # Reads the data and exports it to a file
NOREAD_EXPORT = MemberAccess.NOREAD_EXPORT
READ_UNKNOWN = MemberAccess.READ_UNKNOWN        # For unknown chunks of data
