# Copyright 2020-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-locals,too-many-branches,too-few-public-methods

"""
Organize export data (nyan objects, media, scripts, etc.)
into modpacks.
"""
from .....nyan.import_tree import ImportTree
from ....entity_object.conversion.modpack import Modpack
from ....entity_object.export.formats.nyan_file import NyanFile
from ....value_object.conversion.forward_ref import ForwardRef


class AoCModpackSubprocessor:
    """
    Creates the modpacks containing the nyan files and media from the AoC conversion.
    """

    @classmethod
    def get_modpacks(cls, gamedata):
        """
        Return all modpacks that can be created from the gamedata.
        """
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

        cls.organize_nyan_objects(modpack, gamedata)
        cls.organize_media_objects(modpack, gamedata)

        return modpack

    @staticmethod
    def organize_nyan_objects(modpack, full_data_set):
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
            nyan_file_path = f"{modpack.info.name}/{obj_location}{obj_filename}"

            if nyan_file_path in created_nyan_files.keys():
                nyan_file = created_nyan_files[nyan_file_path]

            else:
                nyan_file = NyanFile(obj_location, obj_filename,
                                     modpack.info.name)
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

        AoCModpackSubprocessor._set_static_aliases(import_tree)

    @staticmethod
    def organize_media_objects(modpack, full_data_set):
        """
        Put export requests and sprite files into as given modpack.
        """
        for graphic_export in full_data_set.graphics_exports.values():
            modpack.add_media_export(graphic_export)

        for sound_export in full_data_set.sound_exports.values():
            modpack.add_media_export(sound_export)

        for metadata_file in full_data_set.metadata_exports:
            modpack.add_metadata_export(metadata_file)

    @staticmethod
    def _set_static_aliases(import_tree):
        """
        Set the aliases for the nyan import tree. The alias names
        and affected nodes are hardcoded in this function.
        """
        # Abilities from the openage API
        import_tree.add_alias(("engine", "ability", "type"), "ablty")
        import_tree.add_alias(("engine", "ability", "specialization"), "ablty_spez")

        # Auxiliary objects
        import_tree.add_alias(("engine", "aux", "accuracy", "Accuracy"), "Accuracy")
        import_tree.add_alias(("engine", "aux", "animation_override", "AnimationOverride"), "AnimationOverride")
        import_tree.add_alias(("engine", "aux", "attribute"), "attribute")
        import_tree.add_alias(("engine", "aux", "attribute_change_type"), "AttributeChangeType")
        import_tree.add_alias(("engine", "aux", "calculation_type", "type"), "calculation_type")
        import_tree.add_alias(("engine", "aux", "civilization", "Civilization"), "Civ")
        import_tree.add_alias(("engine", "aux", "convert_type", "ConvertType"), "ConvertType")
        import_tree.add_alias(("engine", "aux", "cost", "type"), "cost_type")
        import_tree.add_alias(("engine", "aux", "create"), "create")
        import_tree.add_alias(("engine", "aux", "diplomatic_stance"), "diplo_stance")
        import_tree.add_alias(("engine", "aux", "diplomatic_stance", "type", "Self"), "diplo_stance_self")
        import_tree.add_alias(("engine", "aux", "distribution_type", "type"), "distribution_type")
        import_tree.add_alias(("engine", "aux", "dropoff_type", "type"), "dropoff_type")
        import_tree.add_alias(("engine", "aux", "exchange_mode", "type"), "exchange_mode")
        import_tree.add_alias(("engine", "aux", "exchange_rate", "ExchangeRate"), "ExchangeRate")
        import_tree.add_alias(("engine", "aux", "formation"), "formation")
        import_tree.add_alias(("engine", "aux", "game_entity", "GameEntity"), "GameEntity")
        import_tree.add_alias(("engine", "aux", "game_entity_formation", "GameEntityFormation"), "GameEntityFormation")
        import_tree.add_alias(("engine", "aux", "game_entity_stance", "type"), "ge_stance")
        import_tree.add_alias(("engine", "aux", "game_entity_type", "type"), "ge_type")
        import_tree.add_alias(("engine", "aux", "game_entity_type", "GameEntityType"), "GameEntityType")
        import_tree.add_alias(("engine", "aux", "graphics", "Animation"), "Animation")
        import_tree.add_alias(("engine", "aux", "graphics", "Terrain"), "TerrainGraphics")
        import_tree.add_alias(("engine", "aux", "herdable_mode", "type"), "herdable_mode")
        import_tree.add_alias(("engine", "aux", "hitbox", "Hitbox"), "Hitbox")
        import_tree.add_alias(("engine", "aux", "move_mode", "type"), "move_mode")
        import_tree.add_alias(("engine", "aux", "language"), "language")
        import_tree.add_alias(("engine", "aux", "logic", "gate", "type"), "logic_gate")
        import_tree.add_alias(("engine", "aux", "logic", "literal", "type"), "literal")
        import_tree.add_alias(("engine", "aux", "logic", "literal_scope", "type"), "literal_scope")
        import_tree.add_alias(("engine", "aux", "patch"), "patch")
        import_tree.add_alias(("engine", "aux", "passable_mode", "type"), "passable_mode")
        import_tree.add_alias(("engine", "aux", "payment_mode", "type"), "payment_mode")
        import_tree.add_alias(("engine", "aux", "placement_mode", "type"), "placement_mode")
        import_tree.add_alias(("engine", "aux", "price_change", "PriceChange"), "PriceChange")
        import_tree.add_alias(("engine", "aux", "price_mode", "dynamic", "type"), "dynamic_price_mode")
        import_tree.add_alias(("engine", "aux", "price_pool", "PricePool"), "PricePool")
        import_tree.add_alias(("engine", "aux", "production_mode", "type"), "production_mode")
        import_tree.add_alias(("engine", "aux", "progress", "type"), "progress")
        import_tree.add_alias(("engine", "aux", "progress", "specialization"), "progress_spez")
        import_tree.add_alias(("engine", "aux", "progress_status", "ProgressStatus"), "ProgressStatus")
        import_tree.add_alias(("engine", "aux", "progress_type", "type"), "progress_type")
        import_tree.add_alias(("engine", "aux", "research", "ResearchableTech"), "ResearchableTech")
        import_tree.add_alias(("engine", "aux", "resource"), "resource")
        import_tree.add_alias(("engine", "aux", "resource_spot", "ResourceSpot"), "ResourceSpot")
        import_tree.add_alias(("engine", "aux", "selection_box", "type"), "selection_box")
        import_tree.add_alias(("engine", "aux", "sound", "Sound"), "Sound")
        import_tree.add_alias(("engine", "aux", "state_machine", "StateChanger"), "StateChanger")
        import_tree.add_alias(("engine", "aux", "storage"), "storage")
        import_tree.add_alias(("engine", "aux", "target_mode", "type"), "target_mode")
        import_tree.add_alias(("engine", "aux", "tech", "Tech"), "Tech")
        import_tree.add_alias(("engine", "aux", "terrain", "Terrain"), "Terrain")
        import_tree.add_alias(("engine", "aux", "terrain", "TerrainAmbient"), "TerrainAmbient")
        import_tree.add_alias(("engine", "aux", "terrain_type", "TerrainType"), "TerrainType")
        import_tree.add_alias(("engine", "aux", "trade_route", "type"), "trade_route")
        import_tree.add_alias(("engine", "aux", "translated", "type"), "translated")
        import_tree.add_alias(("engine", "aux", "variant", "type"), "variant")

        # Effect objects
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
        import_tree.add_alias(("engine", "effect", "specialization"), "effect_spez")
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
        import_tree.add_alias(("engine", "resistance", "specialization"), "resist_spez")

        # Modifier objects
        import_tree.add_alias(
            ("engine", "modifier", "multiplier", "effect", "flat_attribute_change", "type"),
            "mme_flac"
        )
