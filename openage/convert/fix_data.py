# Copyright 2014-2015 the openage authors. See copying.md for legal info.

""" modernizes/patches the gamespec. """


def fix_data(data):
    """
    updates given input with modifications.

    input: empiresdat object, vanilla, fully read.
    output: empiresdat object, fixed.
    """

    ###
    # Terrain fixes
    ###

    # remove terrains with slp_id == -1
    # we'll need them again in the future, with fixed slp ids
    data.terrains = [val for val in data.terrains if val.slp_id >= 0]

    # assign correct blending modes
    # key:   dat file stored mode
    # value: corrected mode
    # resulting values are also priorities!
    #  -> higher => gets selected as mask for two partners
    blendmode_map = {
        # identical modes: [0,1,7,8], [4,6]
        0: 1,  # dirt, grass, palm_desert
        1: 3,  # farms
        2: 2,  # beach
        3: 0,  # water
        4: 1,  # shallows
        5: 4,  # roads
        6: 5,  # ice
        7: 6,  # snow
        8: 4,  # no terrain has it, but the mode exists..
    }
    for terrain in data.terrains:
        terrain.blend_mode = blendmode_map[terrain.blend_mode]

    # set correct terrain ids
    for idx, terrain in enumerate(data.terrains):
        terrain.terrain_id = idx

    return data
