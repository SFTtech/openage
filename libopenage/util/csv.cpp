// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#include "csv.h"

#include <cstring>

#include "file.h"
#include "../error/error.h"
#include "../log/log.h"


namespace openage {
namespace util {


CSVCollection::CSVCollection(const Path &entryfile_path) {

	auto file = entryfile_path.open();
	log::log(DBG << "Loading csv collection: " << file);

	// The file format is defined in:
	// openage/convert/dataformat/data_definition.py
	// example:
	//
	// ### some/folder/and/filename.docx
	// # irrelevant
	// # comments
	// data,stuff,moar,bla

	// store the currently read file name
	std::string current_file;

	for (auto &line : file.get_lines()) {
		// a new file starts:
		if (line[0] == '#' and
		    line[1] == '#' and
		    line[2] == '#' and
		    line[3] == ' ') {

			// remove the "### "
			current_file = (line.erase(0, 4));

			// create a vector to put lines in
			this->data.emplace(current_file, std::vector<std::string>{});
		}
		else {
			if (line.empty() or line[0] == '#') {
				continue;
			}

			if (likely(current_file.size() > 0)) {
				// add line to the current file linelist
				this->data.at(current_file).push_back(line);
			}
			else {
				throw Error{
					ERR << "csv collection content encountered "
					    << "without known target in " << entryfile_path
					    << ", linedata: " << line
				};
			}
		}
	}

	log::log(INFO << "Loaded multi-csv file: "
	         << this->data.size() << " sub-files");
}

}} // openage::util
