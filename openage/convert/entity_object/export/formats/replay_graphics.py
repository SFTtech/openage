# Copyright 2021-2021 the openage authors. See copying.md for legal info.

"""
Create a replay graphics file for a game version.
"""

import toml

from ..data_definition import DataDefinition

FILE_VERSION = "1.0"


class ReplayGraphicsFile(DataDefinition):
    """
    Used for creating a replay graphics file.
    """

    def __init__(self, targetdir, filename, game_version):
        super().__init__(targetdir, filename)

        self.game_version = game_version
        self.hash_func = None
        self.replay_values = {}

    def dump(self):
        """
        Returns the replay graphics file content in TOML format.
        """

        output_dict = {}

        output_dict["file_version"] = FILE_VERSION
        output_dict["hash_algo"] = self.hash_func

        for media_type, replaydata in self.replay_values.items():
            output_dict.update({media_type.value: {}})

            for idx, replay in enumerate(replaydata):
                replay_table = output_dict[media_type.value]
                replay_table[f"file{idx}"] = {
                    "filepath": replay[0],
                    "hash": replay[1],
                    "compression_settings": replay[2],
                    "packer_settings": replay[3]
                }

        output_str = "# openage graphics replay file\n\n"
        output_str += toml.dumps(output_dict)

        return output_str

    def add_replay_data(self, media_type, filepath, filehash, compr_settings, packer_settings):
        """
        Add replay data for a file.

        :param media_type: Media type of the file (should be a graphics format)
        :type media_type: MediaType
        :param filepath: Path of the source file in the sourcedir 
                         mounted by the openage converter.
        :type filepath: str
        :param filehash: Hash value of the source file.
        :type filehash: str
        :param compr_settings: Settings for the PNG compression.
        :type compr_settings: tuple
        :param packer_settings: Settings for the packing algorithm.
        :type packer_settings: ruple
        """
        if media_type not in self.replay_values.keys():
            self.replay_values[media_type] = []

        self.replay_values[media_type].append(
            (filepath, filehash, compr_settings, packer_settings)
        )

    def set_hash_func(self, hash_func):
        """
        Set the hash function used for generating
        hash values for the graphic files.

        :param hash_func: Hash algorithm
        :type hash_func: str
        """
        self.hash_func = hash_func
