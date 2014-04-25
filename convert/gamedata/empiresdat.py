#!/usr/bin/env python3

#define little endian byte order for the dat file
endianness = '< '

from gamedata import civ
from gamedata import graphic
from gamedata import playercolor
from gamedata import research
from gamedata import sound
from gamedata import tech
from gamedata import terrain
from gamedata import unit

import dataformat
from struct import Struct, unpack_from
from util import dbg, file_get_path, file_open, file_write, zstr
import zlib


# this file can parse and represent the empires2_x1_p1.dat file,
# and also its predecessors, empires2.dat and empires2_x1.dat.
#
# these files contain all the information needed for running the game.
# all units, buildings, terrains, whatever are defined in this dat file.
#
# documentation for this can be found in `doc/gamedata`
# the binary structure, which the dat file has, is in `doc/gamedata.struct`


class EmpiresDat:
	"""class for fighting and beating the compressed empires2*.dat"""

	def __init__(self, fname):
		self.fname = fname
		dbg("reading empires2*.dat from %s..." % fname, 1)

		fname = file_get_path(fname, write = False)
		f = file_open(fname, binary = True, write = False)

		dbg("decompressing data from %s" % fname, 1)

		compressed_data = f.read()
		#decompress content with zlib (note the magic -15)
		#-15: - -> there is no header, 15 is the max windowsize
		self.content = zlib.decompress(compressed_data, -15)
		f.close()

		compressed_size   = len(compressed_data)
		decompressed_size = len(self.content)

		#compressed data no longer needed
		del compressed_data

		dbg("length of compressed data: %d = %d kB" % (compressed_size, compressed_size/1024), 1)
		dbg("length of decompressed data: %d = %d kB" % (decompressed_size, decompressed_size/1024), 1)

		#this variable will store the offset in the raw dat file.
		offset = 0
		offset = self.read(self.content, offset)

		finish_percent = 100*(offset/decompressed_size)
		dbg("finished reading empires*.dat at %d of %d bytes (%f%%)." % (offset, decompressed_size, finish_percent), 1)


	def raw_dump(self, filename):
		"""
		save the dat file in uncompressed format.
		"""

		rawfile_writepath = file_get_path(filename, write=True)
		dbg("saving uncompressed %s file to %s" % (self.fname, rawfile_writepath), 1)
		file_write(rawfile_writepath, self.content)

	def read(self, raw, offset):

		#char versionstr[8];
		header_struct = Struct(endianness + "8s")
		header = header_struct.unpack_from(raw, offset)
		offset += header_struct.size

		self.version = zstr(header[0])

		dbg("dat version: %s" % (self.version), 1)

		self.terrain_header = terrain.TerrainHeaderData()
		offset = self.terrain_header.read(raw, offset)

		self.color = playercolor.PlayerColorData()
		offset = self.color.read(raw, offset)

		self.sound = sound.SoundData()
		offset = self.sound.read(raw, offset)

		self.graphic = graphic.GraphicData()
		offset = self.graphic.read(raw, offset)

		self.terrain = terrain.TerrainData(self.terrain_header.terrain_count)
		offset = self.terrain.read(raw, offset)

		#unknown shiat
		tmp_struct = Struct(endianness + "438c")
		offset += tmp_struct.size

		self.terrain_borders = terrain.TerrainBorderData()
		offset = self.terrain_borders.read(raw, offset)

		self.tech = tech.TechData()
		offset = self.tech.read(raw, offset)

		self.unit = unit.UnitHeaderData()
		offset = self.unit.read(raw, offset)

		self.civ = civ.CivData()
		offset = self.civ.read(raw, offset)

		self.research = research.ResearchData()
		offset = self.research.read(raw, offset)

		#unknown shiat again
		tmp_struct = Struct(endianness + "7i")
		offset += tmp_struct.size

		self.tech = tech.TechtreeData()
		offset = self.tech.read(raw, offset)

		return offset

	def dump(self, what):
		if type(what) != list:
			what = [what]

		ret = list()
		for entry in what:
			member_dump = getattr(self, entry).dump(what)
			ret += member_dump

		return ret

	def structs(what):
		"""
		function for dumping struct data without having to read a dat file.

		note that 'self' is missing from the parameter list.
		"""

		if type(what) != list:
			what = [what]

		ret = list()
		for entry in what:
			if "terrain" == entry:
				target_class = terrain.Terrain
			else:
				raise Exception("unknown struct dump requested: %s" % what)

			ret += [ dataformat.StructDefinition(target_class) ]

		return ret


	def __str__(self):
		ret = "[age2x1p1]\n"
		ret += "TODO: a nice full representation"
		return ret

	def __repr__(self):
		ret = "TODO: a nice short representation"
		return ret
