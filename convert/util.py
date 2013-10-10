import os, argparse
from PIL import Image
from extractionrule import ExtractionRule

class NamedObject:
	def __init__(self, name, **kw):
		self.name = name
		self.__dict__.update(kw)

	def __repr__(self):
		return self.name

p = argparse.ArgumentParser()
p.add_argument("srcdir", help = "The Age of Empires II root directory")
p.add_argument("destdir", help = "The openage root directory")
p.add_argument("extract", nargs = "*", help = "A specific extraction rule, such as graphics:*.slp, or *:*.wav. If no rules are specified, *:*.* is assumed")
p.add_argument("-v", "--verbose", help = "Turn on verbose log messages", action='count', default=0)

args = p.parse_args()
if args.extract == []:
	args.extract.append('*:*.*')

args.extractionrules = [ExtractionRule(e) for e in args.extract]

dbgstack = [[None, 0]]

def dbg(msg = None, lvl = None, push = None, pop = None):
	if lvl == None:
		lvl = dbgstack[-1][1]

	if args.verbose >= lvl and msg != None:
		print((len(dbgstack) - 1) * "  " + str(msg))
	if push != None:
		dbgstack.append([push, lvl])
	if pop != None:
		if pop == True:
			dbgstack.pop()
		elif dbgstack.pop()[0] != pop:
			raise Exception(str(pop) + " is not on top of the debug stack")

def mkdirs(path):
	try:
		os.makedirs(path)
	except FileExistsError:
		pass

def file_getpath(fname, write = False):
	if write:
		basedir = args.destdir + "/data/age"
	else:
		basedir = args.srcdir

	path = basedir + '/' + fname

	if write:
		#ensure that the directory exists
		mkdirs(os.path.dirname(path))

	return path

def file_open(fname, binary = True, write = False):
	path = file_getpath(fname, write = write)

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
		data.save(file_getpath(fname, write = True))
	else:
		raise Exception("Unknown data type for writing: " + str(type(data)))

#reads data from a file in the source directory
def file_read(fname, datatype = str):
	if datatype == bytes:
		return file_open(fname, binary = True, write = False).read()
	elif datatype == str:
		return file_open(fname, binary = False, write = False).read()
	elif datatype == Image.Image:
		return Image.open(file_getpath(fname, write = False)).convert('RGBA')
	else:
		raise Exception("Unknown data type for reading: " + str(datatype))
