# Copyright 2019-2020 the openage authors. See copying.md for legal info.

"""
Sprite definition file.
"""

from enum import Enum

from ..data_definition import DataDefinition

FILE_VERSION = '0.1.0'


class LayerMode(Enum):
    """
    Possible values for the mode of a layer.
    """
    OFF = 'off'
    ONCE = 'once'
    LOOP = 'loop'


class LayerPosition(Enum):
    """
    Possible values for the position of a layer.
    """
    DEFAULT = 'default'
    ROOF = 'roof'
    SHADOW = 'shadow'


class SpriteMetadata(DataDefinition):
    """
    Collects sprite metadata and can format it
    as a .sprite custom format
    """
    def __init__(self, targetdir, filename):
        super().__init__(targetdir, filename)

        self.image_files = {}
        self.layers = {}
        self.angles = {}
        self.frames = []

    def add_image(self, img_id, filename):
        """
        Add an image and the relative file name.

        :param img_id: Image identifier.
        :type img_id: int
        :param filename: Name of the image file, with extension.
        :type filename: str
        """
        self.image_files[img_id] = filename

    def add_layer(self, layer_id, mode, position, time_per_frame=None, replay_delay=None):
        """
        Add a layer and its relative parameters.

        :param layer_id: Layer identifier.
        :type layer_id: int
        :param mode: Animation mode (off, once, loop).
        :type mode: LayerMode
        :param position: Layer position (default, roof, shadow).
        :type position: int, LayerPosition
        :param time_per_frame: Time spent on each frame.
        :type time_per_frame: float
        :param replay_delay: Time delay before replaying the animation.
        :type replay_delay: float
        """
        self.layers[layer_id] = (mode, position, time_per_frame, replay_delay)

    def add_angle(self, degree, mirror_from=None):
        """
        Add an angle definition and its mirror source, if any.

        :param degree: Angle identifier expressed in degrees.
        :type degree: int
        :param mirror_from: Other angle to copy frames from, if any.
        :type mirror_from: int
        """
        # when not None, it will look for the mirrored angle
        self.angles[degree] = mirror_from

    def add_frame(self, layer_id, angle, img_id, xpos, ypos, xsize, ysize, xhotspot, yhotspot):
        """
        Add frame with all its spacial information.

        :param layer_id: ID of the layer to which the frame belongs.
        :type layer_id: int
        :param angle: Angle to which the frame belongs, in degrees.
        :type angle: int
        :param img_id: ID of the image used by this frame.
        :type img_id: int
        :param xpos: X position of the frame on the image canvas.
        :type xpos: int
        :param ypos: Y position of the frame on the image canvas.
        :type ypos: int
        :param xsize: Width of the frame.
        :type xsize: int
        :param ysize: Height of the frame.
        :type ysize: int
        :param xhotspot: X position of the hotspot of the frame.
        :type xhotspot: int
        :param yhotspot: Y position of the hotspot of the frame.
        :type yhotspot: int
        """
        self.frames.append((layer_id, angle, img_id, xpos, ypos, xsize, ysize, xhotspot, yhotspot))

    def dump(self):
        out = ''

        # header
        out += f'# SPRITE DEFINITION FILE version {FILE_VERSION}\n'

        # image files
        for img_id, file in self.image_files.items():
            out += f'imagefile {img_id} {file}\n'

        # layer definitions
        for layer_id, params in self.layers.items():
            if isinstance(params[1], int):
                position = params[1]
            else:
                position = params[1].value
            out += f'layer {layer_id} mode={params[0].value} position={position}'
            if params[2] is not None:
                out += f' time_per_frame={params[2]}'
            if params[3] is not None:
                out += f' replay_delay={params[3]}'
            out += '\n'

        # angle mirroring declarations
        for degree, mirror_from in self.angles.items():
            out += f'angle {degree}'
            if mirror_from is not None:
                out += f' mirror_from={mirror_from}'
            out += '\n'

        # frame definitions
        for frame in self.frames:
            out += f'frame {" ".join(frame)}\n'

        return out

    def __repr__(self):
        return f'SpriteMetadata<{self.filename}>'
