# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Modpack definition file.
"""
import toml
from ..data_definition import DataDefinition
import base64

FILE_VERSION = "0.1.0"


class ModpackInfo(DataDefinition):

    def __init__(self, targetdir, filename, modpack_name):
        super().__init__(targetdir, filename)

        # Mandatory
        self.name = modpack_name
        self.version = None

        # Optional
        self.uid = None
        self.short_description = None
        self.long_description = None
        self.provides = []
        self.conflicts = []
        self.requires = []
        self.url = None
        self.license = None
        self.author_groups = {}
        self.authors = {}
        self.load_files = []

    def add_assets_to_load(self, path):
        """
        Add a path to an asset that is loaded by the modpack. Directories
        are also allowed.

        :param path: Path to the asset(s).
        :type path: str
        """
        self.load_files.append(path)

    def add_author(self, author, contact_info={}):
        """
        Adds an author with optional contact info.

        :param author: Human-readable author identifier.
        :type author: str
        :param contact_info: Dictionary with contact info.
                             (key = contact method, value = address)
                             example: {"e-mail": "mastermind@openage.dev"}
        :type contact_info: dict
        """
        self.authors[author] = contact_info

    def add_author_group(self, author_group, authors=[]):
        """
        Adds an author with optional contact info.

        :param author: Human-readable author identifier.
        :type author: str
        :param contact_info: Dictionary with contact info.
                             (key = contact method, value = address)
                             example: {"e-mail": "mastermind@openage.dev"}
        :type contact_info: dict
        """
        self.author_groups[author_group] = authors

    def add_provided_modpack(self, modpack_name, version, uid):
        """
        Add an identifier of another modpack that this modpack provides.

        :param modpack_name: Name of the provided modpack.
        :type modpack_name: str
        :param version: Version of the provided modpack.
        :type version: str
        :param uid: UID of the provided modpack.
        :type uid: str
        """
        self.provides[modpack_name].update({"uid": uid, "version": version})

    def add_conflicting_modpack(self, modpack_name, version, uid):
        """
        Add an identifier of another modpack that has a conflict with this modpack.

        :param modpack_name: Name of the provided modpack.
        :type modpack_name: str
        :param version: Version of the provided modpack.
        :type version: str
        :param uid: UID of the provided modpack.
        :type uid: str
        """
        self.conflicts[modpack_name].update({"uid": uid, "version": version})

    def add_required_modpack(self, modpack_name, version, uid):
        """
        Add an identifier of another modpack that has is required by this modpack.

        :param modpack_name: Name of the provided modpack.
        :type modpack_name: str
        :param version: Version of the provided modpack.
        :type version: str
        :param uid: UID of the provided modpack.
        :type uid: str
        """
        self.requires[modpack_name].update({"uid": uid, "version": version})

    def dump(self):
        """
        Outputs the modpack info to the TOML output format.
        """
        output_dict = {}

        # info table
        info_table = {"info": {}}
        info_table["info"].update({"name": self.name})
        if self.uid:
            # Encode as base64 string
            uid = base64.b64encode(self.uid.to_bytes(6, byteorder='big')).decode("utf-8")
            info_table["info"].update({"uid": uid})

        if not self.version:
            raise Exception("%s: version needs to be defined before dumping." % (self))

        info_table["info"].update({"version": self.version})

        if self.short_description:
            info_table["info"].update({"short_description": self.short_description})
        if self.long_description:
            info_table["info"].update({"long_description": self.long_description})

        if self.url:
            info_table["info"].update({"url": self.url})
        if self.license:
            info_table["info"].update({"license": self.license})

        output_dict.update(info_table)

        # provides table
        provides_table = {"provides": {}}
        provides_table["provides"].update(self.provides)

        output_dict.update(provides_table)

        # conflicts table
        conflicts_table = {"conflicts": {}}
        conflicts_table["conflicts"].update(self.conflicts)

        output_dict.update(conflicts_table)

        # requires table
        requires_table = {"requires": {}}
        requires_table["requires"].update(self.requires)

        output_dict.update(requires_table)

        # load table
        load_table = {"load": {}}
        load_table["load"].update({"include": self.load_files})

        output_dict.update(load_table)

        # authors table
        authors_table = {"authors": {}}
        authors_table["authors"].update(self.authors)

        output_dict.update(authors_table)

        output_str = "# MODPACK INFO version %s\n\n" % (FILE_VERSION)
        output_str += toml.dumps(output_dict)

        return output_str

    def set_author_info(self, author, contact_method, contact_address):
        """
        Add or change a contact method of an author.

        :param author: Author identifier.
        :type author: str
        :param contact_method: Contact method for an author.
        :type contact_method: str
        :param contact_address: Contact address for a contact method.
        :type contact_address: str
        """
        contact_info = self.authors[author]
        contact_info[contact_method] = contact_address

    def set_short_description(self, path):
        """
        Set path to file with a short description of the mod.

        :param path: Path to description file.
        :type path: str
        """
        self.short_description = path

    def set_long_description(self, path):
        """
        Set path to file with a longer description of the mod.

        :param path: Path to description file.
        :type path: str
        """
        self.long_description = path

    def set_license(self, path):
        """
        Set path to a license file in the modpack.

        :param path: Path to license file.
        :type path: str
        """
        self.license = path

    def set_uid(self, uid):
        """
        Set the unique identifier of the modpack. This must be a 48-Bit
        integer.

        :param uid: Unique identifier.
        :type uid: int
        """
        self.uid = uid

    def set_url(self, url):
        """
        Set the hompage URL of the modpack.

        :param url: Homepage URL.
        :type url: str
        """
        self.url = url

    def set_version(self, version):
        """
        Set the version of the modpack.

        :param version: Version string.
        :type version: str
        """
        self.version = version

    def __repr__(self):
        return "ModpackInfo<%s>" % (self.name)
