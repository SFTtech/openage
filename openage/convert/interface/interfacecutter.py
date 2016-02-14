# Copyright 2016-2016 the openage authors. See copying.md for legal info.

""" Cutting some user interface assets into subtextures """

from ..texture import TextureImage

from .interfacehardcoded import (is_ingame_hud_background,
                                 TOP_STRIP_PATTERN_CORNERS,
                                 KNOWN_SUBTEX_CORNER_COORDS)

from .visgrep import visgrep


class InterfaceCutter(object):
    """
    Callable object that cuts interface textures.
    """
    # pylint: disable=too-few-public-methods

    def __init__(self, filename):
        self.filename = filename

    def __call__(self, image):
        if is_ingame_hud_background(self.filename):
            pil_image = image.get_pil_image()

            yield self.__cut_top_strip(pil_image)

            for coords in KNOWN_SUBTEX_CORNER_COORDS:
                yield TextureImage(pil_image.crop(coords))
        else:
            yield image

    def __cut_top_strip(self, pil_image):
        """
        Finds a horisontally tilable piece of the strip that's at the top of the HUD.

        ||----///////////-------------///////////-------------///////////-------------///////////||
                  ^TOP_STRIP_PATTERN_CORNERS ^                    ^  where it is found last  ^
                  ^           this piece is tileable              ^

        so, cut out a subtexture:
                  ///////-------------///////////-------------////
        """

        pattern = pil_image.crop(TOP_STRIP_PATTERN_CORNERS)
        matches = visgrep(pil_image, pattern, 100000, TOP_STRIP_PATTERN_CORNERS[3])

        if len(matches) < 2 or matches[-1].point[1] != TOP_STRIP_PATTERN_CORNERS[1]:
            raise Exception("visgrep failed to find repeating pattern (%s)\n" % self.filename)

        return TextureImage(pil_image.crop((
            TOP_STRIP_PATTERN_CORNERS[0],
            TOP_STRIP_PATTERN_CORNERS[1],
            matches[-1].point[0],
            TOP_STRIP_PATTERN_CORNERS[3]
        )))
