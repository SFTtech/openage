from . import dataformat
from .util import NamedObject, dbg, file_open, file_get_path, file_write
import math
from struct import Struct, unpack_from
import os.path

endianness = "< "

class BlendingTile:
    def __init__(self, data, width, height):
        self.data   = data
        self.width  = width
        self.height = height

    def get_picture_data(self):
        import numpy

        tile_rows = list()

        for y, picture_row in enumerate(self.data):
            tile_row_data = list()

            for x, alpha_data in enumerate(picture_row):

                if alpha_data == -1:
                    #draw full transparency
                    alpha = 0
                    val   = 0
                else:
                    if alpha_data == 128:
                        alpha = 255
                        val   = 0
                    else:
                        #original data contains 7bit values only
                        alpha = 128
                        val   = (127 - (alpha_data & 0x7f)) * 2

                tile_row_data.append((val, val, val, alpha))

            tile_rows.append(tile_row_data)

        return numpy.array(tile_rows)


class BlendingMode:
    def __init__(self, idx, data_file, tile_count, header):
        """
        initialize one blending mode,
        consisting of multiple frames for all blending directions

        the bitmasks were used to decide whether this pixel has
        to be used for calculations.

        the alphamask is used to determine the alpha amount for blending.
        """

        #should be 2353 -> number of pixels (single alpha byte values)
        self.pxcount = header[0]
        #tile_flags = header[1:]  #TODO what do they do?

        dbg("tiles in blending mode %d have %d pixels" % (idx, self.pxcount), 2)

        #as we draw in isometric tile format, this is the row count
        row_count = int(math.sqrt(self.pxcount)) + 1  #should be 49

        #alpha_masks_raw is an array of bytes that will draw 32 images,
        #which are bit masks.
        #
        #one of these masks also has 2353 pixels
        #the storage of the bit masks is 4*tilesize, here's why:
        #
        #4 * 8bit * 2353 pixels = 75296 bitpixels
        #==> 75296/(32 images) = 2353 bitpixel/image
        #
        #this means if we interprete the 75296 bitpixels as 32 images,
        #each of these images gets 2353 bit as data.
        #TODO: why 32 images? isn't that depending on tile_count?

        bitmask_buf_size = self.pxcount * 4
        dbg("reading 1bit masks -> %d bytes" % (bitmask_buf_size), 2)
        alpha_masks_buf = data_file.read(bitmask_buf_size)
        alpha_masks_raw = unpack_from("%dB" % (bitmask_buf_size), alpha_masks_buf)

        #list of alpha-mask tiles
        self.alphamasks = list()

        dbg("reading 8bit masks for %d tiles -> %d bytes" % (tile_count, self.pxcount * tile_count), 2)

        #draw mask tiles for this blending mode
        for j in range(tile_count):
            tile_buf = data_file.read(self.pxcount)
            pixels   = unpack_from("%dB" % self.pxcount, tile_buf)
            self.alphamasks.append(self.get_tile_from_data(row_count, pixels))

        bitvalues = list()
        for i in alpha_masks_raw:
            for b_id in range(7, -1, -1):
                #bitmask from 0b00000001 to 0b10000000
                bit_mask = 2 ** b_id
                bit      = i & bit_mask
                bitvalues.append(bit)

        #list of bit-mask tiles
        self.bitmasks = list()

        #TODO: is 32 really hardcoded?
        for i in range(32):
            data_begin =  i    * self.pxcount
            data_end   = (i+1) * self.pxcount
            pixels = bitvalues[ data_begin : data_end ]

            self.bitmasks.append(self.get_tile_from_data(row_count, pixels))

    def get_tile_from_data(self, row_count, data):
        """
        get the data pixels, interprete them in isometric tile format

          ....*....
          ..*****..
          *********
          ..*****..
          ....*....  like this, only bigger..

        we end up drawing the rhombus with 49 rows.
        the space indicated by . is added by the function.
        """

        half_row_count = row_count // 2
        tile_size      = len(data)

        read_so_far = 0
        max_width   = 0
        tilerows    = list()

        for y in range(row_count):
            if y < half_row_count:
                #upper half of the tile
                #row i+1 has 4 more pixels than row i
                # +1 for the middle one
                read_values = 1 + (4 * y)
            else:
                #lower half of tile
                read_values = ((row_count * 2) - 1) - (4 * (y - half_row_count))

            if read_values > (tile_size - read_so_far):
                raise Exception("reading more bytes than tile has left")
            elif read_values < 0:
                raise Exception("wanted to read a negative amount of bytes: %d" % read_values)

            #grab the pixels out of the big list
            pixels = list(data[ read_so_far : read_so_far + read_values ])

            #how many empty pixels on the left before the real data begins
            space_count = row_count - 1 - (read_values//2)

            #insert padding to the left and right (-1 for fully transparent)
            padding = ([-1] * space_count)
            pixels = padding + pixels + padding

            if len(pixels) > max_width:
                max_width = len(pixels)

            read_so_far += read_values
            tilerows.append(pixels)

        if read_so_far != tile_size:
            raise Exception("got leftover bytes: %d" % (tile_size-read_so_far))

        return BlendingTile(tilerows, max_width, row_count)


class Blendomatic(dataformat.Exportable):

    name_struct        = "blending_mode"
    name_struct_file   = "blending_mode"
    struct_description = "describes one blending mode, a blending transition shape between two different terrain types."
    data_format = (
        (True, "blend_mode", "int32_t"),
    )

    #struct blendomatic_header {
    # unsigned int nr_blending_modes;
    # unsigned int nr_tiles;
    #};
    blendomatic_header = Struct(endianness + "I I")


    def __init__(self, fname):
        self.fname = fname
        dbg("reading blendomatic data from %s" % fname, 1, push="blendomatic")

        fname = file_get_path(fname, write=False)
        f = file_open(fname, binary=True, write=False)

        buf = f.read(Blendomatic.blendomatic_header.size)
        self.header = Blendomatic.blendomatic_header.unpack_from(buf)

        blending_mode_count, tile_count = self.header

        dbg("%d blending modes, each %d tiles" % (blending_mode_count, tile_count), 2)

        blending_mode = Struct(endianness + "I %dB" % (tile_count))

        self.blending_modes = list()

        for i in range(blending_mode_count):
            header_data  = f.read(blending_mode.size)
            bmode_header = blending_mode.unpack_from(header_data)

            new_mode = BlendingMode(i, f, tile_count, bmode_header)

            self.blending_modes.append(new_mode)

        f.close()
        dbg(pop="blendomatic")

    def get_textures(self):
        """
        generate a list of textures.

        one atlas per blending mode is generated,
        each atlas contains all blending masks merged on one texture
        """

        from .texture import Texture
        return [ Texture(blending_mode) for blending_mode in self.blending_modes ]

    def dump(self, filename):
        data = [ {"blend_mode": idx} for idx, _ in enumerate(self.blending_modes) ]
        return [ dataformat.DataDefinition(self, data, filename) ]

    @classmethod
    def structs(cls):
        return [ dataformat.StructDefinition(cls) ]

    def save(self, output_folder, save_format):
        for idx, texture in enumerate(self.get_textures()):
            fname = os.path.join(output_folder, "mode%02d" % idx)
            dbg("saving blending mode%02d texture -> %s.png" % (idx, fname), 1)
            texture.save(fname, save_format)

        dbg("blending masks exported successfully!", 1)

    def __str__(self):
        return str(self.blending_modes)
