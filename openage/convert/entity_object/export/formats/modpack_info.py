# Copyright 2020-2024 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-instance-attributes,too-many-arguments

"""
Modpack definition file.
"""
import toml

from ..data_definition import DataDefinition


FILE_VERSION = "2"


class ModpackInfo(DataDefinition):
    """
    Represents the header file of the modpack. Contains info for loading data
    and about the creators of the modpack.
    """

    def __init__(self, targetdir: str, filename: str):
        super().__init__(targetdir, filename)

        # Info
        self.packagename: str = None
        self.version: str = None
        self.versionstr: str = None
        self.extra_info: dict[str, str] = {}

        # Assets
        self.includes: list[str] = []
        self.excludes: list[str] = []

        # Dependency
        self.requires: list[str] = []

        # Conflict
        self.conflicts: list[str] = []

        # Authors
        self.authors: dict[str, str] = {}

        # Author groups
        self.author_groups: dict[str, str] = {}

    def add_author(
        self,
        name: str,
        fullname: str = None,
        since: str = None,
        until: str = None,
        roles: str = None,
        contact: str = None
    ) -> None:
        """
        Adds an author with optional contact info.

        :param name: Nickname of the author. Must be unique for the modpack.
        :type name: str
        :param fullname: Full name of the author.
        :type fullname: str
        :param since: Version number of the release where the author started to contribute.
        :type since: str
        :param until: Version number of the release where the author stopped to contribute.
        :type until: str
        :param roles: List of roles of the author during the creation of the modpack.
        :type roles: list
        :param contact: Dictionary with contact info. See the spec
                        for available parameters.
        :type contact: dict
        """
        author = {}
        author["name"] = name
        if fullname:
            author["fullname"] = fullname

        if since:
            author["since"] = since

        if until:
            author["until"] = until

        if roles:
            author["roles"] = roles

        if contact:
            author["contact"] = contact

        self.authors[name] = author

    def add_author_group(
        self,
        name: str,
        authors: list[str],
        description: str = None
    ) -> None:
        """
        Adds an author with optional contact info.

        :param name: Group or team name.
        :type name: str
        :param authors: List of author identifiers. These must match up
                        with subtable keys in the self.authors.
        :type authors: list
        :param description: Path to a file with a description of the team.
        :type description: str
        """
        author_group = {}
        author_group["name"] = name
        author_group["authors"] = authors
        if description:
            author_group["description"] = description

        self.author_groups[name] = author_group

    def add_include(self, path: str) -> None:
        """
        Add a path to an asset that is loaded by the modpack.

        :param path: Path to assets that should be mounted on load time.
        :type path: str
        """
        self.includes.append(path)

    def add_exclude(self, path: str) -> None:
        """
        Add a path to an asset that excluded from loading.

        :param path: Path to assets.
        :type path: str
        """
        self.excludes.append(path)

    def add_conflict(self, modpack_id: str) -> None:
        """
        Add an identifier of another modpack that has a conflict with this modpack.

        :param modpack_id: Modpack alias or identifier.
        :type modpack_id: str
        """
        self.conflicts.append(modpack_id)

    def add_dependency(self, modpack_id: str) -> None:
        """
        Add an identifier of another modpack that is a dependency of this modpack.

        :param modpack_id: Modpack alias or identifier.
        :type modpack_id: str
        """
        self.requires.append(modpack_id)

    def set_info(
        self,
        packagename: str,
        modpack_version: str,
        versionstr: str = None,
        repo: str = None,
        alias: str = None,
        title: str = None,
        description: str = None,
        long_description: str = None,
        url: str = None,
        licenses: str = None
    ) -> None:
        """
        Set the general information about the modpack.

        :param packagename: Name of the modpack.
        :type packagename: str
        :param modpack_version: Internal version number. Must have semver format.
        :type modpack_version: str
        :param versionstr: Human-readable version number. Can be anything.
        :type versionstr: str
        :param repo: Name of the repo where the package is hosted.
        :type repo: str
        :param alias: Alias of the modpack.
        :type alias: str
        :param title: Title used in UI.
        :type title: str
        :param description: Path to a file with a short description (max 500 chars).
        :type description: str
        :param long_description: Path to a file with a detailed description.
        :type long_description: str
        :param url: Link to the modpack's website.
        :type url: str
        :param licenses: License(s) of the modpack.
        :type licenses: list
        """
        self.packagename = packagename
        self.version = modpack_version

        if versionstr:
            self.extra_info["versionstr"] = versionstr

        if repo:
            self.extra_info["repo"] = repo

        if alias:
            self.extra_info["alias"] = alias

        if title:
            self.extra_info["title"] = title

        if description:
            self.extra_info["description"] = description

        if long_description:
            self.extra_info["long_description"] = long_description

        if url:
            self.extra_info["url"] = url

        if licenses:
            self.extra_info["licenses"] = licenses

    def dump(self) -> str:
        """
        Outputs the modpack info to the TOML output format.
        """
        output_str = "# openage modpack definition file\n\n"
        output_dict = {}

        # File version
        output_dict.update({"file_version": FILE_VERSION})

        # info table
        if not self.packagename:
            raise RuntimeError(f"{self}: packagename needs to be defined before dumping.")

        if not self.version:
            raise RuntimeError(f"{self}: version needs to be defined before dumping.")

        info_table = {"info": {}}
        info_table["info"].update(
            {
                "name": self.packagename,
                "version": self.version
            }
        )
        info_table["info"].update(self.extra_info)

        output_dict.update(info_table)

        # assets table
        assets_table = {"assets": {}}
        assets_table["assets"].update(
            {
                "include": self.includes,
                "exclude": self.excludes
            }
        )

        output_dict.update(assets_table)

        # dependency table
        dependency_table = {"dependency": {}}
        dependency_table["dependency"].update({"modpacks": self.requires})

        output_dict.update(dependency_table)

        # conflicts table
        conflicts_table = {"conflict": {}}
        conflicts_table["conflict"].update({"modpacks": self.conflicts})

        output_dict.update(conflicts_table)

        # authors table
        authors_table = {"authors": {}}
        authors_table["authors"].update(self.authors)

        output_dict.update(authors_table)

        # authorgroups table
        authorgroups_table = {"authorgroups": {}}
        authorgroups_table["authorgroups"].update(self.author_groups)

        output_dict.update(authorgroups_table)

        output_str += toml.dumps(output_dict)

        return output_str

    def __repr__(self):
        return f"ModpackInfo<{self.packagename}>"
