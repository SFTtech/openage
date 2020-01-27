# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Stores information about a game edition/expansion..
"""


class GameInfo:
    """
    Stores information about a game edition or expansion, mostly
    indicators to detect the version as well as paths to assets
    and data files.
    """

    def __init__(self, name, support_status, version_detect,
                 media_paths, target_modpacks):
        """
        Create a new GameInfo instance.

        :param name: Name of the game.
        :type name: str
        :param support_status: Whether the converter can read/convert
                               the game to openage formats.
        :type support_status: SupportStatus
        :param version_detect: A set of (file, [hashes]) that is unique to this
                               version of the game.
        :type version_detect: set
        :param media_paths: A dictionary with MediaType as keys and
                            (bool, [str]). bool denotes whether the path
                            is a file that requires extraction. every str is
                            a path to a file or folder.
        :type media_paths: dict
        :param target_modpacks: A list of tuples containing
                                (modpack_name, uid, expected_manifest_hash).
                                These modpacks will be created for this version.
        :type target_modpacks: list
        """

        self.name = name
        self.support_status = support_status
        self.version_detect = version_detect
        self.media_paths = media_paths
        self.target_modpacks = target_modpacks


class GameEditionInfo(GameInfo):
    """
    Info about a GameEdition.
    """

    def __init__(self, name, support_status, version_detect,
                 media_paths, target_modpacks, expansions):
        """
        Create a new GameEditionInfo instance.

        :param name: Name of the game.
        :type name: str
        :param support_status: Whether the converter can read/convert
                               the game to openage formats.
        :type support_status: SupportStatus
        :param version_detect: A set of of (file, {hash: version}) that is
                               unique to this version of the game.
        :type version_detect: set
        :param media_paths: A dictionary with MediaType as keys and
                            (bool, [str]). bool denotes whether the path
                            is a file that requires extraction. every str is
                            a path to a file or folder.
        :type media_paths: dict
        :param target_modpacks: A list of tuples containing
                                (modpack_name, uid, expected_manifest_hash).
                                These modpacks will be created for this version.
        :type target_modpacks: list
        :param expansions: A list of expansions available for this edition.
        :type expansion: list
        """
        super().__init__(name, support_status, version_detect,
                         media_paths, target_modpacks)

        self.expansions = expansions


class GameExpansionInfo(GameInfo):
    """
    Info about a GameExpansion.
    """
