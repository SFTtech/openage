# Copyright 2014-2014 the openage authors. See copying.md for legal info.


class Enum:
    def __init__(self, name, values):
        enums = dict(zip(values, range(len(values))))
        self.reverse_lookup = dict((v, k) for k, v in enums.items())
        self.__dict__.update(enums)
