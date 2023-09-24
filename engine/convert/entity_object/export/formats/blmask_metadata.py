# Copyright 2021-2022 the openage authors. See copying.md for legal info.
#
# pylint: disable=too-many-arguments

"""
Blendmask definition file.
"""
from __future__ import annotations
import typing

from ..data_definition import DataDefinition

FORMAT_VERSION = '1'


class BlendmaskMetadata(DataDefinition):
    """
    Collects blendmask metadata and can format it
    as a .blmask custom format
    """

    def __init__(self, targetdir: str, filename: str):
        super().__init__(targetdir, filename)

        self.image_files: dict[int, dict[str, typing.Any]] = {}
        self.scalefactor = 1.0
        self.masks: dict[int, dict[str, int]] = {}

    def add_image(self, img_id: int, filename: str) -> None:
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

    def add_mask(
        self,
        directions: int,
        img_id: int,
        xpos: int,
        ypos: int,
        xsize: int,
        ysize: int
    ) -> None:
        """
        Add a mask for directions.

        :param directions: Directions bitfield value.
        :type directions: int
        :param img_id: ID of the image used by this mask.
        :type img_id: int
        :param xpos: X position of the mask on the image canvas.
        :type xpos: int
        :param ypos: Y position of the mask on the image canvas.
        :type ypos: int
        :param xsize: Width of the mask.
        :type xsize: int
        :param ysize: Height of the mask.
        :type ysize: int
        """
        self.masks[directions] = {
            "directions": directions,
            "img_id": img_id,
            "xpos": xpos,
            "ypos": ypos,
            "xsize": xsize,
            "ysize": ysize,
        }

    def set_scalefactor(self, factor: typing.Union[int, float]) -> None:
        """
        Set the scale factor of the animation.

        :param factor: Factor by which sprite images are scaled down at default zoom level.
        :type factor: float
        """
        self.scalefactor = float(factor)

    def dump(self) -> str:
        output_str = ""

        # header
        output_str += "# openage blendmask definition file\n\n"

        # version
        output_str += f"version {FORMAT_VERSION}\n\n"

        # image files
        for image in self.image_files.values():
            output_str += f"imagefile {image['image_id']} {image['filename']}\n"

        output_str += "\n"

        # scale factor
        output_str += f"scalefactor {self.scalefactor}\n\n"

        # mask definitions
        for mask in self.masks.values():
            output_str += f'mask {" ".join(str(param) for param in mask.values())}\n'

        return output_str

    def __repr__(self):
        return f'BlendmaskMetadata<{self.filename}>'
