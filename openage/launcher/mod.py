import toml


class ModInfo:
    """
    Parses and exposes info about a mod
    """
    def __init__(self, file_path=None, url=None):
        if file_path is not None:
            with open(file_path, 'r') as f:
                info = toml.load(f)
        elif url is not None:
            info_str = some_black_magic(url)    # TODO: pseudocode
            info = toml.loads(info_str)
        else:
            raise Exception
 
        self.info = info

        # for convenience
        self.uid = self.info['uid']
        self.version = self.info['version']
        self.author = self.info['author']
        self.confl = self.info['conflicts']
        self.req = self.info['requires']

    def conflicts_with(self, other):
        if self in other.confl:
            return True
        if other in self.confl:
            return True
        return False

    def requires(self, other):
        if other in self.req:
            return True
        return False

    def required_by(self, other):
        return other.requires(self)

    def dump_info(self):
        return self.name, self.author, self.version

    def __eq__(self, other):
        if isinstance(other, type(self)):
            return self.uid == other.uid
        # this way we can use the uid to refer to the object
        if isinstance(other, int):
            return self.uid == other
        return NotImplemented

    def __ne__(self, other):
        if isinstance(other, type(self)):
            return self.uid != other.uid
        if isinstance(other, int):
            return self.uid != other
        return NotImplemented

    def __hash__(self):
        return hash(self.uid)

    def __repr__(self):
        return f'ModInfo<{self.uid}>'
