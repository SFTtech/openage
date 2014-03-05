from collections import OrderedDict
import math
import os
from PIL import Image
from string import Template

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
	global writepath, readpath

	if write:
		basedir = writepath
	else:
		basedir = readpath

	path = os.path.join(basedir, fname)

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
	#ensure that the directory exists
	mkdirs(os.path.dirname(fname))

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


def gather_data(obj, members):
	ret = dict()

	for member in members.values():
		for elem in member.keys():
			ret[elem] = getattr(obj, elem)

	return ret

def gather_format(target_class):
	"""
	returns all necessary class properties of the target to
	create a struct dump.

	this can then be fed into `format_data`, with the format="struct"
	"""

	ret = dict()
	ret["name_struct"]      = target_class.name_struct
	ret["name_struct_file"] = target_class.name_struct_file
	ret["data_format"]      = target_class.data_format

	return ret

def format_data(format, data):
	"""
	formats the given data structure

	this function creates the plaintext being stored in the data files


	data has to be a list.
	each entry equals a data table to create.
	the corresponding value is a dict
	it has fife members:
	name_table_file, name_struct_file, name_struct, format and data
	name is the data structure name to create
	format is a dict, it specifies the C type of a column.
	each key of this dict is a ordering priority number, the value is a dict.
	this dict assigns a column name to its type.
	the type value is either a string or a dict.
	if it's a string, it specifies the C type directly ("uint8_t"),
	if it's a dict, it has one mandatory key: type
	optional keys are: length (to specify array lengths)
	"type"=>"char" and "length"=>40, would produce "char columnname[40];"
	data is a list, it stores the table rows.
	a list entry (a row) contains dicts: "column name": column value (for this row)
	i know you did not understand the format specification, so heres an

	example:
	data = [
		{
			"name_table_file":   "awesome_data",
			"name_struct_file":  "awesome_header",
			"name_struct":       "awesome_stuff",
			"format" : {
				0: {"column0": "int"},
				1: {"column1": "int"},
			},
			"data": [
				{ "column0": 1337, "column1": 42 },
				{ "column0":   10, "column1": 42 },
				{ "column0":  235, "column1": 13 },
			],
		},

		{
			"name_table_file":   "food_list",
			"name_struct_file":  "food_header",
			"name_struct":       "epic_food",
			"format": {
				5:  {"epicness": "int16_t"},
				0:  {"name":     { "type": "char", "length": 30 }},
				10: {"price":    "float"},
			},
			"data": [
				{ "name": "döner",      "epicness":   17, "price": 3.5 },
				{ "name": "kässpatzen", "epicness": 9001, "price": 2.3 },
				{ "name": "schnitzel",  "epicness":  200, "price": 5.7 },
				{ "name": "pizza",      "epicness":  150, "price":   6 },
			],
		},
	]

	this can then be converted to structs, csvs, python source, etc.
	the generated output would be:

	a = format_data("csv", data)

	a == {
		"awesome_data": [
			"
			#struct awesome_stuff
			#int, int
			#column0, column1
			1337, 42
			10, 42
			235, 13
			",
		],

		"food_list": [
			"
			#struct epic_food
			#char[30],int16_t,float
			#name,epicness,price
			döner,17,3.5
			kässpatzen,9001,2.3
			schnitzel,200,5.7
			pizza,150,6
			",
		],
	}

	the generated structs would look like this:

	a = format_data("struct", data)

	a == {
		"awesome_header": [
			"
			struct awesome_stuff {
				int column0;
				int column1;
			};
			",
		],

		"food_header": [
			"
			struct epic_food {
				char[30] name;
				int16_t epicness;
				float price;
			};
			",
		],
	}
	"""

	#csv column delimiter:
	delimiter = ","

	#method signature for fill function
	fill_csignature = "int %sfill(const char *by_line)"

	type_scan_lookup = {
		"char":          "hdd",
		"int8_t":        "hhd",
		"uint8_t":       "hhu",
		"int16_t":       "hd",
		"uint16_t":      "hu",
		"int32_t":       "d",
		"uint32_t":      "u",
		"float":         "f",
		"char_array":    "s",
	}


	ret = dict()

	for data_table in data:
		data_struct_name = data_table["name_struct"]

		#create column list to ensure data order for all rows
		column_prios = sorted(data_table["data_format"].keys())

		#create ordered dict according to priorities
		columns = OrderedDict()
		for prio in column_prios:
			for column, ctype in data_table["data_format"][prio].items():
				if type(ctype) == str:
					ctype = {
						"type": ctype,
						"length": 1,
					}
				else:
					if not "type" in ctype:
						raise Exception("column type has to be specified for %s" % (column))

				columns[column] = ctype

		#this text will be the output
		txt = ""

		#export csv file
		if format == "csv":

			column_types_raw = columns.values()
			column_types     = list()

			#create column types line entries
			for c_raw in column_types_raw:
				if "length" in c_raw and c_raw["length"] > 1:
					c = "%s[%d]" % (c_raw["type"], c_raw["length"])
				else:
					c = c_raw["type"]

				column_types.append(c)

			#csv header:
			txt += "#struct %s\n" % (data_struct_name)
			txt += "#%s\n"        % (delimiter.join(column_types))
			txt += "#%s\n"        % (delimiter.join(columns.keys()))

			#csv data entries:
			for entry in data_table["data"]:
				row_entries = [ str(entry[c]) for c in columns.keys() ]
				txt += "%s\n" % (delimiter.join(row_entries))

			output_name = data_table["name_table_file"]

		#create C struct
		elif format == "struct":

			#struct definition
			txt += "struct %s {\n" % (data_struct_name)

			txt += "\tmember_count = %d;\n\n" % len(columns)

			#create struct members:
			for member, dtype in columns.items():
				dlength = ""
				if "length" in dtype and dtype["length"] > 1:
					dlength = "[%d]" % (dtype["length"])
				dtype   = dtype["type"]

				txt += "\t%s %s%s;\n" % (dtype, member, dlength)

			fill_signature  = fill_csignature % ""
			txt += "\n\t%s;\n" % fill_signature
			#struct ends
			txt += "};\n"

			output_name = data_table["name_struct_file"]


		#create C code for fill function
		elif format == "cfile":
			#it is used to fill a struct instance with data of a line in the csv

			#create parser for a single field of the csv:

			required_scanfs = dict()
			for idx, (member, dtype) in enumerate(columns.items()):
				dlength = 1

				if type(dtype) == dict:
					dlength = dtype["length"]
					dtype   = dtype["type"]

				dtype_scan           = type_scan_lookup[dtype]

				if dlength == 1 and dtype != "char":
					required_scanfs[idx] = "err = sscanf(token, '%%%s', &this->%s);" % (dtype_scan, member)

				elif dtype == "char":
					required_scanfs[idx] = "strncpy(this->%s, token, %d);" % (member, dlength)

				else:
					raise Exception("if you can read this, some thing really faked up.")

			tokenparser = """
		switch (idx) {"""

			#create sscanf entries
			for idx, entry_scanner in required_scanfs.items():
				tokenparser += """
		case {case}: {parser} break;""".format(case=idx, parser=entry_scanner)

			tokenparser += """
		default:
			err = -2;
		}"""

			fill_signature = fill_csignature % ("%s::" % data_struct_name)

			#definition of filling function
			txt += Template("""
$funcsignature {
	char separators[] = "$delimiters";
	char* token;
	size_t idx = 0;
	int err = 0;

	token = strtok(by_line, separators);
	while (token != nullptr && idx < this->member_count) {
		$tokenhandler

		if (err < 0) {
			return err;
		}
		token = strtok(nullptr, separators);
		idx += 1;
	}
	return (idx != this->member_count);
}
""").substitute(funcsignature=fill_signature, delimiters=delimiter, tokenhandler=tokenparser)

			output_name = data_table["name_struct_file"]


		else:
			raise Exception("unknown format specified: %s" % format)

		#create list or append to it, if output_name was defined earlier
		if output_name in ret:
			ret[output_name].append(txt)
		else:
			ret[output_name] = [ txt ]

	return ret


def transform_dump(data_dump, output_formats):
	"""
	input: data dump
	output: {output_format => {filename => [file_content]}}
	"""

	ret = dict()

	#create all files of the specified formats and sections
	for output_format in output_formats:

		#format data according to output_format
		formatted_data = format_data(output_format, data_dump)

		if output_format not in ret:
			ret[output_format] = dict()

		for filename, sections in formatted_data.items():
			if filename in ret:
				ret[output_format][filename] += sections
			else:
				ret[output_format][filename]  = sections

	return ret


def merge_data_dump(transformed_data): #data_dump, output_formats):
	"""
	save a given transformed data dump to files.

	input: {output_format => {filename => [file_content]}}
	output: {full_filename => file_content}
	"""

	#inserted warning message for generated files
	dontedit = """
//do not edit this file!
//make changes in the convert script and regenerate the files."""

	#default preferences for output modes
	default_preferences = {
		"folder":         "",
		"file_suffix":    "",
		"content_prefix": "",
		"content_suffix": "",
		"presuffix_func": lambda x: x
	}


	#override the default preferences with the
	#configuration for all the output formats
	output_preferences = {
		"csv": {
			"folder":      "processed/",
			"file_suffix": ".docx",
		},
		"struct": {
			"file_suffix": ".h",
			"content_prefix": """#ifndef _${filename}_H_
#define _${filename}_H_

%s

namespace engine {
""" % dontedit,
			"content_suffix": """
} //namespace engine

#endif // _${filename}_H_
""",
			"presuffix_func": lambda x: x.upper()
		},
		"cfile": {
			"file_suffix":    ".cpp",
			"content_prefix": """#include "${filename}.h"

#include <string.h> //strtok

%s

namespace engine {\n\n""" % dontedit,
			"content_suffix": "} //namespace engine\n",
		}
	}


	#return dict, key: file_name, value: file_content
	ret = dict()

	#iterate over all output formats
	for output_format, formatted_data in transformed_data.items():

		#apply preference overrides
		prefs = default_preferences.copy()
		for pref_name, pref_value in output_preferences[output_format].items():
			prefs[pref_name] = pref_value

		#create all files exported by dumping the requested sections
		for output_name, output_data in formatted_data.items():

			#merge file contents
			file_data = ""
			for block in output_data:
				file_data += "%s\n" % block

			#create content, with prefix and suffix (actually header guards)
			subst_filename = prefs["presuffix_func"](output_name)
			content_prefix = Template(prefs["content_prefix"]).substitute(filename=subst_filename)
			content_suffix = Template(prefs["content_suffix"]).substitute(filename=subst_filename)
			file_data = content_prefix + file_data + content_suffix

			#determine output file name
			file_name = os.path.join(prefs["folder"], "%s%s" % (output_name, prefs["file_suffix"]))
			file_name = file_get_path(file_name, write=True)

			#whee, store the content
			ret[file_name] = file_data

	return ret
