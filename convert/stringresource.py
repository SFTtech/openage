from collections import defaultdict
import dataformat
from util import dbg

class StringResource:
	name_struct        = "string_resource"
	name_struct_file   = "string_resource"
	struct_description = "string id/language to text mapping, extracted from language.dll file."

	data_format = {
		0: {"id": "int32_t"},
		1: {"lang":   {"type": "char", "length": 16}},
		2: {"text":   "std::string"}
	}

	def __init__(self):
		self.strings = defaultdict(lambda: {})

	def fill_from(self, pefile):
		for lang, langstrings in pefile.strings.items():
			self.strings[lang].update(langstrings)

	def dump(self):
		ret = dict()

		ret.update(dataformat.gather_format(self))
		ret["name_table_file"] = "string_resources"
		data = []

		for lang, langstrings in self.strings.items():
			for idx, text in langstrings.items():
				entry = {
					"id": idx,
					"lang": lang,
					"text": text,
				}
				data.append(entry)

		ret["data"] = sorted(data, key = lambda x: x["id"])
		return [ret]
