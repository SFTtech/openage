import os, argparse

class NamedObject:
	def __init__(self, name, **kw):
		self.name = name
		self.__dict__.update(kw)

	def __repr__(self):
		return self.name

def mkdirs(path):
	try:
		os.makedirs(path)
	except FileExistsError:
		pass

p = argparse.ArgumentParser()
p.add_argument("srcdir", help = "The Age of Empires II root directory")
p.add_argument("destdir", help = "The openage root/data/age directory")
p.add_argument("-v", "--verbose", help = "Turn on verbose log messages", action='count', default=0)

args = p.parse_args()

def dbg(msg, lvl = 1):
	if args.verbose < lvl:
		return
	print(msg)
