# Copyright 2019-2020 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments

"""
Sprite definition file.
"""

from enum import Enum

from ..data_definition import DataDefinition

FILE_VERSION = '1'


class LayerMode(Enum):
    """
    Possible values for the mode of a layer.
    """
    OFF = 'off'
    ONCE = 'once'
    LOOP = 'loop'


class SpriteMetadata(DataDefinition):
    """
    Collects sprite metadata and can format it
    as a .sprite custom format
    """

    def __init__(self, targetdir, filename):
        super().__init__(targetdir, filename)

        self.scalefactor = 1
        self.image_files = {}
        self.layers = {}
        self.angles = {}
        self.frames = []

    def add_image(self, img_id, filename):
        """
        Add an image and the relative file name.

        :param img_id: Image identifier.
        :type img_id: int
        :param filename: Path to the image file.
        :type filename: str
        """
        self.image_files[img_id] = {
            "image_id": img_id,
            "filename": filename,
        }

    def add_layer(self, layer_id, mode=None, position=None, time_per_frame=None, replay_delay=None):
        """
        Define a layer for the rendered sprite.

        :param layer_id: Layer identifier.
        :type layer_id: int
        :param mode: Animation mode (off, once, loop).
        :type mode: LayerMode
        :param position: Layer position.
        :type position: int
        :param time_per_frame: Time spent on each frame.
        :type time_per_frame: float
        :param replay_delay: Time delay before replaying the animation.
        :type replay_delay: float
        """
        self.layers[layer_id] = {
            "layer_id": layer_id,
            "mode": mode,
            "position": position,
            "time_per_frame": time_per_frame,
            "replay_delay": replay_delay,
        }

    def add_angle(self, degree, mirror_from=None):
        """
        Specifies an angle that frames can get assigned to.

        :param degree: Angle identifier expressed in degrees.
        :type degree: int
        :param mirror_from: Other angle to copy frames from, if any.
        :type mirror_from: int
        """
        self.angles[degree] = {
            "degree": degree,
            "mirror_from": mirror_from,
        }

    def add_frame(self, frame_idx, angle, layer_id, img_id, xpos, ypos,
                  xsize, ysize, xhotspot, yhotspot):
        """
        Add frame with all its spacial information.

        :param frame_idx: Index of the frame in the animation for the specified angle.
        :type frame_idx: int
        :param angle: Angle to which the frame belongs, in degrees.
        :type angle: int
        :param layer_id: ID of the layer to which the frame belongs.
        :type layer_id: int
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
        self.frames.append(
            {
                "frame_idx": frame_idx,
                "angle": angle,
                "layer_id": layer_id,
                "img_id": img_id,
                "xpos": xpos,
                "ypos": ypos,
                "xsize": xsize,
                "ysize": ysize,
                "xhotspot": xhotspot,
                "yhotspot": yhotspot,
            }
        )

    def set_scalefactor(self, factor):
        """
        Set the scale factor of the animation.

        :param factor: Factor by which sprite images are scaled down at default zoom level.
        :type factor: float
        """
        self.scalefactor = float(factor)

    def dump(self):
        output_str = ""

        # header
        output_str += "# openage sprite definition file\n\n"

        # version
        output_str += f"version {FILE_VERSION}\n\n"

        # scale factor
        output_str += f"scalefactor {self.scalefactor}\n\n"

        # image files
        for image in self.image_files.values():
            output_str += f"imagefile {image['image_id']} {image['filename']}\n\n"

        # layer definitions
        for layer in self.layers.values():
            output_str += f"layer {layer['layer_id']}"

            if layer["mode"]:
                output_str += f" mode={layer['mode']}"

            if layer["position"]:
                output_str += f" position={layer['position']}"

            if layer["time_per_frame"]:
                output_str += f" time_per_frame={layer['time_per_frame']}"

            if layer["replay_delay"]:
                output_str += f" replay_delay={layer['replay_delay']}"

            output_str += "\n"

        output_str += "\n"

        # angle mirroring declarations
        for angle in self.angles.values():
            output_str += f"angle {angle['degree']}"

            if angle["mirror_from"]:
                output_str += f" mirror_from={angle['mirror_from']}"

            output_str += '\n'

        output_str += '\n'

        # frame definitions
        for frame in self.frames:
            output_str += f'frame {" ".join(str(param) for param in frame.values())}\n'

        return output_str

    def __repr__(self):
        return f'SpriteMetadata<{self.filename}>'
