#!/usr/bin/env python3
#
#this file contains code for generating data files
#and their corresponding structs.

from collections import OrderedDict
from string import Template
import os.path

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
	it has five members:
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
				"refto"=>"key": name the subdata after this local key's value
					not set: use a simple incremented number for the reference
			"type"=>"enum": this member is a enum. further settings:
				"name"=>"my_enum": the enum name
				"values"=>{"VAL0", "VAL1"}: the possible enum values
					only required for first-time definition
				"xref": "filename": assume this enum to be defined in "filename"
				"filename"=>"header_name": the header where the enum will be placed in
					only required for first-time definition
		additional keys are:
			"length": (optional) to specify array lengths, default 1
				e.g. "type"=>"char" and "length"=>40, would produce "char columnname[40];"
	data is a list, it stores the table rows.
	a list entry (a row) contains dicts: "column name": column value (for this row)
	if "column name" was defined as "subdata" type, the column value is a list
	of objects that have a dump() method.
	this method has to generate a data_export dict again.

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
				1: {"lol_id":  "int"},
				2: {"reference": {"type": "subdata", "refto": "lol_id"}}
			},
			"data": [
				{ "column0": 1337, "lol_id": 42, "reference": [a] },
				{ "column0":   10, "lol_id": 42, "reference": [b, c] },
				{ "column0":  235, "lol_id": 13, "reference": [] },
			],

			#where a.dump(), b.dump() and c.dump() must generate something like:
			{
				"name_table_file":   "referenced_data",
				"name_struct_file":  "awesome_header",
				"name_struct":       "refdata_demo",
				"struct_description: "this is a refrenced sub-struct.",
				"format" : {
					0: { "refdata0": "int" },
				},
				"data": [
					{ "refdata0": 123 },
					{ "refdata0": 456 },
				],
			}
			#end of output of a.dump()
		},

		{
			"name_table_file":   "food_list",
			"name_struct_file":  "awesome_header",
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
			#int,int,refdata_demo
			#column0,column1,reference
			1337,42,[a]
			10,42,[b, c]
			235,13,[]
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
				int lol_id;
				struct reference;
			};
			",
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
						"length": 1,
					}

				else:
					if not "type" in ctype:
						raise Exception("column type has to be specified for %s" % (column))

					#this column is an enum, store it to generate it later
					if ctype["type"] == "enum":
						if not "name" in ctype:
							raise Exception("enum columns need to specify a name")

						#create the needed enum
						if not ctype["name"] in enums:

							if "xref" in ctype:
								#this enum is a cross reference to an existing enum in "filename"
								enum_filename = ctype["xref"]
								enum_values   = None
								enum_is_xref  = True
							else:
								#this enum is defined here the first time
								if not "filename" in ctype:
									raise Exception("first time definition of enum %s requires 'filename'" % ctype["name"])

								if not "values" in ctype:
									raise Exception("first time definition of enum %s requires its 'values'" % ctype["name"])

								enum_filename = ctype["filename"]
								enum_values   = ctype["values"]
								enum_is_xref  = False

							enums[ctype["name"]] = {"values": enum_values, "filename": enum_filename, "xref": enum_is_xref}

						#enum already known
						else:
							if "values" in ctype:
								if not ctype["values"] == enums[ctype["name"]]:
									raise Exception("you reused the enum %s with different values this time." % ctype["name"])

				columns[column] = ctype

		#export csv file
		if format == "csv":

			txt = ""

			if data_struct_desc != None:
				#prepend each line with a comment hash
				csv_struct_desc = "".join(("#%s\n" % line for line in data_struct_desc.split("\n")))
			else:
				csv_struct_desc = ""

			column_types_raw = columns.values()
			column_types     = list()

			#create column types line entries
			for c_raw in column_types_raw:
				if c_raw["type"] == "enum":
					#TODO: maybe list possible enum values here
					c_type = c_raw["name"]
				else:
					c_type = c_raw["type"]

				if "length" in c_raw and c_raw["length"] > 1:
					c = "%s[%d]" % (c_type, c_raw["length"])
				else:
					c = c_type

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
			column_token_parser = dict()
			for idx, (member, column_type) in enumerate(columns.items()):
				dlength = 1

				if not type(column_type) == dict:
					column_type = {"type": column_type}

				dtype = column_type["type"]

				if dtype == "char" and "length" in column_type:
					#read char array
					dlength = column_type["length"]
					column_token_parser[idx] = "strncpy(this->%s, token, %d); this->%s[%d-1] = '\\0';" % (member, dlength, member, dlength)

				elif dtype == "enum":
					#read enum values
					column_token_parser[idx] = "/*TODO read the enum*/ err = -1;"

				elif dtype == "std::string":
					#store std::string
					column_token_parser[idx] = "this->%s = token;" % (member)

				elif dtype in type_scan_lookup:
					dtype_scan = type_scan_lookup[dtype]
					column_token_parser[idx] = "err = sscanf(token, \"%%%s\", &this->%s);" % (dtype_scan, member)

				else:
					raise Exception("unknown column type: %s" % dtype)



			tokenparser = """
		switch (idx) {"""

			#create sscanf entries
			for idx, entry_scanner in column_token_parser.items():
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
	char *token;
	size_t idx = 0;
	int err    = 0;

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
	input: metadata dump (the big one with loads of keys, see ``format_data``
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
#include <string>   //std::string

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
