# Copyright 2016-2017 the openage authors. See copying.md for legal info.

""" Cutting some user interface assets into subtextures """
# REFA: Whole file -> service

from ..texture import TextureImage

from .hardcoded import (is_ingame_hud_background,
                        TOP_STRIP_PATTERN_CORNERS,
                        TOP_STRIP_PATTERN_SEARCH_AREA_CORNERS,
                        MID_STRIP_PATTERN_CORNERS,
                        MID_STRIP_PATTERN_SEARCH_AREA_CORNERS,
                        KNOWN_SUBTEX_CORNER_COORDS)
from .visgrep import visgrep, crop_array


class InterfaceCutter:
    """
    Cuts interface textures into repeatable parts.
    """

    def __init__(self, idx):
        self.idx = idx

    def cut(self, image):
        """
        Create subtextures by searching for patterns at hardcoded positions.
        """

        if not isinstance(image, TextureImage):
            raise ValueError("we can only cut TextureImage, "
                             "not '%s'" % type(image))

        if is_ingame_hud_background(self.idx):
            img_data = image.get_data()

            yield self.cut_strip(img_data,
                                 TOP_STRIP_PATTERN_CORNERS,
                                 TOP_STRIP_PATTERN_SEARCH_AREA_CORNERS)

            yield self.cut_strip(img_data,
                                 MID_STRIP_PATTERN_CORNERS,
                                 MID_STRIP_PATTERN_SEARCH_AREA_CORNERS)

            for coords in KNOWN_SUBTEX_CORNER_COORDS:
                yield TextureImage(crop_array(img_data, coords))
        else:
            yield image

    def cut_strip(self, img_array, pattern_corners, search_area_corners):
        """
        Finds a horizontally tilable piece of the strip (ex. the top of the HUD).

        ||----///////////-------------///////////-------------///////////-------------///////////||
                  ^      pattern_corners     ^                    ^  where it is found last  ^
                  ^           this piece is tileable              ^

        so, cut out a subtexture:
                  ///////-------------///////////-------------////
        """

        search_area = crop_array(img_array, search_area_corners)
        pattern = crop_array(img_array, pattern_corners)

        # search for patterns, with "tolerance"
        matches = visgrep(search_area, pattern, 100000)

        if len(matches) < 2:
            raise Exception("visgrep failed to find repeating pattern in id=%d)\n" % self.idx)

        # create the found pattern texture
        return TextureImage(
            crop_array(img_array, (
                pattern_corners[0],
                pattern_corners[1],
                search_area_corners[0] + matches[-1].point[0],
                pattern_corners[3]
            ))
        )
