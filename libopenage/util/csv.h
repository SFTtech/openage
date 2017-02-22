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
#include "fslike/native.h"
#include "path.h"


namespace openage {
namespace util {


/**
 * Collection of multiple csv files.
 * Read from a packed csv that contains all the data.
 *
 * Then, data can be read recursively.
 */
class CSVCollection {
public:

	/**
	 * Type for storing csv data:
	 * {filename: [line, ...]}.
	 */
	using csv_file_map_t = std::unordered_map<std::string, std::vector<std::string>>;

	/**
	 * Initialize the collection by reading the given file.
	 * This file must contain the data that this collection is made up of.
	 */
	explicit CSVCollection(const Path &entryfile);
	virtual ~CSVCollection() = default;


	/**
	 * This function is the entry point to load the whole file tree recursively.
	 *
	 * Should be called again from the .recurse() method of the struct.
	 *
	 * The internal flow is as follows:
	 * * read entries of the given files
	 *   (call to the generated field parsers (the fill function))
	 * * then, recurse into referenced subdata entries
	 *   (this implementation is generated)
	 * * from there, reach this function again to read each subdata entry.
	 *
	 */
	template<class lineformat>
	std::vector<lineformat> read(const std::string &filename) const {

		// first read the content from the data
		auto result = this->get_data<lineformat>(filename);

		std::string dir = dirname(filename);

		size_t line_count = 0;

		// then recurse into each subdata entry.
		for (auto &entry : result) {
			line_count += 1;

			if (not entry.recurse(*this, dir)) {
				throw Error{
					MSG(err)
					<< "Failed to read follow-up files for "
					<< filename << ":" << line_count
				};
			}
		}

		return result;
	}


	/**
	 * Parse the data from one file in the map.
	 */
	template<typename lineformat>
	std::vector<lineformat> get_data(const std::string &filename) const {

		size_t line_count = 0;

		std::vector<lineformat> ret;

		// locate the data in the collection
		auto it = this->data.find(filename);

		if (it != std::end(this->data)) {
			const std::vector<std::string> &lines = it->second;

			for (auto &line : lines) {
				line_count += 1;
				lineformat current_line_data;

				// use the line copy to fill the current line struct.
				int error_column = current_line_data.fill(line);
				if (error_column != -1) {
					throw Error{
						ERR
						<< "Failed to read CSV file: "
						<< filename << ":" << line_count << ":" << error_column
						<< ": error parsing " << line
					};
				}

				ret.push_back(current_line_data);
			}
		}
		else {
			throw Error{
				ERR << "File was not found in csv cache: '"
				    << filename << "'"
			};
		}

		return ret;
	}

protected:
	csv_file_map_t data;
};


/**
 * Referenced file tree structure.
 *
 * Used for storing information for subtype members
 * that need to be recursed.
 */
template<class lineformat>
struct csv_subdata {
	/**
	 * File where to read subdata from.
	 * This name is relative to the file that defined the subdata!
	 */
	std::string filename;

	/**
	 * Data that was read from the file with this->filename.
	 */
	std::vector<lineformat> data;

	/**
	 * Read the data of the lineformat from the collection.
	 * Can descend recursively into dependencies.
	 */
	bool read(const CSVCollection &collection, const std::string &basedir) {
		std::string next_file = basedir;
		if (basedir.size() > 0) {
			next_file += fslike::PATHSEP;
		}
		next_file += this->filename;

		this->data = collection.read<lineformat>(next_file);
		return true;
	}

	/**
	 * Convenience operator to access data
	 */
	const lineformat &operator [](size_t idx) const {
		return this->data[idx];
	}
};


/**
 * read a single csv file.
 * call the destination struct .fill() method for actually storing line data
 */
template<typename lineformat>
std::vector<lineformat> read_csv_file(const Path &path) {

	File csv = path.open();

	std::vector<lineformat> ret;
	size_t line_count = 0;

	for (auto &line : csv.get_lines()) {
		line_count += 1;

		// ignore comments and empty lines
		if (line.empty() || line[0] == '#') {
			continue;
		}

		lineformat current_line_data;

		// use the line copy to fill the current line struct.
		int error_column = current_line_data.fill(line);
		if (error_column != -1) {
			throw Error{
				ERR
				<< "Failed to read CSV file: "
				<< csv << ":" << line_count << ":" << error_column
				<< ": error parsing " << line
			};
		}

		ret.push_back(current_line_data);
	}

	return ret;
}

}} // openage::util
