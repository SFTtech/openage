import os
from PIL import Image

class NamedObject:
	def __init__(self, name, **kw):
		self.name = name
		self.__dict__.update(kw)

	def __repr__(self):
		return self.name

dbgstack = [[None, 0]]

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

		if callable(lazymsg):
			msg = lazymsg()
		else:
			raise Exception("the lazy message must be a callable (lambda)")

	if verbose >= lvl and msg != None:
		#msg may be a lambda for lazy dbg message generation:
		print((len(dbgstack) - 1) * "  " + str(msg), end = end)

	if push != None:
		dbgstack.append([push, lvl])

	if pop != None:
		if pop == True:
			if dbgstack.pop()[0] == None:
				raise Exception("stack underflow in debug stack!")
		elif dbgstack.pop()[0] != pop:
			raise Exception(str(pop) + " is not on top of the debug stack")

def mkdirs(path):
	os.makedirs(path, exist_ok = True)

def set_dir(dirname, is_writedir):
	global writepath, readpath
	if is_writedir:
		writepath = dirname
	else:
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
