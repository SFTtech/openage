# Copyright 2019-2019 the openage authors. See copying.md for legal info.

"""
Loads the API into the converter.

TODO: Implement a parser instead of hardcoded 
object creation.
"""

from ...nyan.nyan_structs import NyanObject, NyanMember


def load_api():
    """
    Returns a dict with the API object's fqon as keys
    and the API objects as values.
    """
    api_objects = dict()

    # Object creation

    # engine.root
    # engine.root.Entity
    nyan_object = NyanObject("Entity")
    fqon = "engine.root.Entity"
    nyan_object.set_fqon(fqon)

    api_objects.update({fqon: nyan_object})

    # engine.aux
    # engine.aux.accuracy.Accuracy
    parents = [api_objects["engine.root.Entity"]]
    nyan_object = NyanObject("Accuracy", parents)
    fqon = "engine.aux.accuracy.Accuracy"
    nyan_object.set_fqon(fqon)

    api_objects.update({fqon: nyan_object})
