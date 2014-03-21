from collections import OrderedDict
import os
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


def file_get_path(fname, write=False, mkdir=False):
	global writepath, readpath

	if write:
		basedir = writepath
	else:
		basedir = readpath

	path = os.path.join(basedir, fname)

	if mkdir:
		dirname = os.path.dirname(path)
		mkdirs(dirname)

	return path


def file_open(path, binary = True, write = False):
	if write:
		flags = 'w'
	else:
		flags = 'r'
	if binary:
		flags += 'b'

	return open(path, flags)


def file_write(fname, data):
	"""
	writes data to a file with given name.
	"""

	#ensure that the directory exists
	mkdirs(os.path.dirname(fname))

	if type(data) == bytes:
		handle = file_open(fname, binary=True, write=True)
		handle.write(data)
		handle.close()
	elif type(data) == str:
		handle = file_open(fname, binary=False, write=True)
		handle.write(data)
		handle.close()
	else:
		raise Exception("Unknown data type for writing: " + str(type(data)))


def file_write_multi(file_dict, file_prefix=""):
	"""
	save the given file dictionary to files

	key: filename
	value: file content
	"""

	written_files = list()

	for file_name, file_data in file_dict.items():
		file_name = file_prefix + file_name
		dbg("saving %s.." % file_name, 1)
		file_name = file_get_path(file_name, write=True, mkdir=True)
		file_write(file_name, file_data)

		written_files.append(file_name)

	return written_files


def file_write_image(filename, image):
	"""
	writes a PIL image to a filename
	"""
	image.save(filename)


#reads data from a file in the source directory
def file_read(fname, datatype = str):
	if datatype == bytes:
		return file_open(fname, binary=True, write=False).read()
	elif datatype == str:
		return file_open(fname, binary=False, write=False).read()
	else:
		raise Exception("Unknown data type for reading: " + str(datatype))


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
	ret["name_struct"]        = target_class.name_struct
	ret["name_struct_file"]   = target_class.name_struct_file
	ret["data_format"]        = target_class.data_format
	ret["struct_description"] = target_class.struct_description

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
			"struct_description: "you can't believe how epic this struct is!",
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
			"struct_description: None,
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
			#you can't believe how epic this struct is!
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
			/**
			you can't believe how epic this struct is!
			*/
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
	fill_csignature = "int %sfill(char *by_line)"

	type_scan_lookup = {
		"char":          "hdd",
		"int8_t":        "hhd",
		"uint8_t":       "hhu",
		"int16_t":       "hd",
		"uint16_t":      "hu",
		"int32_t":       "d",
		"uint32_t":      "u",
		"float":         "f",
	}


	ret = dict()

	for data_table in data:
		data_struct_name = data_table["name_struct"]
		data_struct_desc = data_table["struct_description"]

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

			if data_struct_desc != None:
				#prepend each line with a comment hash
				csv_struct_desc = "".join(("#%s\n" % line for line in data_struct_desc.split("\n")))
			else:
				csv_struct_desc = ""

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
			txt += "%s"           % (csv_struct_desc)
			txt += "#%s\n"        % (delimiter.join(column_types))
			txt += "#%s\n"        % (delimiter.join(columns.keys()))

			#csv data entries:
			for entry in data_table["data"]:
				row_entries = [ str(entry[c]) for c in columns.keys() ]
				txt += "%s\n" % (delimiter.join(row_entries))

			output_name = data_table["name_table_file"]

		#create C struct
		elif format == "struct":

			#optional struct description
			if data_struct_desc != None:
				txt += "/**\n%s\n*/\n" % data_struct_desc

			#struct definition
			txt += "struct %s {\n" % (data_struct_name)


			#create struct members:
			for member, dtype in columns.items():
				dlength = ""
				if "length" in dtype and dtype["length"] > 1:
					dlength = "[%d]" % (dtype["length"])
				dtype   = dtype["type"]

				txt += "\t%s %s%s;\n" % (dtype, member, dlength)

			#append member count variable
			txt += "\n\tstatic constexpr size_t member_count = %d;\n" % len(columns)

			#add filling function prototype
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
					required_scanfs[idx] = "err = sscanf(token, \"%%%s\", &this->%s);" % (dtype_scan, member)

				elif dtype == "char":
					required_scanfs[idx] = "strncpy(this->%s, token, %d); this->%s[%d-1] = '\\0';" % (member, dlength, member, dlength)

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


def metadata_format(data_dump, output_formats):
	"""
	input: metadata dump
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


def merge_data_dump(transformed_data):
	"""
	save a given transformed data dump to files.

	this function merges the file sections and adds per-file
	stuff like includes, header guards and the namespace.

	input: {output_format => {filename => [file_content]}}
	output: {full_filename => file_content}
	"""

	#inserted warning message for generated files
	dontedit = """
//do not edit this file, it's autogenerated. all changes will be undone!
//make changes in the convert script and regenerate the files."""

	#default preferences for output modes
	default_preferences = {
		"folder":         "",
		"file_suffix":    "",
		"content_prefix": "",
		"content_suffix": "",
		"binary":         False,
		"presuffix_func": lambda x: x
	}


	#override the default preferences with the
	#configuration for all the output formats
	output_preferences = {
		"csv": {
			"folder":      "",
			"file_suffix": ".docx",
		},
		"png": {
			"folder":      "",
			"file_suffix": ".png",
			"binary":      True,
		},
		"struct": {
			"file_suffix": ".h",
			"content_prefix": """#ifndef _${filename}_H_
#define _${filename}_H_

#include <stddef.h> //various types
#include <stdint.h> //int types

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
#include <stdio.h>  //sscanf

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
			if prefs["binary"]:
				file_data = b""
			else:
				file_data = ""

			for block in output_data:
				file_data += block

			#only append header and footer of file when non-binary
			if not prefs["binary"]:
				#create content, with prefix and suffix (actually header guards)
				subst_filename = prefs["presuffix_func"](output_name)

				#fill file header and footer with the generated filename
				content_prefix = Template(prefs["content_prefix"]).substitute(filename=subst_filename)
				content_suffix = Template(prefs["content_suffix"]).substitute(filename=subst_filename)

				#this is the final file content
				file_data = content_prefix + file_data + content_suffix

			#determine output file name
			output_filename_parts = [
				prefs["folder"],
				"%s%s" % (output_name, prefs["file_suffix"])
			]
			file_name = os.path.join(*output_filename_parts)

			#whee, store the content
			ret[file_name] = file_data

	return ret


class VirtualFile:
	"""
	provides a virtual file, with seek, tell, read and write functions.

	this can be used to store PIL images in particular.
	operates in binary mode.
	"""

	def __init__(self):
		self.buf = b""

	def seek(self, destination, whence=0):
		raise NotImplementedError("seek not implemented")

	def tell(self):
		raise NotImplementedError("tell not implemented")

	def read(self):
		raise NotImplementedError("read not implemented")

	def write(self, data):
		self.buf += data

	def data(self):
		return self.buf
