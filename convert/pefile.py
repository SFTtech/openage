#TODO properly integrate with rest of convert script collection
#TODO this includes:
#TODO   using the dbg function
#TODO   proper pydoc
from struct import unpack_from, Struct
from collections import defaultdict
from langcodes import langcodes
import os

#PE file
IMAGE_OPTIONAL_HDR32_MAGIC = 0x010b
PE_SIGNATURE = b"PE\0\0"
SUPPORTED_OPTIONAL_HEADER_SIZE = 224
endianness = "< "

#struct image_dos_header {  // DOS PE header
#  uint16_t e_magic;         // Magic number
#  uint16_t e_cblp;          // Bytes on last page of file
#  uint16_t e_cp;            // Pages in file
#  uint16_t e_crlc;          // Relocations
#  uint16_t e_cparhdr;       // Size of header in paragraphs
#  uint16_t e_minalloc;      // Minimum extra paragraphs needed
#  uint16_t e_maxalloc;      // Maximum extra paragraphs needed
#  uint16_t e_ss;            // Initial (relative) SS value
#  uint16_t e_sp;            // Initial SP value
#  uint16_t e_csum;          // Checksum
#  uint16_t e_ip;            // Initial IP value
#  uint16_t e_cs;            // Initial (relative) CS value
#  uint16_t e_lfarlc;        // File address of relocation table
#  uint16_t e_ovno;          // Overlay number
#  uint16_t e_res[4];        // Reserved words
#  uint16_t e_oemid;         // OEM identifier (for e_oeminfo)
#  uint16_t e_oeminfo;       // OEM information; e_oemid specific
#  uint16_t e_res2[10];      // Reserved words
#  uint32_t e_lfanew;        // File address of new exe header
#};
image_dos_header = Struct(endianness + "30H I")

#struct image_file_header {
#  char     signature[4];
#  uint16_t machine;
#  uint16_t number_of_sections;
#  uint32_t time_stamp;
#  uint32_t symbol_table_ptr;
#  uint32_t symbol_count;
#  uint16_t size_of_optional_header;
#  uint16_t charactersitics;
#};
image_file_header = Struct(endianness + "4s HH 3I HH")

#struct image_data_directory {
#  uint32_t rva;
#  uint32_t size;
#};
image_data_directory = Struct(endianness + "II")

#struct image_optional_header32 {
#  uint16_t magic;
#  unsigned char major_linker_version;
#  unsigned char minor_linker_version;
#  uint32_t size_of_code;
#  uint32_t size_of_initialized_data;
#  uint32_t size_of_uninitialized_data;
#  uint32_t address_of_entry_point;
#  uint32_t base_of_code;
#  uint32_t base_of_data;
#  uint32_t image_base;
#  uint32_t section_alignment;
#  uint32_t file_alignment;
#  uint16_t major_operating_system_version;
#  uint16_t minor_operating_system_version;
#  uint16_t major_image_version;
#  uint16_t minor_image_version;
#  uint16_t major_subsystem_version;
#  uint16_t minor_subsystem_version;
#  uint32_t win32_version_value;
#  uint32_t size_of_image;
#  uint32_t size_of_headers;
#  uint32_t check_sum;
#  uint16_t subsystem;
#  uint16_t dll_characteristics;
#  uint32_t size_of_stack_reserve;
#  uint32_t size_of_stack_commit;
#  uint32_t size_of_heap_reserve;
#  uint32_t size_of_heap_commit;
#  uint32_t loader_flags;
#  uint32_t number_of_rva_and_sizes;
#  struct image_data_directory data_directory[DATA_DIRECTORY_COUNT];
#};
image_optional_header32 = Struct(endianness + "H BB 9I 6H 4I HH 6I")
#note that this struct is always followed by number_of_rva_and_sizes image_data_directory structs

#struct image_section_header {
#  char name[8]; // needs extra treatment for names > 8 characters (see doc)
#  uint32_t virtual_size;
#  uint32_t virtual_adress;
#  uint32_t size_of_raw_data;
#  uint32_t pointer_to_raw_data;
#  uint32_t pointer_to_relocations;
#  uint32_t pointer_to_linenumbers;
#  uint16_t number_of_relocations;
#  uint16_t number_of_linenumbers;
#  uint32_t characteristics;
#};
image_section_header = Struct(endianness + "8s 6I HH I")

#ressource section
SECTION_NAME_RESOURCE = ".rsrc"

#ressource tree
#types for id in ressource directory root node
restypes = {
0: 'unknown',
1: 'cursor',
2: 'bitmap',
3: 'icon',
4: 'menu',
5: 'dialog',
6: 'string',
7: 'fontdir',
8: 'font',
9: 'accelerator',
10: 'rcdata',
11: 'messagetable',
12: 'group_cursor',
14: 'group_icon',
16: 'version',
17: 'dlginclude',
19: 'plugplay',
20: 'vxd',
21: 'anicursor',
22: 'aniicon',
23: 'html',
24: 'manifest'}
#reverse-lookup directory
restypesinv = {v:k for k, v in restypes.items()}

#string ressources
STRINGTABLE_SIZE = 16
STRINGTABLE_ENCODING = 'utf-16-le'
STRINGTABLE_LENGTH_MULTIPLIER =2

#struct resource_data_entry {
#  uint32_t data_rva; // Only important on read and will be overwritten on write
#  uint32_t size;
#  uint32_t codepage;
#  uint32_t reserved; // must be 0
#};
resource_data_entry = Struct(endianness + "4I")

#struct resource_directory_entry {
#  uint32_t id; //or name rva //rva: relative virtual address
#  uint32_t rva; // if high bit: subdirectory_entry rva else: data_entry rva
#};
resource_directory_entry = Struct(endianness + "II")

#struct resource_directory_table {
#  uint32_t characteristics;
#  uint32_t time_stamp;
#  uint16_t major_version;
#  uint16_t minor_version;
#  uint16_t number_of_name_entries;
#  uint16_t number_of_id_entries;
#};
resource_directory_table = Struct(endianness + "II 4H")

#struct stringtable_length_specifier {
#  uint16_t string_length;
#}
stringtable_length_specifier = Struct(endianness + "H")

class PEFile:
	def __init__(self, fname):
		self.defaultcultureid = 0
		self.defaultcp = 0
		self.todefaultcharsetcd = 0
		self.fromdefaultcharsetcd = 0

		data = open(fname, 'rb').read()

		#read DOS header
		dosheader = image_dos_header.unpack_from(data, 0)

		print("DOS header: " + repr(dosheader))

		#read PE header
		headerpos = dosheader[30] #e_lfanew

		peheader = image_file_header.unpack_from(data, headerpos)

		signature, machine, number_of_sections, time_stamp, symbol_table_ptr, symbol_count, size_of_optional_header, characteristics = peheader

		if signature != PE_SIGNATURE:
			raise Exception("Invalid PE signature")

		print("PE header: " + repr(peheader))

		#read optional header
		optheaderpos = headerpos + image_file_header.size

		if size_of_optional_header != SUPPORTED_OPTIONAL_HEADER_SIZE:
			raise Exception("Unsupported optional header size")

		optheader = image_optional_header32.unpack_from(data, optheaderpos)
		if optheader[0] != IMAGE_OPTIONAL_HDR32_MAGIC:
			raise Exception("Bad magic number for optional header")
		number_of_rva_and_sizes = optheader[-1]

		print("Optional header: " + repr(optheader))

		#read data directory
		datadirectorypos = optheaderpos + image_optional_header32.size
		datadirectory = []
		for i in range(number_of_rva_and_sizes):
			entry = image_data_directory.unpack_from(data, datadirectorypos + i * image_data_directory.size)
			datadirectory.append(entry)

		#read section headers
		secttablepos = datadirectorypos + number_of_rva_and_sizes * image_data_directory.size

		#number of sections is known from PE header
		sections = {}
		for i in range(number_of_sections):
			sectionheader = image_section_header.unpack_from(data, secttablepos + image_section_header.size * i)
			sectionname = sectionheader[0].decode('ascii').strip('\0')
			sectionheader = sectionheader[1:]

			print(sectionname + ": " + repr(sectionheader))

			#read section data
			virtual_size, virtual_address, size_of_raw_data, pointer_to_raw_data, pointer_to_relocations, pointer_to_linenumbers, number_of_relocations, number_of_linenumbers, characteristics = sectionheader

			rawdata = data[pointer_to_raw_data:][:virtual_size]

			sections[sectionname] = sectionheader, rawdata

		ressectionheader, self.rsrcdata = sections[SECTION_NAME_RESOURCE]
		self.resdatava = ressectionheader[3]

		self.data = data
		self.dosheader = dosheader
		self.peheader = peheader
		self.optheader = optheader
		self.datadirectory = datadirectory
		self.sections = sections

		#rootnode = pcr_read_rsrc_tree(section offset = ftell(file), raw data offset = ptr - virtaddr)

		self.rootnode = self.read_rsrc_tree(0)
		self.strings = self.parse_rsrc_strings()

	def parse_rsrc_strings(self):
		"""
		returns a dict of dicts, such that
			self.parse_rsrc_strings()[languageid][stringid] == string
		"""

		result = defaultdict(lambda: {})

		stringrestype = restypesinv['string']
		if stringrestype not in self.rootnode:
			return result
		stringnode = self.rootnode[stringrestype]

		#iterate through all stringtables
		for tableid, languages in stringnode.items():
			#yeah, counting from 1 is a perfectly great idea, microsoft.
			baseidx = (tableid - 1) * STRINGTABLE_SIZE

			#the stringtable might exist in multiple languages
			for langid, stringtable in languages.items():
				langcode = langcodes[langid]

				#parse the stringtable

				#position is in bytes from start of this stringtable
				pos = 0

				#the stringtable contains STRINGS_PER_STRINGTABLE entries,
				#which are just (little-endian short) length + length * 2 bytes of little-endian UTF-16
				for tableidx in range(STRINGTABLE_SIZE):
					stringlen, = stringtable_length_specifier.unpack_from(stringtable, pos)
					pos += 2
					stringend = pos + STRINGTABLE_LENGTH_MULTIPLIER * stringlen
					string = stringtable[pos:stringend].decode(STRINGTABLE_ENCODING)
					pos = stringend

					if len(string) > 0:
						result[langcode][baseidx + tableidx] = string

				if pos != len(stringtable):
					raise Exception("stringtable invalid: " + str((tableid, langid)))

		return result

	def rsrc_strings_to_textfile(self, lang):
		result = ""
		for stringid, string in sorted(self.strings[lang].items()):
			result += str(stringid) + ':' + lang + ':' + str(string.count('\n') + 1) + '\n'
			result += string + '\n'
		return result

	def read_rsrc_tree(self, pos, recdepth = 0):
		"""
		reads a resource directory
		note that the directory may contain subdirectories, in which case the function
		is called recursively

		pos
			position of directory in rsrc section
		returns
			resource directory structure dict
		"""
		rdir = resource_directory_table.unpack_from(self.rsrcdata, pos)
		characteristics, timestamp, maj_ver, min_ver, name_entry_count, id_entry_count = rdir
		pos += resource_directory_table.size

		entries = {}
		for i in range(name_entry_count + id_entry_count):
			name, rva = resource_directory_entry.unpack_from(self.rsrcdata, pos)
			pos += resource_directory_entry.size

			#process name
			if i < name_entry_count:
				#TODO get name from RVA entry[0]
				name = 'name@' + str(name)

			#process rva
			if rva & 2**31:
				#print(recdepth * "  " + "dir:" + str(name))
				rva -= 2**31
				#rva points to a subdirectory
				entry = self.read_rsrc_tree(rva, recdepth + 1)
			else:
				dataentry = resource_data_entry.unpack_from(self.rsrcdata, rva)
				data_rva, size, codepage, _ = dataentry
				data_absa = data_rva - self.resdatava
				#rva points to a leaf node
				entry = self.rsrcdata[data_absa:data_absa + size]
				#print(recdepth * "  " + "leaf:" + str(name) + ", cp: " + str(codepage) + ", size: " + str(size) + ", addr = " + str(data_rva) + ", absa = " + str(data_absa))

			entries[name] = entry


		return entries

	def rsrc_tree_to_dir(self, dirname, node = None):
		"""
		writes the ressource tree to the filesystem as a directory tree
		"""
		if node == None:
			node = self.rootnode

		for name, e in node.items():
			if type(e) == dict:
				try:
					os.makedirs(dirname + '/' + str(name))
				except FileExistsError:
					pass
				self.restodir(dirname + '/' + str(name), e)
			else:
				with open(dirname + '/' + str(name), 'wb') as f:
					f.write(e)

#example usage #TODO call from convert main.py
pe = PEFile("/tmp/language.dll")
rsrc_tree_to_dir('/tmp/language.dll.rsrcs')
#f = open('/tmp/strings_en', 'w')
#f.write(pe.rsrc_strings_to_textfile('de_DE'))
