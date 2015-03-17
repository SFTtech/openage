# Copyright 2015-2015 the openage authors. See copying.md for legal info.

import configparser
import re


class PackConfig:
    """
    main config file for a mod pack.

    stores the package metainformation for the mod list.

    file contents:

    [openage-pack]
    name = <package name>
    version = <package version>
    author = <pack author>
    pack_type = (game|mod|...)
    config_type = (cfg|nyan|...)
    config = <filename.cfg>
    """

    # everything has to be in this section:
    root_cfg = "openage-pack"

    # required attributes
    pack_attrs = ("name", "version", "author",
                  "pack_type", "config_type", "config")

    def __init__(self):
        """
        init all required attributes to None.
        """

        self.cfg = dict()

        for attr in self.pack_attrs:
            self.cfg[attr] = None

    def check_sanity(self):
        """
        checks whether the current config is valid.
        """

        if None in self.cfg.values():
            nones = [v for k, v in self.cfg.items() if v is None]
            raise Exception("unset attribute(s): %s" % nones)

        if self.cfg["pack_type"] not in ("game", "mod"):
            raise Exception("unsupported data pack type")

        if self.cfg["config_type"] not in ("cfg",):
            raise Exception("config_type not supported: %s" %
                            self.cfg["config_type"])

        if not re.match(r"[a-zA-Z][a-zA-Z0-9_]*", self.cfg["name"]):
            raise Exception("disallowed chars in pack name found")

        if not re.match(r"v[0-9]+(\.[0-9]+)*(-r[0-9]+)", self.cfg["version"]):
            raise Exception("invalid pack version")

    def read(self, handle):
        """
        fill this pack config by the contents provided by the file handle.
        """

        cfp = configparser.ConfigParser()

        # read the config from the (pseudo-?)handle
        cfp.read_file(handle, source=handle.name)

        if self.root_cfg not in cfp:
            raise Exception("pack root config doesn't contain '%s' section."
                            % self.root_cfg)
        else:
            self.cfg = cfp[self.root_cfg]

        self.check_sanity()

    def write(self, handle):
        """
        store the settings of this configfile to the given file handle.
        """

        self.check_sanity()

        cfp = configparser.ConfigParser()
        cfp[self.root_cfg] = self.cfg

        cfp.write(handle)

    def set_attrs(self, **dct):
        """
        set all attributes from the dict to this pack config.
        """

        # just take the known attributes
        to_set = {k: v for k, v in dct.items() if k in self.pack_attrs}

        # check if all keys were used
        unknown_keys = dct.keys() - to_set.keys()
        if unknown_keys:
            raise Exception("unknown attr name(s) to set: %s" % unknown_keys)

        self.cfg.update(to_set)

    def get_attrs(self):
        """
        return this config's settings.
        """

        return self.cfg
