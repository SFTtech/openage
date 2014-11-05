# Copyright 2014-2014 the openage authors. See copying.md for legal info.

class ParserException(Exception):
    def __init__(self, message, token):
        super(ParserException, self).__init__(message)
        self.token = token
