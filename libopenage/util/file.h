// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <string>
#include <vector>

#include "dir.h"
#include "../error/error.h"


namespace openage {
namespace util {


/**
 * Return the size in bytes of a given file name.
 */
ssize_t file_size(const std::string &filename);


/**
 * Return the size in bytes of a filename relative to a directory.
 */
ssize_t file_size(Dir basedir, const std::string &fname);


/**
 * Read the contents of a given filename.
 */
std::string read_whole_file(const std::string &filename);


/**
 * get the lines of a file.
 *
 * returns vector of strings, each entry is one line in the file.
 */
std::vector<std::string> file_get_lines(const std::string &file_name);

}} // openage::util
