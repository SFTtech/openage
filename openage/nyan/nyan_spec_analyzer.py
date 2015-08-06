# Copyright 2014-2015 the openage authors. See copying.md for legal info.

# pylint: disable=R,C

from .parser_exception import ParserException
from .token import Token


class NyanSpecAnalyzer:
    def __init__(self, ast):
        self.ast = ast
        self.errors = []

    def analyze(self):
        for ntype in self.ast.types.values():
            self.check_type(ntype)
            self.check_default_value(ntype)
        return self.errors

    def check_type(self, ntype):
        self.check_attribute_types_exist(ntype)
        self.check_delta_types_exist(ntype)

    def check_attribute_types_exist(self, ntype):
        for attr in ntype.attributes.values():
            type_name = attr.atype.content
            if not self.is_basic_type(type_name):
                if type_name not in self.ast.types:
                    self.error("Type '%s' is undefined" % type_name, attr.atype)
            else:
                if attr.is_set:
                    self.error("Sets of type '%s' are not allowed" % type_name,
                               attr.atype)

    def check_delta_types_exist(self, ntype):
        for delta in ntype.deltas.values():
            type_name = delta.delta_type.content
            if self.is_basic_type(type_name):
                self.error("Delta of primitive type '%s' is not allowed" %
                           type_name, delta.delta_type)
            elif type_name not in self.ast.types:
                self.error("Type '%s' is not defined" % type_name,
                           delta.delta_type)

    def check_default_value(self, ntype):
        for attr in ntype.attributes.values():
            if self.is_basic_type(attr.atype.content) and not attr.is_set and\
                    attr.default_value is not None:
                # if the attribute is a basic type and no set, we have to check
                # it's default value
                self.match_types(attr.atype, attr.default_value)
            elif attr.default_value is not None:
                # otherwise there must not be a default value
                if attr.is_set:
                    error_message = "Type 'set(%s)' must not have default value"
                else:
                    error_message = "Type '%s' must not have default value"
                self.error(error_message % attr.atype.content,
                           attr.default_value)

    def is_basic_type(self, type_name):
        return type_name in ["bool", "int", "float", "string"]

    def match_types(self, attr_type, attr_value):
        if attr_type.content == "bool":
            if attr_value.ttype == Token.Type.IDENTIFIER and\
                    attr_value.content in ["true", "false"]:
                return
        elif attr_type.content == "int":
            if attr_value.ttype == Token.Type.INTEGER:
                return
        elif attr_type.content == "float":
            if attr_value.ttype in [Token.Type.INTEGER, Token.Type.FLOAT]:
                return
        elif attr_type.content == "string":
            if attr_value.ttype == Token.Type.STRING:
                return
        self.error("'%s' is no valid literal for primitive type '%s'"
                   % (attr_value.content, attr_type.content), attr_value)

    def error(self, message, token):
        self.errors.append(ParserException(message, token))
