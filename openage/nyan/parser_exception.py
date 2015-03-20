# Copyright 2014-2015 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C


class ParserException(Exception):
    def __init__(self, message, token):
        super().__init__(message)
        self.token = token
