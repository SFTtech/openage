# Copyright 2019-2019 the openage authors. See copying.md for legal info.

from openage.convert.dataformat.converter_object import ConverterObject,\
    ConverterObjectGroup


class GenieEffectObject(ConverterObject):
    """
    Single effect contained in GenieEffectBundle.
    """

    def __init__(self, effect_id, bundle_id, full_data_set):
        """
        Creates a new Genie civilization object.

        :param effect_id: The index of the effect in the .dat file's effect
                          bundle. (the index is referenced as tech_effect_id by techs)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(effect_id)

        self.bundle_id = bundle_id

        self.data = full_data_set
        self.data.genie_effect_bundles.update({self.get_id(): self})


class GenieEffectBundle(ConverterObject):
    """
    A set of effects of a tech.
    """

    def __init__(self, bundle_id, full_data_set):
        """
        Creates a new Genie civilization object.

        :param bundle_id: The index of the effect in the .dat file's effect
                          block. (the index is referenced as tech_effect_id by techs)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        """

        super().__init__(bundle_id)

        self.effects = []

        self.data = full_data_set
        self.data.genie_effect_bundles.update({self.get_id(): self})
