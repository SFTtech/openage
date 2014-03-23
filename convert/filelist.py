import dataformat
import util
from util import dbg

class SoundList:
	name_struct        = "sound_file"
	name_struct_file   = "sound"
	struct_description = "one available sound file."

	data_format = {
		0: {"category": {
			"type": "enum",
			"name": "audio_category_t",
			"filename": "sound",
			"values": {
				"GAME",
				"INTERFACE",
				"MUSIC",
				"TAUNT",
			}
		}},

		1: {"sound_id": "int"},
		2: {"path": "std::string"},

		3: {"format": {
			"type": "enum",
			"name": "audio_format_t",
			"filename": "sound",
			"values": {
				"OPUS",
				"WAV",
				"FLAC",
				"MP3",
			}
		}},

		4: {"loader_policy": {
			"type": "enum",
			"name": "audio_loader_policy_t",
			"filename": "sound",
			"values": {
				"IN_MEMORY",
				"DYNAMIC",
			}
		}},
	}

	def __init__(self):
		self.sounds = list()

	def append(self, sndid, filename, sndformat):
		#TODO: actually determine category
		new_sound = {
			"category":      "GAME",
			"sound_id":      sndid,
			"path":          filename,
			"format":        sndformat.upper(),
			"loader_policy": "IN_MEMORY",
		}
		self.sounds.append(new_sound)

	def metadata(self):
		ret = dict()

		ret.update(dataformat.gather_format(self))
		ret["name_table_file"] = "sound_list"
		ret["data"] = self.sounds

		return [ ret ]

	def structs():
		ret = dataformat.gather_format(SoundList)
		return [ ret ]
