// Copyright 2013-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>

#include <stddef.h>

#include "../error/error.h"

#include "compiler.h"
#include "dir.h"

#define ssize_t long

namespace openage {
namespace util {

ssize_t file_size(const std::string &filename);
ssize_t file_size(Dir basedir, const std::string &fname);

ssize_t read_whole_file(char **result, const char *filename);
ssize_t read_whole_file(char **result, const std::string &filename);

/**
 * get the lines of a file.
 *
 * returns vector of strings, each entry is one line in the file.
 */
std::vector<std::string> file_get_lines(const std::string &file_name);


/**
 * read a single csv file.
 * call the destination struct .fill() method for actually storing line data
 */
template<typename lineformat>
void read_csv_file(const std::string &fname, std::vector<lineformat> &out) {
	size_t line_count = 0;
	lineformat current_line_data;
	std::string line;
	std::vector<char> strbuf;

	std::ifstream csvfile{fname};

	while (std::getline(csvfile, line)) {
		line_count += 1;

		// ignore comments and empty lines
		if (line.empty() || line[0] == '#') {
			continue;
		}

		// create writable copy, for tokenisation
		if (unlikely(strbuf.size() <= line.size())) {
			strbuf.resize(line.size() + 1);
		}
		memcpy(strbuf.data(), line.c_str(), line.size() + 1);

		// use the line copy to fill the current line struct.
		int error_column = current_line_data.fill(strbuf.data());
		if (error_column != -1) {
			throw Error(MSG(err) <<
				"Failed to read CSV file: " <<
				fname << ":" << line_count << ":" << error_column << ": "
				"error parsing " << line);
		}

		out.push_back(current_line_data);
	}

	if (unlikely(!line_count)) {
		throw Error(MSG(err) <<
			"Failed to read CSV file " << fname << ": " <<
			"Empty or missing.");
	}
}

/**
 * reads data files recursively.
 * should be called from the .recurse() method of the struct.
 */
template<class lineformat>
std::vector<lineformat> recurse_data_files(Dir basedir, const std::string &fname) {
	std::vector<lineformat> result;
	std::string merged_filename = basedir.join(fname);

	read_csv_file<lineformat>(merged_filename, result);

	//the new basedir is the old basedir
	// + the directory part of the current relative file name
	Dir new_basedir = basedir.append(dirname(fname));

	size_t line_count = 0;
	for (auto &entry : result) {
		line_count += 1;
		if (not entry.recurse(new_basedir)) {
			throw Error(MSG(err) <<
				"Failed to read follow-up files for " <<
				merged_filename << ":" << line_count);
		}
	}

	return result;
}


/**
 * referenced file tree structure.
 *
 * used to store the filename and resulting data of a file down
 * the gamedata tree.
 */
template<class cls>
struct subdata {
	std::string filename;
	std::vector<cls> data;

	bool read(Dir basedir) {
		this->data = recurse_data_files<cls>(basedir, this->filename);
		return true;
	}

	cls operator [](size_t idx) const {
		return this->data[idx];
	}
};


}} // openage::util
