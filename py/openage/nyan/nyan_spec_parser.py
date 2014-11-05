# Copyright 2014-2014 the openage authors. See copying.md for legal info.

from .nyan_spec_ast import *
from .parser_exception import ParserException
from .token import Token

class NyanSpecParser:
    def __init__(self, tokens):
        self.index = 0
        self.tokens = tokens
        self.token = self.tokens[self.index]

    def parse(self):
        self.ast = NyanSpecAST()
        self.parse_spec()
        return self.ast

    def parse_spec(self):
        while self.token.ttype != Token.Type.END:
            if self.is_token(Token.Type.IDENTIFIER):
                self.parse_type_decl()
            else:
                self.expected("type declaration")

    def parse_type_decl(self):
        type_name = self.skip_token()
        if type_name.content in self.ast.types:
            raise ParserException("Duplicated definition of type '%s'" %
                    type_name.content, type_name)
        else:
            self.current_type = NyanSpecType(type_name)
            self.ast.types[type_name.content] = self.current_type

        self.expect_token(Token.Type.LBRACE, "'{'")
        self.parse_type_body()
        self.expect_token(Token.Type.RBRACE, "'}'")

    def parse_type_body(self):
        got_comma = self.parse_type_attributes();
        if not got_comma:
            return

        dynamic_attributes = self.accept_token(Token.Type.ELLIPSIS)
        self.current_type.dynamic_attributes = dynamic_attributes

        if dynamic_attributes:
            if self.accept_token(Token.Type.COMMA):
                self.parse_type_deltas()
        else:
            self.parse_type_deltas()

    def parse_type_attributes(self):
        got_comma = True
        while got_comma:
            if not self.is_token(Token.Type.IDENTIFIER):
                return True
            
            attr_name = self.skip_token()
            if attr_name.content in self.current_type.attributes:
                raise ParserException("Duplicated definition of attribute '%s' "
                        "in type '%s'" % (attr_name.content,
                            self.current_type.name.content), attr_name)
            new_attr = NyanSpecAttribute(attr_name)
            self.current_type.attributes[attr_name.content] = new_attr

            self.expect_token(Token.Type.COLON, "':'") 
            if not self.is_token(Token.Type.IDENTIFIER):
                self.expected("type")
            attr_type = self.skip_token()
            new_attr.atype = attr_type

            if attr_type.content == "set":
                self.expect_token(Token.Type.LPAREN, "'('")
                set_attr_type = self.expect_token(Token.Type.IDENTIFIER, "set type")
                self.expect_token(Token.Type.RPAREN, "')'")
                new_attr.is_set = True
            else:
                if self.accept_token(Token.Type.ASSIGN):
                    default_value = self.parse_default_value()
                    new_attr.default_value = default_value

            got_comma = self.accept_token(Token.Type.COMMA)
        return got_comma

    def parse_default_value(self):
        default_value_type = None
        if self.is_token(Token.Type.INTEGER) or\
                self.is_token(Token.Type.FLOAT) or\
                self.is_token(Token.Type.STRING) or\
                self.is_token(Token.Type.IDENTIFIER, "true") or\
                self.is_token(Token.Type.IDENTIFIER, "false"):
            default_value = self.skip_token()
            return default_value
        else:
            self.expected("default value")

    def parse_type_deltas(self):
        got_comma = True
        while got_comma:
            self.expect_token(Token.Type.CIRCUM, "'^'")
            delta_type = self.expect_token(Token.Type.IDENTIFIER, "delta type")
            if delta_type.content in self.current_type.deltas:
                raise ParserException("Duplicated definition of delta '%s' in "
                        "type '%s'" % (delta_type.content,
                            self.current_type.content), delta_type)

            new_delta = NyanSpecDelta(delta_type)
            self.current_type.deltas[delta_type.content] = new_delta
            
            got_comma = self.accept_token(Token.Type.COMMA)

    def expected(self, expectation):
        """
        Raises a ParserException with the given expectation string.
        """
        raise ParserException("Expected %s, got '%s'" % (expectation,
            self.token.content), self.token)

    def next_token(self):
        """
        Jumps to the next token, until a END token is reached.
        """
        if self.token.ttype != Token.Type.END:
            self.index += 1
            self.token = self.tokens[self.index]

    def accept_token(self, ttype, content=None):
        """
        Returns whether the current token has the given type and content. If
        yes, the token is skipped.
        """
        if self.is_token(ttype, content):
            self.next_token()
            return True
        return False
    
    def is_token(self, ttype, content=None):
        """
        Returns whether the current token has the given type and content.
        """
        result = self.token.ttype == ttype
        if content is not None:
            result = result and self.token.content == content
        return result

    def expect_token(self, ttype, expectation):
        """
        Checks if the current token has the given type. If yes, the token is
        returned and skipped. Otherwise a ParserException with the given
        expectation string is raised.
        """
        if not self.is_token(ttype):
            self.expected(expectation)
        token = self.token
        self.next_token()
        return token
    
    def skip_token(self):
        """
        Returns and skips the current token.
        """
        token = self.token
        self.next_token()
        return token
