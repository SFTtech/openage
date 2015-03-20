# Copyright 2014-2015 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C,R,E

from enum import Enum


class Token:
    class Type(Enum):
        CIRCUM      = "^"
        COLON       = ":"
        COMMA       = ","
        LPAREN      = "("
        RPAREN      = ")"
        LBRACK      = "["
        RBRACK      = "]"
        LBRACE      = "{"
        RBRACE      = "}"
        ELLIPSIS    = "..."
        ASSIGN      = "="
        IDENTIFIER  = "[_a-zA-Z][_a-zA-Z0-9]*"
        INTEGER     = "integer literal, e.g. 12323"
        FLOAT       = "float literal, e.g. 123.342"
        STRING      = "string literal ('foo')"
        FAIL        = "failed to parse token, unexpected character or eof"
        END         = "end of the token stream"

    CHARACTER_TYPE_LOOKUP = {member.value: member for member in Type}

    def __init__(self, ttype, content, line, offset):
        self.ttype = ttype
        self.content = content
        self.line = line
        self.offset = offset

    def __str__(self):
        ttype_name = Token.Type.reverse_lookup[self.ttype]
        return "%d:%d:%s:%s" % (self.line, self.offset, ttype_name, self.content)

    @classmethod
    def get_type_for_character(cls, c):
        return cls.CHARACTER_TYPE_LOOKUP[c]
