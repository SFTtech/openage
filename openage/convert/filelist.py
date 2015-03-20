# Copyright 2014-2015 the openage authors. See copying.md for legal info.

# TODO pylint: disable=C

from .dataformat.exportable import Exportable
from .dataformat.members import EnumMember
from .dataformat.data_definition import DataDefinition
from .dataformat.struct_definition import StructDefinition


class SoundList(Exportable):
    name_struct = "sound_file"
    name_struct_file = "sound_file"
    struct_description = "one available sound file."

    data_format = (
        (True, "category", EnumMember(
            type_name="audio_category_t",
            values=(
                "GAME",
                "INTERFACE",
                "MUSIC",
                "TAUNT",
            ),
            file_name="sound_file",
        )),

        (True, "sound_id", "int"),
        (True, "path",     "std::string"),

        (True, "format", EnumMember(
            type_name="audio_format_t",
            values=(
                "OPUS",
                "WAV",
                "FLAC",
                "MP3",
            ),
            file_name="sound_file",
        )),

        (True, "loader_policy", EnumMember(
            type_name="audio_loader_policy_t",
            values=(
                "IN_MEMORY",
                "DYNAMIC",
            ),
            file_name="sound_file",
        )),
    )

    def __init__(self):
        super().__init__()

        self.sounds = list()

    def add_sound(self, sndid, filename, sndformat):
        # TODO: actually determine category instead of hardcoding
        new_sound = {
            "category":      "GAME",
            "sound_id":      sndid,
            "path":          filename,
            "format":        sndformat.upper(),
            "loader_policy": "IN_MEMORY",
        }
        self.sounds.append(new_sound)

    def dump(self, filename="sound_list"):
        return [DataDefinition(self, self.sounds, filename)]

    @classmethod
    def structs(cls):
        return [StructDefinition(cls)]
