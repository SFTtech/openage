import math
import os

from PIL import Image

class NamedObject:
	def __init__(self, name, **kw):
		self.name = name
		self.__dict__.update(kw)

	def __repr__(self):
		return self.name

dbgstack = [(None, 0)]

readpath = "/dev/null"
writepath = "/dev/null"
verbose = 0

def set_verbosity(newlevel):
	global verbose
	verbose = newlevel

def ifdbg(lvl):
	global verbose

	if verbose >= lvl:
		return True
	else:
		return False

def dbg(msg = None, lvl = None, push = None, pop = None, lazymsg = None, end = "\n"):
	global verbose

	if lvl == None:
		#if no level is set, use the level on top of the debug stack
		lvl = dbgstack[-1][1]

	if lazymsg != None:
		if msg != None:
			raise Exception("debug message called with message and lazy message!")


	if verbose >= lvl:
		if lazymsg != None:
			if callable(lazymsg):
				msg = lazymsg()
			else:
				raise Exception("the lazy message must be a callable (lambda)")

		if msg != None:
			print((len(dbgstack) - 1) * "  " + str(msg), end = end)

	if push != None:
		dbgstack.append((push, lvl))

	if pop != None:
		if pop == True:
			if dbgstack.pop()[0] == None:
				raise Exception("stack underflow in debug stack!")
		elif dbgstack.pop()[0] != pop:
			raise Exception(str(pop) + " is not on top of the debug stack")

def mkdirs(path):
	os.makedirs(path, exist_ok = True)

def set_write_dir(dirname):
	global writepath
	writepath = dirname

def set_read_dir(dirname):
	global readpath
	readpath = dirname

def file_get_path(fname, write = False):
	if write:
		basedir = writepath + "/data/age"
	else:
		basedir = readpath

	path = basedir + '/' + fname

	if write:
		#ensure that the directory exists
		mkdirs(os.path.dirname(path))

	return path

def file_open(path, binary = True, write = False):
	if write:
		flags = 'w'
	else:
		flags = 'r'
	if binary:
		flags += 'b'

	return open(path, flags)

#writes data to a file in the destination directory
def file_write(fname, data):
	if type(data) == bytes:
		file_open(fname, binary = True, write = True).write(data)
	elif type(data) == str:
		file_open(fname, binary = False, write = True).write(data)
	elif type(data) == Image.Image:
		data.save(fname)
	else:
		raise Exception("Unknown data type for writing: " + str(type(data)))

#reads data from a file in the source directory
def file_read(fname, datatype = str):
	if datatype == bytes:
		return file_open(fname, binary = True, write = False).read()
	elif datatype == str:
		return file_open(fname, binary = False, write = False).read()
	elif datatype == Image.Image:
		return Image.open(fname).convert('RGBA')
	else:
		raise Exception("Unknown data type for reading: " + str(datatype))


def merge_frames(frames, max_width = None, max_height = None):
	"""
	merge all given frames of this slp to a single image file.

	frames = [(PNG, (width, height), (hotspot_x, hotspot_y)), ... ]
	"""

	#TODO: actually optimize free space on the texture.
	#if you ever wanted to implement a combinatoric optimisation
	#algorithm, go for it, this function just waits for you.
	#https://en.wikipedia.org/wiki/Bin_packing_problem

	#for now, using max values for solving bin packing problem

	#if not predefined, get maximum frame size by checking all frames
	if max_width == None or max_height == None:
		for (_, (w, h), _) in frames:
			if w > max_width:
				max_width = w
			if h > max_height:
				max_height = h


	max_per_row = math.ceil(math.sqrt(len(frames)))
	num_rows    = math.ceil(len(frames) / max_per_row)

	#we leave 1 pixel free in between two sprites
	width  = math.ceil((max_width  + 1) * max_per_row)
	height = math.ceil((max_height + 2) * num_rows)

	dbg("merging %d frames to %dx%d atlas, %d pics per row, %d rows." % (len(frames), width, height, max_per_row, num_rows), 2)

	#create the big atlas image where the small ones will be placed on
	atlas = Image.new('RGBA', (width, height), (0, 0, 0, 0))

	pos_x = 0
	pos_y = 0

	drawn_frames_meta = []
	drawn_current_row = 0

	for (sub_frame, size, hotspot) in frames:
		subtexture = sub_frame.image
		sub_w = subtexture.size[0]
		sub_h = subtexture.size[1]
		box = (pos_x, pos_y, pos_x + sub_w, pos_y + sub_h)

		atlas.paste(subtexture, box) #, mask=subtexture)
		dbg("drew frame %03d on atlas at %dx%d " % (len(drawn_frames_meta), pos_x, pos_y), 3)

		drawn_subtexture_meta = {
			'tx': pos_x,
			'ty': pos_y,
			'tw': sub_w,
			'th': sub_h,
			'hx': hotspot[0],
			'hy': hotspot[1]
		}
		drawn_frames_meta.append(drawn_subtexture_meta)

		drawn_current_row = drawn_current_row + 1

		#place the subtexture with a 1px border
		pos_x = pos_x + max_width + 1

		#see if we have to start a new row now
		if drawn_current_row > max_per_row - 1:
			drawn_current_row = 0
			pos_x = 0
			pos_y = pos_y + max_height + 1

	return atlas, drawn_frames_meta, (width, height)


def generate_meta_text(metadata, header = None):
	#generate texture atlas metadata file

	meta_out = ""

	if header:
		meta_out += "#" + header

	meta_out += "#meta information: subtexid=x,y,w,h,hotspot_x,hotspot_y\n"
	meta_out += "n=%d\n" % (len(metadata))

	for idx, frame_meta in enumerate(metadata):
		tx = frame_meta["tx"]
		ty = frame_meta["ty"]
		tw = frame_meta["tw"]
		th = frame_meta["th"]
		hotspot_x = frame_meta["hx"]
		hotspot_y = frame_meta["hy"]

		meta_out += "%d=" % idx
		meta_out += "%d,%d,%d,%d," % (tx, ty, tw, th)
		meta_out += "%d,%d\n" % (hotspot_x, hotspot_y)

	return meta_out


def zstr(data):
	"""
	returns the utf8 string representation of a byte array.

	terminates on end of string, or when \0 is reached.
	"""

	return data.decode("utf-8").rstrip("\x00")


def check_file(fid):
	if (True):
		#deactivated for now, maybe use again later
		return

	import filelist
	if fid in filelist.avail_files:
		entries = filelist.avail_files[fid]
		for arch, extension in entries:
			dbg("%d.%s in %s" % (fid, extension, arch), 2)
	elif fid in [-1, 0]:
		dbg("***** graphic is %d!!" % fid, 2)
		pass
	else:
		msg = "##### file %d not found" % (fid)
		#raise Exception(msg)
		dbg(msg, 2)


def offset_info(offset, data, msg="", s=None, mode=0):
	ret = "====== @ %d = %#x ======" % (offset, offset)
	ret += " %s " % msg

	#print struct info
	if s != None:
		ret += "== \"" + str(s.format.decode("utf-8")) + "\" =="

	#long info mode
	if mode == 0:
		ret += "\n" + str(data) + "\n"
	elif mode == 1:
		ret += " " + str(data)

	dbg(ret, 3)

