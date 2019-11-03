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

    def get_type(self):
        """
        Returns the effect's type.
        """
        return self.get_member("type_id").get_value()


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

        # Sanitized bundles should not contain 'garbage' effects, e.g.
        #     - effects that do nothing
        #     - effects without a type        #
        # Processors should set this to True, once the bundle is sanitized.
        self.sanitized = False

        self.data = full_data_set
        self.data.genie_effect_bundles.update({self.get_id(): self})

    def get_effects(self, effect_type=None):
        """
        Returns the effects in the bundle, optionally only effects with a specific
        type.

        :param effect_type: Type that the effects should have.
        :type effect_type: int, optional
        :returns: List of matching effects.
        :rtype: list
        """
        if effect_type:
            matching_effects = []
            for _, effect in self.effects.items():
                if effect.get_type() == effect_type:
                    matching_effects.append(effect)

            return matching_effects

        else:
            return self.effects

    def is_sanitized(self):
        """
        Returns whether the effect bundle has been sanitized.
        """
        return self.sanitized
