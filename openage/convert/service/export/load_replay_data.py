# Copyright 2021-2021 the openage authors. See copying.md for legal info.

"""
Load data of replay files.
"""
import toml


def load_graphics_replay_data(filepath):
    """
    Parses a replay graphics file and returns it as a dict.
    """
    output_dict = {}

    with filepath.open() as infile:
        replaydata = toml.loads(infile.read())

    replaydata.pop("file_version")
    replaydata.pop("hash_algo")

    replay_files = list(replaydata["graphics"].values())
    for replayinfo in replay_files:
        output_dict.update(
            {
                replayinfo["filepath"]: {
                    "compr_settings": replayinfo["compression_settings"],
                    "packer_settings": replayinfo["packer_settings"]
                }
            }
        )

    return output_dict
