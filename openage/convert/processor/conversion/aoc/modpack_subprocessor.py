# Copyright 2020-2025 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-branches,too-few-public-methods,too-many-statements

"""
Organize export data (nyan objects, media, scripts, etc.)
into modpacks.
"""
from __future__ import annotations
import typing


from .....nyan.import_tree import ImportTree
from ....entity_object.conversion.modpack import Modpack
from ....entity_object.export.formats.nyan_file import NyanFile
from ....value_object.conversion.forward_ref import ForwardRef

from .modpack.import_tree import set_static_aliases

if typing.TYPE_CHECKING:
    from openage.convert.entity_object.conversion.aoc.genie_object_container\
        import GenieObjectContainer
    from openage.convert.entity_object.conversion.converter_object import RawAPIObject


class AoCModpackSubprocessor:
    """
    Creates the modpacks containing the nyan files and media from the AoC conversion.
    """

    @classmethod
    def get_modpacks(cls, full_data_set: GenieObjectContainer) -> list[Modpack]:
        """
        Return all modpacks that can be created from the gamedata.
        """
        aoe2_base = cls._get_aoe2_base(full_data_set)

        return [aoe2_base]

    @classmethod
    def _get_aoe2_base(cls, full_data_set: GenieObjectContainer) -> Modpack:
        """
        Create the aoe2-base modpack.
        """
        modpack = Modpack("aoe2_base")

        mod_def = modpack.get_info()

        targetmod_info = full_data_set.game_version.edition.target_modpacks["aoe2_base"]
        version = targetmod_info["version"]
        versionstr = targetmod_info["versionstr"]
        mod_def.set_info("aoe2_base", version, versionstr=versionstr, repo="openage")

        mod_def.add_include("data/**")

        cls.organize_nyan_objects(modpack, full_data_set)
        cls.organize_media_objects(modpack, full_data_set)

        return modpack

    @staticmethod
    def organize_nyan_objects(modpack: Modpack, full_data_set: GenieObjectContainer) -> None:
        """
        Put available nyan objects into a given modpack.
        """
        created_nyan_files: dict[str, NyanFile] = {}

        # Access all raw API objects
        raw_api_objects: list[RawAPIObject] = []
        raw_api_objects.extend(full_data_set.pregen_nyan_objects.values())

        for unit_line in full_data_set.unit_lines.values():
            raw_api_objects.extend(unit_line.get_raw_api_objects().values())

        for building_line in full_data_set.building_lines.values():
            raw_api_objects.extend(building_line.get_raw_api_objects().values())

        for ambient_group in full_data_set.ambient_groups.values():
            raw_api_objects.extend(ambient_group.get_raw_api_objects().values())

        for variant_group in full_data_set.variant_groups.values():
            raw_api_objects.extend(variant_group.get_raw_api_objects().values())

        for tech_group in full_data_set.tech_groups.values():
            raw_api_objects.extend(tech_group.get_raw_api_objects().values())

        for terrain_group in full_data_set.terrain_groups.values():
            raw_api_objects.extend(terrain_group.get_raw_api_objects().values())

        for civ_group in full_data_set.civ_groups.values():
            raw_api_objects.extend(civ_group.get_raw_api_objects().values())

        # Create the files
        for raw_api_object in raw_api_objects:
            obj_location = raw_api_object.get_location()

            if isinstance(obj_location, ForwardRef):
                # Resolve location and add nested object
                nyan_object = obj_location.resolve()
                nyan_object.add_nested_object(raw_api_object.get_nyan_object())
                continue

            obj_filename = raw_api_object.get_filename()
            nyan_file_path = f"{modpack.name}/{obj_location}{obj_filename}"

            if nyan_file_path in created_nyan_files:
                nyan_file = created_nyan_files[nyan_file_path]

            else:
                nyan_file = NyanFile(obj_location, obj_filename,
                                     modpack.name)
                created_nyan_files.update({nyan_file.get_relative_file_path(): nyan_file})
                modpack.add_data_export(nyan_file)

            nyan_file.add_nyan_object(raw_api_object.get_nyan_object())

        # Create an import tree from the files
        import_tree = ImportTree()

        for nyan_file in created_nyan_files.values():
            import_tree.expand_from_file(nyan_file)

        for nyan_object in full_data_set.nyan_api_objects.values():
            import_tree.expand_from_object(nyan_object)

        for nyan_file in created_nyan_files.values():
            nyan_file.set_import_tree(import_tree)

        set_static_aliases(modpack.name, import_tree)

    @staticmethod
    def organize_media_objects(modpack: Modpack, full_data_set: GenieObjectContainer) -> None:
        """
        Put export requests and sprite files into as given modpack.
        """
        for graphic_export in full_data_set.graphics_exports.values():
            modpack.add_media_export(graphic_export)

        for blend_export in full_data_set.blend_exports.values():
            modpack.add_media_export(blend_export)

        for sound_export in full_data_set.sound_exports.values():
            modpack.add_media_export(sound_export)

        for metadata_file in full_data_set.metadata_exports:
            modpack.add_metadata_export(metadata_file)
