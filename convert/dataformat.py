#!/usr/bin/env python3
#
#this file contains code for generating data files
#and their corresponding structs.

from collections import OrderedDict
from string import Template
import os.path

def encode_value(val):
	"""
	encodes val to a (possibly escaped) string,
	for use in a csv column of type valtype (string)
	"""
	result = ""
	for c in str(val):
		if c == '\\':
			result += '\\\\'
		elif c == ',':
			result += '\\,'
		elif c == '\n':
			result += '\\n'
		else:
			result += c

	return result

def gather_data(obj, members):
	"""
	queries the given object for the given member variables
	and returns that as a dict.

	key: member name
	value: obj's member value
	"""
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
	transforms and merges the given data structure
	according to the format specified in the data itself.

	this function creates the plaintext being stored in the data files


	data has to be a list.
	each entry equals a data table to create (called data_export for reference).
	the corresponding value is a dict
	it has these members:
		name_table_file, name_struct_file, name_struct, format and data
			name is the data structure name to create
			format is a dict, it specifies the C type of a column.
	each key of this dict is a ordering priority number, the value is a dict.
	this dict assigns a column name to its type.
	the type value is either a string or a dict.
		if it's a string, it specifies the C type directly (e.g. "uint8_t"),
		if it's a dict, it has one mandatory key:
			"type"=>ctype: this member will have this C type
			"type"=>"subdata": this member is a list of submembers, which fill be
				resolved separately and referencable by the parent later
				this allows nesting and cross referencing data structures.
				settings:
				"ref_type"=>class: data class of the subdata column
				"ref_to"=>"key": add the local key's value to name the reference
					not set: just use a simple incremented number for the reference
			"type"=>"enum": this member is a enum. further settings:
				"name"=>"my_enum": the enum name
				"values"=>["VAL0", "VAL1", ...]: the possible enum values (list or set)
					only required for first-time definition
				"xref": "filename": assume this enum to be defined in "filename"
					"filename" may be None when the definition is in the same file
				"filename"=>"header_name": the header where the enum will be placed in
					when ommitted, use 'name_struct_file'
		additional keys are:
			"length": (optional) to specify array lengths, default 1
				e.g. "type"=>"char" and "length"=>40, would produce "char columnname[40];"
	data is a list, it stores the table rows.
	a list entry (a row) contains dicts: "column name": column value (for this row)

	this input is transformed to filename-grouped output
	so that real files can be created afterwards

	i know you did not understand the format specification, so heres an example:

	input:
	data = [
		{
			"name_table_file":   "civilisation_data",
			"name_struct_file":  "gamedata_header",
			"name_struct":       "civilisation",
			"struct_description: "represents one civilisation.",
			"format": {
				0: {"civ_id":  "int"},
				1: {"mode":    {"type": "enum", "name": "civ_rating", "values": ["boring", "epic", "lame"] }},
				2: {"unit":    {"type": "subdata", "ref_to": "civ_id", "ref_type": Unit }},
			},
			"data": [
				{ "civ_id": 1337, "mode": "epic",   "unit": a },
				{ "civ_id":   42, "mode": "boring", "unit": b },
				{ "civ_id":  235, "mode": "boring", "unit": c },
			],

			#a b and c can be queried recursively,
			#they have to provide the data to be passed to `format_data`.
			#a.dump("unit_data_civ_1337")
			#generates something like:
			[{
				"name_table_file":   "unit_data_civ_1337",
				"name_struct_file":  "gamedata_header",
				"name_struct":       "unit",
				"struct_description: "walking or real estate objects.",
				"format": {
					0: { "id":   "int32_t" },
					1: { "name": "std::string" },
				},
				"data": [
					{ "id": 10, "name": "hussar, 1337 edition" },
					{ "id": 12, "name": "dorfbewohner" },
				],
			}],
			#end of output of a.dump

			#b.dump and c.dump result in something similar,
			#the a b and c dumps can then be stored into a subfolder, like civilisation_data_unit/
		},

		{
			"name_table_file":   "terrain_list",
			"name_struct_file":  "gamedata_header",
			"name_struct":       "terrain",
			"struct_description: None,
			"format": {
				5:  {"name":   { "type": "char", "length": 30 }},
				0:  {"id":     "int16_t"},
				10: {"size":   "float"},
			},
			"data": [
				{ "name": "schwarzwald", "id":  88, "size": 9000.001 },
				{ "name": "alpen",       "id":  10, "size":    4.2   },
				{ "name": "wattenmeer",  "id": 400, "size":  235.42  },
			],
		},
	]

	this can then be converted to structs, csvs, python source, etc.
	the generated output would be:

	a = format_data("csv", data)

	a == {
		"civilisation_data": {
			"data": [
				"
				#struct civilisation
				#represents one civilisation.
				#int,civ_rating,Unit
				#civ_id,mode,unit
				1337,epic,civilisation_data-unit/unit_data_civ_1337
				42,boring,civilisation_data-unit/unit_data_civ_42
				235,boring,civilisation_data-unit/unit_data_civ_235
				",
			],
		},

		"civilisation_data_unit/unit_data_civ_1337": {
			"data": [
				"
				#struct unit
				#walking or real-estate objects.
				#int32_t,std::string
				#id,name
				10,Hussar\, 1337 edition
				12,Dorfbewohner
				",
			],
		},

		"civilisation_data_unit/unit_data_civ_42": {
			"data": [
				"
				#struct unit
				#walking or real-estate objects.
				#int32_t,std::string
				#id,name
				1337,flying spaghetti monster
				12,Dorfbewohner
				",
			],
		},

		"civilisation_data_unit/unit_data_civ_235": you can imagine it yourself..

		"terrain_list": {
			"data": [
				"
				#struct terrain
				#int16_t,char[30],float
				#name,id,size
				schwarzwald,88,9001.001
				alpen,10,4.2
				wattenmeer,400,235.42
				",
			],
		},
	}

	the generated structs would look like this:

	a = format_data("struct", data)

	a == {
		"gamedata_header": [
			{
				"data":
				"
				enum class civ_rating {
					boring,
					epic,
					lame
				};
				",
			},

			{
				"data":
				"
				/**
				 * represents one civilisation.
				 */
				struct civilisation {
					int civ_id;
					civ_rating mode;
					std::string unit_filename;
				};
				",
				"metadata": {
					"headers": {
						"global": {
							"string",
						},
						"local": {
							"gamedata_header",
						},
					},
					"typerefs": {
						"unit",
					},
				},
			},

			{
				"data":
				"
				struct unit {
					int32_t id;
					std::string name;
				};
				",
				"metadata": {
					"headers": {
						"global": {
							"stdint.h",
						},
					},
				},
			},

			{
				"data":
				"
				struct terrain {
					int16_t id;
					char name[30];
					float size;
				};
				",
			},
		],
	}


	=> the output format is:
	{"filename": [{"data": section data, "metadata": {metadata}} ] }
		where metadata may contain
			"headers" -> "local": set()/list(), "global": set()/list()
			"typerefs" -> set() of types that need to be defined before or forward declared

	this data can then be passed to some function
	that creates the csv, cpp and h files

	i'll omit the generated c code, because reasons.

	conclusion: this function is the central part of the data exporting
	functionality.
	"""

	#csv column delimiter:
	delimiter = ","

	#method signature for fill function
	fill_csignature = "bool %sfill(char *by_line)"

	type_scan_lookup = {
		"char":          "hhd",
		"int8_t":        "hhd",
		"uint8_t":       "hhu",
		"int16_t":       "hd",
		"uint16_t":      "hu",
		"int":           "d",
		"int32_t":       "d",
		"uint":          "u",
		"uint32_t":      "u",
		"float":         "f",
	}


	#returned data: key=filename/structname, value:formatted data
	ret = dict()

	#needed enums storage
	#key=enum name
	#value: {"values": set(enum values), "filename": name}
	enums = dict()

	#these headers will be needed for the current format selected.
	needed_headers = set()


	def store_output(result_storage, key, value, prepend=False):
		#create list or append to it, if output_name was defined earlier
		if key in result_storage:
			if not prepend:
				ret[key].append(value)
			else:
				ret[key].insert(0, value)
		else:
			ret[key] = [ value ]


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
					}
				else:
					if not "type" in ctype:
						raise Exception("column type has to be specified for %s" % (column))

				columns[column] = ctype

		#check definition validity of all data column definitions
		for column_name, ctype in columns.items():

			#this column is an enum, store it to generate it later
			if ctype["type"] == "enum":
				if "name" not in ctype:
					raise Exception("enum columns need to specify a name")

				#create the needed enum
				if ctype["name"] not in enums:

					if "xref" in ctype:
						#this enum is a cross reference to an existing enum
						enum_is_xref  = True
						enum_filename = ctype["xref"]
						enum_values   = None

					else:
						#this enum is defined here the first time
						enum_is_xref  = False

						if "filename" in ctype:
							enum_filename = ctype["filename"]
						else:
							enum_filename = data_table["name_struct_file"]

						if "values" not in ctype:
							raise Exception("first time definition of enum %s requires its 'values'" % ctype["name"])
						else:
							enum_values   = ctype["values"]

					#add the requested enum to the enum list
					enums[ctype["name"]] = {"values": enum_values, "filename": enum_filename, "xref": enum_is_xref}

				#enum already known
				else:
					if "values" in ctype:
						if not ctype["values"] == enums[ctype["name"]]:
							raise Exception("you reused the enum %s with different values this time." % ctype["name"])

			#this column references to a list of member data sets
			elif ctype["type"] == "subdata":
				if "ref_to" in ctype:
					if ctype["ref_to"] not in columns.keys():
						raise Exception("subdata reference specification for column %s is no valid column name: %s" % (column_name, ctype["ref_to"]))
				else:
					ctype["ref_to"] = None

				if "ref_type" not in ctype:
					raise Exception("reference class must be specified by 'ref_type': for column %s" % column_name)


		#export csv file
		if format == "csv":

			txt = ""

			if data_struct_desc != None:
				#split description to its lines
				data_struct_desc = data_struct_desc.split("\n")

				#prepend each line with a comment hash
				csv_struct_desc = "".join(("#%s\n" % line for line in data_struct_desc))
			else:
				csv_struct_desc = ""

			column_types_raw = columns.values()
			csv_column_types = list()

			#create column types line entries
			for c_raw in column_types_raw:
				if c_raw["type"] == "enum":
					#set the column type to the enum name
					#TODO: maybe list possible enum values here
					c_type = c_raw["name"]

				elif c_raw["type"] == "subdata":
					#set the column type to the referenced class name
					c_type = c_raw["ref_type"].__name__

				else:
					#just use the column type name
					c_type = c_raw["type"]

				if "length" in c_raw and c_raw["length"] > 1:
					c = "%s[%d]" % (c_type, c_raw["length"])
				else:
					c = c_type

				csv_column_types.append(c)

			#csv header:
			txt += "#struct %s\n" % (data_struct_name)
			txt += "%s"           % (csv_struct_desc)
			txt += "#%s\n"        % (delimiter.join(csv_column_types))
			txt += "#%s\n"        % (delimiter.join(columns.keys()))

			#csv data entries:
			for idx, data_line in enumerate(data_table["data"]):
				row_entries = list()
				for col_name, col_type in columns.items():
					entry = data_line[column_name]

					#check if enum data value is valid
					if col_type["type"] == "enum":
						if entry not in col_type["values"]:
							raise Exception("data entry %d '%s' not a valid enum %s value" % (idx, entry, col_type["name"]))

					#generate subdata reference link
					elif col_type["type"] == "subdata":
						if col_type["ref_to"] == None:
							ref_to = idx
						else:
							ref_to = data_line[col_type["refto"]]

						subdata_folder = "%s-%s" % (data_table["name_table_file"], col_name)
						subdata_table_name = "TODO"

						entry = "%s/%s" % (subdata_folder, subdata_table_name)

					#escape the entry (e.g. newlines and commas)
					entry = encode_value(entry)
					row_entries.append(entry)

				txt += "%s\n" % (delimiter.join(row_entries))

			output_name = data_table["name_table_file"]
			store_output(ret, output_name, txt)

		#create C struct
		elif format == "struct":

			txt = ""

			#optional struct description
			if data_struct_desc != None:
				txt += "/**\n * "

				#prepend * before every comment line
				txt += "\n * ".join(data_struct_desc.split("\n"))

				txt += "\n */\n"

			#struct definition
			txt += "struct %s {\n" % (data_struct_name)

			#create struct members:
			for member, dtype in columns.items():
				dlength = ""
				if "length" in dtype and dtype["length"] > 1:
					dlength = "[%d]" % (dtype["length"])

				if dtype["type"] == "enum":
					dtype = dtype["name"]
				elif dtype["type"] == "subdata":
					dtype = "std::string"
				else:
					dtype = dtype["type"]

				txt += "\t%s %s%s;\n" % (dtype, member, dlength)

			#append member count variable
			txt += "\n\tstatic constexpr size_t member_count = %d;\n" % len(columns)

			#add filling function prototype
			fill_signature  = fill_csignature % ""
			txt += "\n\t%s;\n" % fill_signature

			#struct ends
			txt += "};\n"

			output_name = data_table["name_struct_file"]
			store_output(ret, output_name, txt)


		#create C code for fill function
		elif format == "cfile":
			#it is used to fill a struct instance with data of a line in the csv

			txt = ""

			#creating a parser for a single field of the csv
			parse_tokens = []

			for idx, (member, column_type) in enumerate(columns.items()):
				dlength = 1

				if not type(column_type) == dict:
					column_type = {"type": column_type}

				dtype = column_type["type"]

				if dtype == "char" and "length" in column_type:
					#read char array
					dlength = column_type["length"]
					parsestr = "strncpy(this->%s, buf[%d], %d); this->%s[%d-1] = '\\0';" % (member, idx, dlength, member, dlength)
					parse_tokens.append(parsestr)

				elif dtype == "enum":
					#read enum values
					enum_name = column_type["name"]
					enum_values = column_type["values"]

					enum_parse_else = ""
					enum_parser = list()
					enum_parser.append("//parse enum %s" % (enum_name))
					for enum_value in enum_values:
						enum_parser.append("%sif (0 == strcmp(buf[%d], \"%s\")) {" % (enum_parse_else, idx, enum_value))
						enum_parser.append("\tthis->%s = %s::%s;"                % (member, enum_name, enum_value))
						enum_parser.append("}")
						enum_parse_else = "else "
					enum_parser.append("else {")
					enum_parser.append("\treturn false;")
					enum_parser.append("}")
					parse_tokens.append(enum_parser)

				elif dtype == "std::string":
					#store std::string
					parse_tokens.append("this->%s = buf[%d];" % (member, idx))

				elif dtype in type_scan_lookup:
					dtype_scan = type_scan_lookup[dtype]
					parsestr = "if (sscanf(buf[%d], \"%%%s\", &this->%s) != 1) { return false; }" % (idx, dtype_scan, member)
					parse_tokens.append(parsestr)

				else:
					raise Exception("unknown column type: %s" % dtype)

			#flatten the token parser statement list
			parse_tokens = sum((type(i) == list and i or i.split('\n') for i in parse_tokens), [])

			#indent/newline the token parser statement list
			parse_tokens = "\n\t".join(parse_tokens)

			#prepend struct name to fill function signature
			fill_signature = fill_csignature % ("%s::" % data_struct_name)

			member_count = data_struct_name + "::member_count"

			#definition of filling function
			txt += Template("""
$funcsignature {
	//tokenize
	char *buf[$member_count];
	int count = engine::util::string_tokenize_to_buf(by_line, '$delimiter', buf, $member_count);

	//check tokenization result
	if (count != $member_count) {
		return false;
	}

	//parse tokens
	$parse_tokens

	return true;
}
""").substitute(funcsignature=fill_signature, delimiter=delimiter, parse_tokens=parse_tokens, member_count=member_count)

			output_name = data_table["name_struct_file"]
			store_output(ret, output_name, txt)

		else:
			raise Exception("unknown format specified: %s" % format)


	#now create the needed enums.
	if format == "struct":
		for enum_name, enum_settings in enums.items():
			if enum_settings["xref"] == True:
				#skip cross referenced enum
				continue

			output_name = enum_settings["filename"]

			#create enum definition
			txt = "enum class %s {\n\t" % enum_name
			txt += ",\n\t".join(enum_settings["values"])
			txt += "\n};\n\n"

			store_output(ret, output_name, txt, prepend=True)

	return ret


def metadata_format(data_dump, output_formats):
	"""
	input: metadata dump (the big one with loads of keys, see ``format_data``)
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

#include <stddef.h> //size_t
#include <stdint.h> //int types
#include <string>   //std::string

#include "../engine/util/strings.h"

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
