# Copyright 2020-2024 the openage authors. See copying.md for legal info.
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

        AoCModpackSubprocessor._set_static_aliases(modpack, import_tree)

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

    @staticmethod
    def _set_static_aliases(modpack: Modpack, import_tree: ImportTree) -> None:
        """
        Set the aliases for the nyan import tree. The alias names
        and affected nodes are hardcoded in this function.
        """
        # Abilities from the openage API
        import_tree.add_alias(("engine", "ability", "type"), "ability")
        import_tree.add_alias(("engine", "ability", "property", "type"), "ability_prop")

        # Auxiliary objects
        import_tree.add_alias(
            ("engine", "util", "activity", "condition", "type"), "activity_condition"
        )
        import_tree.add_alias(("engine", "util", "activity", "event", "type"), "activity_event")
        import_tree.add_alias(("engine", "util", "activity", "node", "type"), "activity_node")
        import_tree.add_alias(("engine", "util", "accuracy"), "accuracy")
        import_tree.add_alias(
            ("engine", "util", "animation_override"), "animation_override"
        )
        import_tree.add_alias(("engine", "util", "attribute"), "attribute")
        import_tree.add_alias(("engine", "util", "attribute_change_type", "type"),
                              "attribute_change_type")
        import_tree.add_alias(("engine", "util", "calculation_type", "type"), "calculation_type")
        import_tree.add_alias(("engine", "util", "setup"), "civ")
        import_tree.add_alias(("engine", "util", "convert_type"), "convert_type")
        import_tree.add_alias(("engine", "util", "cost", "type"), "cost_type")
        import_tree.add_alias(("engine", "util", "create"), "create")
        import_tree.add_alias(("engine", "util", "diplomatic_stance"), "diplo_stance")
        import_tree.add_alias(
            ("engine", "util", "diplomatic_stance", "type"),
            "diplo_stance_type"
        )
        import_tree.add_alias(("engine", "util", "distribution_type", "type"), "distribution_type")
        import_tree.add_alias(("engine", "util", "dropoff_type", "type"), "dropoff_type")
        import_tree.add_alias(("engine", "util", "exchange_mode", "type"), "exchange_mode")
        import_tree.add_alias(("engine", "util", "exchange_rate"), "exchange_rate")
        import_tree.add_alias(("engine", "util", "formation"), "formation")
        import_tree.add_alias(("engine", "util", "game_entity"), "game_entity")
        import_tree.add_alias(
            ("engine", "util", "game_entity_formation"), "ge_formation"
        )
        import_tree.add_alias(("engine", "util", "game_entity_stance", "type"), "ge_stance")
        import_tree.add_alias(("engine", "util", "game_entity_type", "type"), "ge_type")
        import_tree.add_alias(
            ("engine", "util", "game_entity_type"), "game_entity_type"
        )
        import_tree.add_alias(("engine", "util", "graphics"), "graphics")
        import_tree.add_alias(("engine", "util", "herdable_mode", "type"), "herdable_mode")
        import_tree.add_alias(("engine", "util", "hitbox"), "hitbox")
        import_tree.add_alias(("engine", "util", "move_mode", "type"), "move_mode")
        import_tree.add_alias(("engine", "util", "language"), "lang")
        import_tree.add_alias(("engine", "util", "language", "translated", "type"), "translated")
        import_tree.add_alias(("engine", "util", "logic", "gate", "type"), "logic_gate")
        import_tree.add_alias(("engine", "util", "logic", "literal", "type"), "literal")
        import_tree.add_alias(("engine", "util", "logic", "literal_scope", "type"), "literal_scope")
        import_tree.add_alias(("engine", "util", "patch"), "patch")
        import_tree.add_alias(("engine", "util", "patch", "property", "type"), "patch_prop")
        import_tree.add_alias(("engine", "util", "path_type"), "path_type")
        import_tree.add_alias(("engine", "util", "payment_mode", "type"), "payment_mode")
        import_tree.add_alias(("engine", "util", "placement_mode", "type"), "placement_mode")
        import_tree.add_alias(("engine", "util", "price_mode", "type"), "price_mode")
        import_tree.add_alias(("engine", "util", "price_pool"), "price_pool")
        import_tree.add_alias(("engine", "util", "production_mode", "type"), "production_mode")
        import_tree.add_alias(("engine", "util", "progress"), "progress")
        import_tree.add_alias(("engine", "util", "progress", "property", "type"), "progress_prop")
        import_tree.add_alias(("engine", "util", "progress_status"), "progress_status")
        import_tree.add_alias(("engine", "util", "progress_type", "type"), "progress_type")
        import_tree.add_alias(("engine", "util", "research"), "research")
        import_tree.add_alias(("engine", "util", "resource"), "resource")
        import_tree.add_alias(("engine", "util", "resource_spot"), "resource_spot")
        import_tree.add_alias(("engine", "util", "selection_box", "type"), "selection_box")
        import_tree.add_alias(("engine", "util", "sound",), "sound")
        import_tree.add_alias(("engine", "util", "state_machine"), "state_machine")
        import_tree.add_alias(("engine", "util", "storage"), "storage")
        import_tree.add_alias(("engine", "util", "target_mode", "type"), "target_mode")
        import_tree.add_alias(("engine", "util", "tech"), "tech")
        import_tree.add_alias(("engine", "util", "terrain"), "terrain")
        import_tree.add_alias(("engine", "util", "terrain_type"), "terrain_type")
        import_tree.add_alias(("engine", "util", "trade_route", "type"), "trade_route")
        import_tree.add_alias(("engine", "util", "variant", "type"), "variant")

        # Effect objects
        import_tree.add_alias(("engine", "effect", "property", "type"), "effect_prop")
        import_tree.add_alias(
            ("engine", "effect", "continuous", "flat_attribute_change", "type"),
            "econt_flac"
        )
        import_tree.add_alias(
            ("engine", "effect", "continuous", "time_relative_progress", "type"),
            "econt_trp"
        )
        import_tree.add_alias(
            ("engine", "effect", "continuous", "time_relative_attribute", "type"),
            "econt_tra"
        )
        import_tree.add_alias(
            ("engine", "effect", "discrete", "convert", "type"),
            "edisc_conv"
        )
        import_tree.add_alias(
            ("engine", "effect", "discrete", "flat_attribute_change", "type"),
            "edisc_flac"
        )
        import_tree.add_alias(("engine", "resistance", "property", "type"), "resist_prop")
        import_tree.add_alias(
            ("engine", "resistance", "continuous", "flat_attribute_change", "type"),
            "rcont_flac"
        )
        import_tree.add_alias(
            ("engine", "resistance", "continuous", "time_relative_progress", "type"),
            "rcont_trp"
        )
        import_tree.add_alias(
            ("engine", "resistance", "continuous", "time_relative_attribute", "type"),
            "rcont_tra"
        )
        import_tree.add_alias(
            ("engine", "resistance", "discrete", "convert", "type"),
            "rdisc_conv"
        )
        import_tree.add_alias(
            ("engine", "resistance", "discrete", "flat_attribute_change", "type"),
            "rdisc_flac"
        )

        # Modifier objects
        import_tree.add_alias(
            ("engine", "modifier", "effect", "flat_attribute_change", "type"),
            "me_flac"
        )

        # Aliases for objects from the modpack itself

        # Prefix aliases to prevent naming conflucts with 'engine'
        prefix = modpack.name + "_"

        # Auxiliary objects
        import_tree.add_alias(
            (modpack.name, "data", "util", "attribute", "types"),
            prefix + "attribute"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "attribute_change_type", "types"),
            prefix + "attr_change_type"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "construct_type", "types"),
            prefix + "construct_type"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "convert_type", "types"),
            prefix + "convert_type"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "diplomatic_stance", "types"),
            prefix + "diplo_stance"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "game_entity_type", "types"),
            prefix + "ge_type"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "formation", "types"),
            prefix + "formation"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "formation", "subformations"),
            prefix + "subformations"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "language", "language"),
            prefix + "lang"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "logic", "death", "death"),
            "death_condition"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "logic", "garrison_empty",
             "garrison_empty"),
            "empty_garrison_condition"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "path_type", "types"),
            prefix + "path_type"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "resource", "market_trading"),
            prefix + "market_trading"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "resource", "types"),
            prefix + "resource"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "terrain_type", "types"),
            prefix + "terrain_type"
        )

        # Effect objects
        import_tree.add_alias(
            (modpack.name, "data", "effect", "discrete", "flat_attribute_change",
             "fallback"),
            "attack_fallback"
        )
        import_tree.add_alias(
            (modpack.name, "data", "effect", "discrete", "flat_attribute_change",
             "min_damage"),
            "min_damage"
        )
        import_tree.add_alias(
            (modpack.name, "data", "effect", "discrete", "flat_attribute_change",
             "min_heal"),
            "min_heal"
        )

        # Modifier objects
        import_tree.add_alias(
            (modpack.name, "data", "util", "modifier", "elevation_difference",
             "elevation_difference"),
            prefix + "mme_elev_high"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "modifier", "elevation_difference",
             "elevation_difference"),
            prefix + "mme_elev_low"
        )
        import_tree.add_alias(
            (modpack.name, "data", "util", "modifier", "flyover_cliff",
             "flyover_cliff"),
            prefix + "mme_cliff_attack"
        )

        # Terrain objects
        import_tree.add_alias(
            (modpack.name, "data", "terrain", "foundation", "foundation"),
            prefix + "foundation"
        )

        # Generic aliases
        # TODO: Make this less hacky
        fqon = (modpack.name, "data", "game_entity", "generic")
        current_node = import_tree.root
        for part in fqon:
            current_node = current_node.get_child(part)

        for child in current_node.children.values():
            current_node = child

            # These are folders and should have unique names
            alias_name = f"ge_{current_node.name}"

            for subchild in current_node.children.values():
                if subchild.name in ("graphics", "sounds", "projectiles"):
                    continue

                if subchild.name.endswith("upgrade"):
                    alias = f"{alias_name}_{subchild.name}"
                    subchild.set_alias(alias)
                    continue

                # One level deeper: This should be the nyan file
                current_node = subchild

                alias = f"ge_{current_node.name}"

                # Use the file name as alias for the file
                current_node.set_alias(alias)

        fqon = (modpack.name, "data", "tech", "generic")
        current_node = import_tree.root
        for part in fqon:
            current_node = current_node.get_child(part)

        for child in current_node.children.values():
            current_node = child

            # These are folders and should have unique names
            alias_name = "tech_" + current_node.name

            # One level deeper: This should be the nyan file
            current_node = current_node.children[current_node.name]

            # Set the folder name as alias for the file
            current_node.set_alias(alias_name)

        fqon = (modpack.name, "data", "civ")
        current_node = import_tree.root
        for part in fqon:
            current_node = current_node.get_child(part)

        for child in current_node.children.values():
            current_node = child

            # These are folders and should have unique names
            alias_name = "civ_" + current_node.name

            # One level deeper: This should be the nyan file
            current_node = current_node.children[current_node.name]

            # Set the folder name as alias for the file
            current_node.set_alias(alias_name)

        fqon = (modpack.name, "data", "terrain")
        current_node = import_tree.root
        for part in fqon:
            current_node = current_node.get_child(part)

        for child in current_node.children.values():
            current_node = child

            # These are folders and should have unique names
            alias_name = "terrain_" + current_node.name

            # One level deeper: This should be the nyan file
            current_node = current_node.children[current_node.name]

            # Set the folder name as alias for the file
            current_node.set_alias(alias_name)
