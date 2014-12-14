# Copyright 2014-2014 the openage authors. See copying.md for legal info.

from .util import Enum


class Token:
    Type = Enum(
        name="Type",
        values=[
            "CIRCUM",      # ^
            "COLON",       # :
            "COMMA",       # ,
            "LPAREN",      # (
            "RPAREN",      # )
            "LBRACK",      # [
            "RBRACK",      # ]
            "LBRACE",      # {
            "RBRACE",      # }
            "ELLIPSIS",    # ...
            "ASSIGN",      # =
            "IDENTIFIER",  # [_a-zA-Z][_a-zA-Z0-9]*
            "INTEGER",     # integer literal, e.g. 12323
            "FLOAT",       # float literal, e.g. 123.342
            "STRING",      # 'foobar'
            "FAIL",        # failed to parse token, unexpected character or eof
            "END"          # end of the token stream
        ]
    )

    def __init__(self, ttype, content, line, offset):
        self.ttype = ttype
        self.content = content
        self.line = line
        self.offset = offset

    def __str__(self):
        ttype_name = Token.Type.reverse_lookup[self.ttype]
        return "%d:%d:%s:%s" % (self.line, self.offset, ttype_name, self.content)

    CHARACTER_TYPE_LOOKUP = {
        '^': Type.CIRCUM,
        ':': Type.COLON,
        ',': Type.COMMA,
        '(': Type.LPAREN,
        ')': Type.RPAREN,
        '[': Type.LBRACK,
        ']': Type.RBRACK,
        '{': Type.LBRACE,
        '}': Type.RBRACE,
        '=': Type.ASSIGN,
    }

    @classmethod
    def get_type_for_character(cls, c):
        return cls.CHARACTER_TYPE_LOOKUP[c]
