# Copyright 2019-2020 the openage authors. See copying.md for legal info.

"""
Terrain definition file.
"""

from enum import Enum

from ..data_definition import DataDefinition

FILE_VERSION = "0.1.0"


class LayerMode(Enum):
    """
    Possible values for the mode of a layer.
    """
    OFF = 'off'
    ONCE = 'once'
    LOOP = 'loop'


class TerrainMetadata(DataDefinition):
    """
    Collects terrain metadata and can format it
    as a .terrain custom format
    """
    def __init__(self, targetdir, filename):
        super().__init__(targetdir, filename)

        self.image_files = {}
        self.layers = {}
        self.frames = []
        self.blending_masks = {}
        self.blending_priority = None
        self.dots_per_tile = None

    def add_image(self, img_id, filename):
        """
        Add an image and the relative file name.

        :param img_id: Image identifier.
        :type img_id: int
        :param filename: Name of the image file, with extension.
        :type filename: str
        """
        self.image_files[img_id] = filename

    def add_layer(self, layer_id, mode, time_per_frame=None, replay_delay=None):
        """
        Add a layer and its relative parameters.

        :param layer_id: Layer identifier.
        :type layer_id: int
        :param mode: Animation mode (off, once, loop).
        :type mode: LayerMode
        :param time_per_frame: Time spent on each frame.
        :type time_per_frame: float
        :param replay_delay: Time delay before replaying the animation.
        :type replay_delay: float
        """
        self.layers[layer_id] = (mode, time_per_frame, replay_delay)

    def add_frame(self, layer_id, img_id, blend_id, xpos, ypos, xsize, ysize):
        """
        Add frame with all its spacial information.

        :param layer_id: ID of the layer to which the frame belongs.
        :type layer_id: int
        :param img_id: ID of the image used by this frame.
        :type img_id: int
        :param blend_id: ID of the blending mask fror this frame.
        :type blend_id: int
        :param xpos: X position of the frame on the image canvas.
        :type xpos: int
        :param ypos: Y position of the frame on the image canvas.
        :type ypos: int
        :param xsize: Width of the frame.
        :type xsize: int
        :param ysize: Height of the frame.
        :type ysize: int
        """
        self.frames.append((layer_id, img_id, blend_id, xpos, ypos, xsize, ysize))

    def add_blending_mask(self, mask_id, filename):
        """
        Add a blending mask and the relative file name.

        :param mask_id: Mask identifier.
        :type mask_id: int
        :param filename: Name of the blending mask file, with extension.
        :type filename: str
        """
        self.blending_masks[mask_id] = filename

    def set_blending_priority(self, priority):
        """
        Set the blending priority of this terrain.

        :param priority: Priority level.
        :type priority: int
        """
        self.blending_priority = priority

    def set_dots_per_tile(self, dot_amount):
        """
        Set the amount of dots per tile.

        :param dot_amount: Amount of dots per tile.
        :type dot_amount: float
        """
        self.dots_per_tile = dot_amount

    def dump(self):
        out = ''

        # header
        out += f'# TERRAIN DEFINITION FILE version {FILE_VERSION}\n'

        # priority for blending mask
        out += f'blending_priority {self.blending_priority}'

        # blending mask files
        for mask_id, file in self.blending_masks.items():
            out += f'blending_mask {mask_id} {file}'

        # dots per tile
        out += f'dots_per_tile {self.dots_per_tile}'

        # image files
        for img_id, file in self.image_files.items():
            out += f'imagefile {img_id} {file}\n'

        # layer definitions
        for layer_id, params in self.layers.items():
            out += f'layer {layer_id} mode={params[0].value}'
            if params[1] is not None:
                out += f' time_per_frame={params[1]}'
            if params[2] is not None:
                out += f' replay_delay={params[2]}'
            out += '\n'

        # frame definitions
        for frame in self.frames:
            out += f'frame {" ".join(frame)}\n'

        return out

    def __repr__(self):
        return f'TerrainMetadata<{self.filename}>'
