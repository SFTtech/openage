# Copyright 2019-2023 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments

"""
Terrain definition file.
"""
from __future__ import annotations
import typing


from enum import Enum

from ..data_definition import DataDefinition

FORMAT_VERSION = '2'


class LayerMode(Enum):
    """
    Possible values for the mode of a layer.
    """
    OFF = 'off'     # layer is not animated
    LOOP = 'loop'   # animation loops indefinitely


class TerrainMetadata(DataDefinition):
    """
    Collects terrain metadata and can format it
    as a .terrain custom format
    """

    def __init__(self, targetdir: str, filename: str):
        super().__init__(targetdir, filename)

        self.texture_files: dict[int, dict[str, typing.Any]] = {}
        self.scalefactor = 1.0
        self.blendtable: dict[str, typing.Any] = None
        self.layers: dict[int, dict[str, typing.Any]] = {}
        self.frames: list[dict[str, int]] = []

    def add_texture(self, texture_id: int, filename: str) -> None:
        """
        Add a texture and the relative file name.

        :param texture_id: Texture identifier.
        :type texture_id: int
        :param filename: Path to the image file.
        :type filename: str
        """
        self.texture_files[texture_id] = {
            "texture_id": texture_id,
            "filename": filename,
        }

    def add_layer(
        self,
        layer_id: int,
        mode: LayerMode = None,
        position: int = None,
        time_per_frame: float = None,
        replay_delay: float = None
    ) -> None:
        """
        Define a layer for the rendered texture.

        :param layer_id: Layer identifier.
        :type layer_id: int
        :param mode: Animation mode (off, loop).
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

    def add_frame(
        self,
        frame_idx: int,
        layer_id: int,
        texture_id: int,
        subtex_id: int,
        priority=None,
        blend_mode=None
    ) -> None:
        """
        Add frame with all its spacial information.

        :param frame_idx: Index of the frame in the animation.
        :type frame_idx: int
        :param layer_id: ID of the layer to which the frame belongs.
        :type layer_id: int
        :param texture_id: ID of the texture used by this frame.
        :type texture_id: int
        :param subtex_id: ID of the subtexture from the texture used by this frame.
        :type subtex_id: int
        :param priority: Priority for blending.
        :type priority: int
        :param blend_mode: Used for looking up the blending pattern index in the blending table.
        :type blend_mode: int
        """
        self.frames.append(
            {
                "frame_idx": frame_idx,
                "layer_id": layer_id,
                "texture_id": texture_id,
                "subtex_id": subtex_id,
                "priority": priority,
                "blend_mode": blend_mode,
            }
        )

    def set_blendtable(self, table_id: int, filename: str) -> None:
        """
        Set the blendtable and the relative filename.

        :param img_id: Table identifier.
        :type img_id: int
        :param filename: Path to the blendtable file.
        :type filename: str
        """
        self.blendtable = {
            "table_id": table_id,
            "filename": filename,
        }

    def set_scalefactor(self, factor: typing.Union[int, float]) -> None:
        """
        Set the scale factor of the texture.

        :param factor: Factor by which sprite images are scaled down at default zoom level.
        :type factor: float
        """
        self.scalefactor = float(factor)

    def dump(self) -> str:
        output_str = ""

        # header
        output_str += "# openage terrain definition file\n\n"

        # version
        output_str += f"version {FORMAT_VERSION}\n\n"

        # texture files
        for texture in self.texture_files.values():
            output_str += f"texture {texture['texture_id']} \"{texture['filename']}\"\n"

        output_str += "\n"

        # blendtable reference
        if self.blendtable:
            output_str += (f"blendtable {self.blendtable['table_id']} "
                           "{self.blendtable['filename']}\n\n")

        # scale factor
        output_str += f"scalefactor {self.scalefactor}\n\n"

        # layer definitions
        for layer in self.layers.values():
            output_str += f"layer {layer['layer_id']}"

            if layer["mode"]:
                output_str += f" mode={layer['mode'].value}"

            if layer["position"]:
                output_str += f" position={layer['position']}"

            if layer["time_per_frame"]:
                output_str += f" time_per_frame={layer['time_per_frame']}"

            if layer["replay_delay"]:
                output_str += f" replay_delay={layer['replay_delay']}"

            output_str += "\n"

        output_str += "\n"

        # frame definitions
        for frame in self.frames:
            frame_attributes = list(frame.values())
            output_str += f'frame {" ".join(str(param) for param in frame_attributes[:4])}'

            if frame["priority"]:
                output_str += f" priority={frame['priority']}"

            if frame["blend_mode"]:
                output_str += f" blend_mode={frame['blend_mode']}"

            output_str += "\n"

        return output_str

    def __repr__(self):
        return f'TerrainMetadata<{self.filename}>'
