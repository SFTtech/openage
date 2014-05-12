import dataformat
from struct import Struct, unpack_from
from util import dbg, zstr

from .empiresdat import endianness
from gamedata import unit


class Civ(dataformat.Exportable):
    name_struct        = "civilisation"
    name_struct_file   = name_struct
    struct_description = "describes one a civilisation."

    data_format = (
        (dataformat.READ, "enabled", "int8_t"),
        (dataformat.READ_EXPORT, "name", "char[20]"),
        (dataformat.READ, "ressources_count", "uint16_t"),
        (dataformat.READ_EXPORT, "tech_tree_id",  "int16_t"),
        (dataformat.READ_EXPORT, "team_bonus_id", "int16_t"),
        (dataformat.READ, "ressources", "float[ressources_count]"),
        (dataformat.READ, "graphic_set", "int8_t"),
        (dataformat.READ, "unit_count", "uint16_t"),
        (dataformat.READ, "unit_offsets", "int32_t[unit_count]"),
        (dataformat.READ_EXPORT, "units", dataformat.SubdataMember(
            ref_type=unit.Unit,
            ref_to="name",
            length="unit_count",
            offset_to="unit_offsets",
        )),
    )

    def read(self, raw, offset):
        civ_header0_struct = Struct(endianness + "b 20s H 2h")

        pc = civ_header0_struct.unpack_from(raw, offset)
        offset += civ_header0_struct.size

        self.one             = pc[0]
        self.name            = zstr(pc[1])
        self.ressource_count = pc[2]
        self.tech_tree_id    = pc[3]
        self.team_bonus_id   = pc[4]

        civ_ressources_struct = Struct(endianness + "%df" % self.ressource_count)

        pc = civ_ressources_struct.unpack_from(raw, offset)
        offset += civ_ressources_struct.size

        self.ressources = pc

        civ_header1_struct = Struct(endianness + "b H")

        pc = civ_header1_struct.unpack_from(raw, offset)
        offset += civ_header1_struct.size

        self.graphic_set = pc[0]
        self.unit_count  = pc[1]

        civ_header2_struct = Struct(endianness + "%di" % self.unit_count)
        pc = civ_header2_struct.unpack_from(raw, offset)
        offset += civ_header2_struct.size

        self.unit_offsets = pc

        self.units = list()
        dbg("unit count = %d" % self.unit_count, 2)
        for i in range(self.unit_count):
            dbg("%%%%%%%%%%%%%%%%%%%%%%%%%%%% UNIT entry %d" % i, 2)

            uo = self.unit_offsets[i]
            dbg("unit offset = %d" % uo, 3)
            if uo == 0:
                dbg("skipping UNIT entry %d" % i, 2)
                continue

            t = unit.Unit()
            offset = t.read(raw, offset)
            self.units.append(t)

        return offset


class CivData(dataformat.Exportable):
    name_struct        = "civilisation_data"
    name_struct_file   = "gamedata"
    struct_description = "civilisation list."

    data_format = (
        (dataformat.READ_EXPORT, "civs", dataformat.SubdataMember(ref_type=Civ)),
    )

    def read(self, raw, offset):
        #uint16_t civ_count;
        header_struct = Struct(endianness + "H")

        header = header_struct.unpack_from(raw, offset)
        offset += header_struct.size
        self.civ_count, = header

        self.civs = list()
        for i in range(self.civ_count):
            dbg("############################ civilisation entry %d" % i, 2)

            t = Civ()
            offset = t.read(raw, offset)
            self.civs.append(t)

        return offset
