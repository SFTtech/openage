// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../error/error.h"

#include "compiler.h"
#include "dir.h"

namespace openage {
namespace util {


/**
 * Type for storing csv data:
 * {filename: [line, ...]}.
 */
using csv_file_map_t = std::unordered_map<std::string, std::vector<std::string>>;


/**
 * Load a multi csv file into a csv_file_map_t
 */
csv_file_map_t load_multi_csv_file(Dir basedir, const std::string &fname);


/**
 * read a single csv file.
 * call the destination struct .fill() method for actually storing line data
 */
template<typename lineformat>
void read_csv_file(const std::string &fname, std::vector<lineformat> &out, csv_file_map_t *file_map=nullptr) {
	size_t line_count = 0;
	lineformat current_line_data;
	std::vector<char> strbuf;

	// instead of reading from the file, use the file_map as buffer.
	if (file_map && file_map->count(fname)) {
		std::vector<std::string> lines = file_map->at(fname);

		for (auto &line : lines) {
			line_count += 1;

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
	}
	else {
		std::string line;

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
}


/**
 * reads data files recursively.
 * should be called from the .recurse() method of the struct.
 */
template<class lineformat>
std::vector<lineformat> recurse_data_files(Dir basedir, const std::string &fname, csv_file_map_t *file_map=nullptr) {
	std::vector<lineformat> result;
	std::string merged_filename = basedir.join(fname);

	read_csv_file<lineformat>(merged_filename, result, file_map);

	//the new basedir is the old basedir
	// + the directory part of the current relative file name
	Dir new_basedir = basedir.append(dirname(fname));

	size_t line_count = 0;
	for (auto &entry : result) {
		line_count += 1;
		if (not entry.recurse(new_basedir, file_map)) {
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

	bool read(Dir basedir, csv_file_map_t *file_map=nullptr) {
		this->data = recurse_data_files<cls>(basedir, this->filename, file_map);
		return true;
	}

	cls operator [](size_t idx) const {
		return this->data[idx];
	}
};


}} // openage::util
