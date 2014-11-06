# Copyright 2014-2014 the openage authors. See copying.md for legal info.

from .nyan_spec_ast import *
from .parser_exception import ParserException
from .token import Token

class NyanSpecParser:
    def __init__(self, tokens):
        self.index = 0
        self.tokens = tokens
        self.token = self.tokens[self.index]
        self.errors = []

    def parse(self):
        self.ast = NyanSpecAST()
        self.parse_spec()
        return self.ast, self.errors

    def parse_spec(self):
        while self.token.ttype != Token.Type.END:
            if self.is_token(Token.Type.IDENTIFIER):
                self.parse_type_decl()
            else:
                self.expected("type declaration")

    def parse_type_decl(self):
        type_name = self.skip_token()
        self.current_type = NyanSpecType(type_name)
        if type_name.content in self.ast.types:
            self.error("Duplicated definition of type '%s'" %
                    type_name.content, type_name)
        else:
            self.ast.types[type_name.content] = self.current_type

        self.expect_token(Token.Type.LBRACE, "'{'")
        self.parse_type_body()
        self.expect_token(Token.Type.RBRACE, "','", "'}'")

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
            
            # parse attribute name
            attr_name = self.skip_token()
            new_attr = NyanSpecAttribute(attr_name)
            if attr_name.content in self.current_type.attributes:
                self.error(
                        "Duplicated definition of attribute '%s' in type '%s'"
                        % (attr_name.content, self.current_type.name.content),
                        attr_name)
            else:
                self.current_type.attributes[attr_name.content] = new_attr

            # parse colon
            self.expect_token(Token.Type.COLON, "':'") 

            # parse attribute type
            attr_type = self.expect_token(Token.Type.IDENTIFIER,
                    "attribute type")
            new_attr.atype = attr_type

            if attr_type.content == "set":
                # parse set type
                self.expect_token(Token.Type.LPAREN, "'('")
                new_attr.atype = self.expect_token(Token.Type.IDENTIFIER, "set type")
                self.expect_token(Token.Type.RPAREN, "')'")
                new_attr.is_set = True

            if self.accept_token(Token.Type.ASSIGN):
                # parse default value
                default_value = self.parse_default_value()
                new_attr.default_value = default_value

            got_comma = self.accept_token(Token.Type.COMMA)
        return got_comma

    def parse_default_value(self):
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
            new_delta = NyanSpecDelta(delta_type)
            if delta_type.content in self.current_type.deltas:
                self.error("Duplicated definition of delta '%s' in "
                        "type '%s'" % (delta_type.content,
                            self.current_type.content), delta_type)
            else:
                self.current_type.deltas[delta_type.content] = new_delta
            
            got_comma = self.accept_token(Token.Type.COMMA)

    def build_expectations_string(self, *expectations):
        length = len(expectations)
        strings = list(map(str, expectations))
        result = ""
        if length >= 2:
            result += ", ".join(strings[0:length-1]) + " or "
        result += strings[-1]
        return result

    def expected(self, *expectations):
        """
        Adds a ParserException with the given expectation strings to the error
        list.
        """
        self.error("Expected %s, got '%s'" %
            (self.build_expectations_string(*expectations),
            self.token.content), self.token, True)

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

    def expect_token(self, ttype, *expectations):
        """
        Checks if the current token has the given type. If yes, the token is
        returned and skipped. Otherwise a ParserException with the given
        expectation strings is raised.
        """
        if not self.is_token(ttype):
            self.expected(*expectations)
        return self.skip_token()
    
    def skip_token(self):
        """
        Returns and skips the current token.
        """
        token = self.token
        self.next_token()
        return token

    def error(self, message, token, critical=False):
        exception = ParserException(message, token)
        if critical:
            raise exception
        else:
            self.errors.append(exception)
