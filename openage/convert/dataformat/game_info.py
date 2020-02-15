# Copyright 2020-2020 the openage authors. See copying.md for legal info.


class GameFileVersion:
    """
    Can be used to associate a file hash with a specific version number.
    This can be used to pinpoint the exact version of a game. 
    """

    def __init__(self, filepath, hashes):
        """
        Create a new file hash to version association.
        """

        self.path = filepath
        self.hashes = hashes

    def get_path(self):
        """
        Return the path of the file.
        """
        return self.path

    def get_hashes(self):
        """
        Return the hash-version association for the file.
        """
        return self.hashes
