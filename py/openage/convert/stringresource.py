from collections import defaultdict
from . import dataformat
from .util import dbg

class StringResource(dataformat.Exportable):
    name_struct        = "string_resource"
    name_struct_file   = "string_resource"
    struct_description = "string id/language to text mapping, extracted from language.dll file."

    data_format = (
        (True, "id",   "int32_t"),
        (True, "lang", "char[16]"),
        (True, "text", "std::string"),
    )

    def __init__(self):
        self.strings = defaultdict(lambda: {})

    def fill_from(self, pefile):
        for lang, langstrings in pefile.strings.items():
            self.strings[lang].update(langstrings)

    def dump(self, filename):
        data = list()

        for lang, langstrings in self.strings.items():
            for idx, text in langstrings.items():
                entry = {
                    "id": idx,
                    "lang": lang,
                    "text": text,
                }
                data.append(entry)

        data = sorted(data, key=lambda x: x["id"])
        return [ dataformat.DataDefinition(self, data, filename) ]

    @classmethod
    def structs(cls):
        return [ dataformat.StructDefinition(cls) ]
