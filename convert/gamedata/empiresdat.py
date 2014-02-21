import zlib

from struct import Struct, unpack_from
from util import dbg, file_get_path, file_open, zstr

#define little endian byte order for the dat file
endianness = '< '


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


		from util import file_write
		rawfile_writepath = file_get_path('raw/empires2x1p1.raw', write=True)
		print("saving uncompressed %s file to %s" % (self.fname, rawfile_writepath))
		file_write(rawfile_writepath, self.content)

		offset = 0
		offset = self.read(self.content, offset)

		finish_percent = 100*(offset/decompressed_size)
		dbg("finished reading empires*.dat at %d of %d bytes (%f%%)." % (offset, decompressed_size, finish_percent), 1)


	def read(self, raw, offset):

		#char versionstr[8];
		header_struct = Struct(endianness + "8s")
		header = header_struct.unpack_from(raw, offset)
		offset += header_struct.size

		self.version = zstr(header[0])

		dbg("dat version: %s" % (self.version), 1)

		from gamedata import terrain
		t = terrain.TerrainHeaderData()
		offset = t.read(raw, offset)
		self.terrain = t

		from gamedata import playercolor
		t = playercolor.PlayerColorData()
		offset = t.read(raw, offset)
		self.color = t

		from gamedata import sound
		t = sound.SoundData()
		offset = t.read(raw, offset)
		self.sound = t

		from gamedata import graphic
		t = graphic.GraphicData()
		offset = t.read(raw, offset)
		self.graphic = t

		from gamedata import terrain
		t = terrain.TerrainData(self.terrain.terrain_count)
		offset = t.read(raw, offset)
		self.terrain.data = t

		#unknown shiat
		tmp_struct = Struct(endianness + "438c")
		offset += tmp_struct.size

		from gamedata import terrain
		t = terrain.TerrainBorderData()
		offset = t.read(raw, offset)
		self.terrain.borders = t

		from gamedata import tech
		t = tech.TechData()
		offset = t.read(raw, offset)
		self.tech = t

		from gamedata import unit
		t = unit.UnitHeaderData()
		offset = t.read(raw, offset)
		self.unit = t

		from gamedata import civ
		t = civ.CivData()
		offset = t.read(raw, offset)
		self.civ = t

		from gamedata import research
		t = research.ResearchData()
		offset = t.read(raw, offset)
		self.research = t

		#unknown shiat again
		tmp_struct = Struct(endianness + "7i")
		offset += tmp_struct.size

		from gamedata import tech
		t = tech.TechtreeData()
		offset = t.read(raw, offset)
		self.tech = t

		return offset


	def dump(self, what):
		return getattr(self, what).dump()


	def __str__(self):
		ret = "[age2x1p1]\n"
		ret += "TODO: a nice full representation"
		return ret

	def __repr__(self):
		ret = "TODO: a nice short representation"
		return ret
