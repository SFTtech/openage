# Copyright 2014-2014 the openage authors. See copying.md for legal info.

from collections import defaultdict

from .dataformat import exportable, data_definition, struct_definition
from openage.log import dbg


class StringResource(exportable.Exportable):
    name_struct = "string_resource"
    name_struct_file = "string_resource"
    struct_description = "string id/language to text mapping,"\
                         " extracted from language.dll file."

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
        return [data_definition.DataDefinition(self, data, filename)]

    @classmethod
    def structs(cls):
        return [struct_definition.StructDefinition(cls)]
