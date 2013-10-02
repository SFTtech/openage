#include "filetools.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "../log/log.h"

namespace openage {
namespace util {

char* read_whole_file(const char *filename) {

	//get the file size
	struct stat st;
	if (stat(filename, &st) < 0) {
		log::fatal("failed getting filesize of %s", filename);
		exit(1);
	}

	//open the file
	FILE *filehandle = fopen(filename, "r");
	if (filehandle == NULL) {
		log::fatal("failed opening file %s", filename);
		exit(1);
	}

	//read the whole content
	char *str = new char[st.st_size + 1];
	if (((unsigned long) st.st_size) != fread(str, 1, st.st_size, filehandle)) {
		log::fatal("failed reading the file %s", filename);
		exit(1);
	}

	str[(size_t) st.st_size] = '\0';

	//return the file contents
	return str;
}

} //namespace util
} //namespace openage
