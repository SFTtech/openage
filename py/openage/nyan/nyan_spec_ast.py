# Copyright 2014-2014 the openage authors. See copying.md for legal info.


class NyanSpecAST:
    def __init__(self):
        self.types = {}

    def __str__(self):
        result = ""
        for ntype in self.types.values():
            result += str(ntype) + "\n"
        return result


class NyanSpecType:

    def __init__(self, name):
        self.name = name
        self.attributes = {}
        self.dynamic_attributes = False
        self.deltas = {}

    def __str__(self):
        result = self.name.content + " {\n"
        for attr in self.attributes.values():
            result += "\t" + str(attr) + "\n"
        if self.dynamic_attributes:
            result += "\t...\n"
        for delta in self.deltas.values():
            result += "\t" + str(delta) + "\n"
        result += "}\n"
        return result


class NyanSpecAttribute:
    def __init__(self, name):
        self.name = name
        self.is_set = False
        self.atype = None
        self.default_value = None

    def __str__(self):
        result = self.name.content + ": "
        if self.is_set:
            result += "set(" + self.atype.content + ")"
        else:
            result += self.atype.content
        if self.default_value:
            result += " = " + self.default_value.content
        return result


class NyanSpecDelta:
    def __init__(self, delta_type):
        self.delta_type = delta_type

    def __str__(self):
        return "^" + self.delta_type.content
