import dataformat
import util
from util import dbg

class SoundList(dataformat.Exportable):
    name_struct        = "sound_file"
    name_struct_file   = "sound_file"
    struct_description = "one available sound file."

    data_format = (
        (True, "category", dataformat.EnumMember(
            type_name="audio_category_t",
            file_name="sound_file",
            values =(
                "GAME",
                "INTERFACE",
                "MUSIC",
                "TAUNT",
            )
        )),

        (True, "sound_id", "int"),
        (True, "path",     "std::string"),

        (True, "format", dataformat.EnumMember("audio_format_t", "sound_file", (
            "OPUS",
            "WAV",
            "FLAC",
            "MP3",
        ))),

        (True, "loader_policy", dataformat.EnumMember("audio_loader_policy_t", "sound_file", (
            "IN_MEMORY",
            "DYNAMIC",
        ))),
    )

    def __init__(self):
        self.sounds = list()

    def add_sound(self, sndid, filename, sndformat):
        #TODO: actually determine category
        new_sound = {
            "category":      "GAME",
            "sound_id":      sndid,
            "path":          filename,
            "format":        sndformat.upper(),
            "loader_policy": "IN_MEMORY",
        }
        self.sounds.append(new_sound)

    def dump(self):
        return [ dataformat.DataDefinition(self, self.sounds, "sound_list") ]

    @classmethod
    def structs(cls):
        return [ dataformat.StructDefinition(cls) ]
