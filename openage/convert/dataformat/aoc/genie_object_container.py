# Copyright 2019-2019 the openage authors. See copying.md for legal info.

from ...dataformat.converter_object import ConverterObjectContainer


class GenieObjectContainer(ConverterObjectContainer):
    """
    Contains everything from the dat file, sorted into several
    categories.

    Newly created instances of ConverterObject and ConverterObjectGroup
    should add themselves to the object's dicts during initilization.
    """

    def __init__(self):

        # Phase 1: Genie-like objects
        # ConverterObject types (the data from the game)
        # key: obj_id; value: ConverterObject instance
        self.genie_units = {}
        self.genie_techs = {}
        self.genie_effect_bundles = {}
        self.genie_civs = {}
        self.age_connections = {}
        self.building_connections = {}
        self.unit_connections = {}
        self.tech_connections = {}
        self.genie_graphics = {}
        self.genie_sounds = {}
        self.genie_terrains = {}

        # Phase 2: API-like objects
        # ConverterObjectGroup types (things that will become
        # nyan objects)
        # key: group_id; value: ConverterObjectGroup instance
        self.unit_lines = {}
        self.building_lines = {}
        self.task_groups = {}
        self.transform_groups = {}
        self.villager_groups = {}
        self.monk_groups = {}
        self.variant_groups = {}
        self.civ_groups = {}
        self.tech_groups = {}
        self.age_upgrades = {}
        self.unit_upgrades = {}
        self.building_upgrades = {}
        self.unit_unlocks = {}
        self.civ_boni = {}

        # Phase 3: nyan objects
        self.combined_sprites = {}
