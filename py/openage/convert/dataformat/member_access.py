# Copyright 2014-2014 the openage authors. See copying.md for legal info.

# global member type modifiers
from openage.util import NamedObject

READ          = NamedObject("binary-read_member")
READ_EXPORT   = NamedObject("binary-read-export_member")
NOREAD_EXPORT = NamedObject("noread-export_member")
READ_UNKNOWN  = NamedObject("read-unknown_member")
