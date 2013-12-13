#include "file.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "../log.h"
#include "error.h"

namespace engine {
namespace util {


char *read_whole_file(const char *filename) {
	auto file = read_whole_file_s(filename);
	char *ret = new char[file->size];
	memcpy(ret, file->content, file->size);
	delete file;
	return ret;
}

struct file_data *read_whole_file_s(const char *filename) {

	//get the file size
	struct stat st;
	if (stat(filename, &st) < 0) {
		throw Error("failed getting filesize of %s", filename);
	}

	//open the file
	FILE *filehandle = fopen(filename, "r");
	if (filehandle == NULL) {
		throw Error("failed opening file %s", filename);
	}

	size_t content_length = (size_t) st.st_size;

	//read the whole content
	char *str = new char[content_length + 1];
	if (content_length != fread(str, 1, st.st_size, filehandle)) {
		throw Error("failed reading the file %s", filename);
	}

	//make sure 0-byte is at the end
	str[content_length] = '\0';

	//return the file contents
	struct file_data *ret = new struct file_data(content_length, str);
	return ret;
}


} //namespace util
} //namespace engine
