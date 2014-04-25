from struct import Struct, unpack_from
from util import dbg

from .empiresdat import endianness


class PlayerColorData:
    def read(self, raw, offset):
        #uint16_t player_color_count;
        header_struct = Struct(endianness + "h")

        self.player_color_count, = header_struct.unpack_from(raw, offset)
        offset += header_struct.size

        self.player_color = list()
        for i in range(self.player_color_count):
            t = PlayerColor()
            offset = t.read(raw, offset)
            self.player_color.append(t)

        return offset


class PlayerColor:
    def read(self, raw, offset):
        #int32_t uid;
        #int32_t palette;
        #int32_t color;
        #int32_t unknown;
        #int32_t unknown;
        #int32_t minimap_color;
        #int32_t unknown;
        #int32_t unknown;
        #int32_t unknown;
        player_color_struct = Struct(endianness + "9i")

        pc = player_color_struct.unpack_from(raw, offset)
        offset += player_color_struct.size

        self.uid           = pc[0]
        self.palette       = pc[1]
        self.color         = pc[2]
        #self. = pc[0]
        #self. = pc[0]
        self.minimap_color = pc[5]
        #self. = pc[0]
        #self. = pc[0]

        return offset
