# Copyright 2019-2019 the openage authors. See copying.md for legal info.

from ...dataformat.converter_object import ConverterObject


class GenieEffectObject(ConverterObject):
    """
    Single effect contained in GenieEffectBundle.
    """

    def __init__(self, effect_id, bundle_id, full_data_set, members=None):
        """
        Creates a new Genie effect object.

        :param effect_id: The index of the effect in the .dat file's effect
        :param bundle_id: The index of the effect bundle that the effect belongs to.
                          (the index is referenced as tech_effect_id by techs)
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :param members: An already existing member dict.
        """

        super().__init__(effect_id, members=members)

        self.bundle_id = bundle_id
        self.data = full_data_set


class GenieEffectBundle(ConverterObject):
    """
    A set of effects of a tech.
    """

    def __init__(self, bundle_id, effects, full_data_set, members=None):
        """
        Creates a new Genie effect bundle.

        :param bundle_id: The index of the effect in the .dat file's effect
                          block. (the index is referenced as tech_effect_id by techs)
        :param effects: Effects of the bundle as list of GenieEffectObject.
        :param full_data_set: GenieObjectContainer instance that
                              contains all relevant data for the conversion
                              process.
        :param members: An already existing member dict.
        """

        super().__init__(bundle_id, members=members)

        self.effects = effects

        self.data = full_data_set
        self.data.genie_effect_bundles.update({self.get_id(): self})
