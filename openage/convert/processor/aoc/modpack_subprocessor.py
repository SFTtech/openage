# Copyright 2020-2020 the openage authors. See copying.md for legal info.

"""
Organize export data (nyan objects, media, scripts, etc.)
into modpacks.
"""
from openage.convert.dataformat.modpack import Modpack
from openage.convert.dataformat.aoc.expected_pointer import ExpectedPointer
from openage.convert.export.formats.nyan_file import NyanFile


class AoCModpackSubprocessor:

    @classmethod
    def get_modpacks(cls, gamedata):

        aoe2_base = cls._get_aoe2_base(gamedata)

        return [aoe2_base]

    @classmethod
    def _get_aoe2_base(cls, gamedata):
        """
        Create the aoe2-base modpack.
        """
        modpack = Modpack("aoe2-base")

        mod_def = modpack.get_info()

        mod_def.set_version("1.0c")
        mod_def.set_uid(2000)

        mod_def.add_assets_to_load("data/*")

        cls._organize_nyan_objects(modpack, gamedata)
        cls._organize_media_objects(modpack, gamedata)

        return modpack

    @staticmethod
    def _organize_nyan_objects(modpack, full_data_set):
        """
        Put available nyan objects into a given modpack.
        """
        created_nyan_files = {}

        # Access all raw API objects
        raw_api_objects = []
        raw_api_objects.extend(full_data_set.pregen_nyan_objects.values())

        for unit_line in full_data_set.unit_lines.values():
            raw_api_objects.extend(unit_line.get_raw_api_objects().values())
        for building_line in full_data_set.building_lines.values():
            raw_api_objects.extend(building_line.get_raw_api_objects().values())
        for variant_group in full_data_set.variant_groups.values():
            raw_api_objects.extend(variant_group.get_raw_api_objects().values())

        # TODO: Other lines?

        for raw_api_object in raw_api_objects:
            obj_location = raw_api_object.get_location()

            if isinstance(obj_location, ExpectedPointer):
                # Resolve location and add nested object
                nyan_object = obj_location.resolve()
                nyan_object.add_nested_object(raw_api_object.get_nyan_object())
                continue

            obj_filename = raw_api_object.get_filename()
            nyan_file_path = "%s/%s%s" % (modpack.info.name,
                                          obj_location,
                                          obj_filename)

            if nyan_file_path in created_nyan_files.keys():
                nyan_file = created_nyan_files[nyan_file_path]

            else:
                nyan_file = NyanFile(obj_location, obj_filename,
                                     modpack.info.name)
                created_nyan_files.update({nyan_file.get_relative_file_path(): nyan_file})
                modpack.add_data_export(nyan_file)

            nyan_file.add_nyan_object(raw_api_object.get_nyan_object())

    @staticmethod
    def _organize_media_objects(modpack, full_data_set):
        """
        Put export requests and sprite files into as given modpack.
        """
        for graphic_export in full_data_set.graphics_exports.values():
            modpack.add_media_export(graphic_export)
