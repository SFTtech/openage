// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#include "lzxd.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "util.h"

class STDIOWrapper: public IOWrapper {
public:
	virtual ssize_t read(void *buf, size_t size) {
		return fread(buf, 1, size, stdin);
	}

	virtual ssize_t write(const void *buf, size_t size) {
		return fwrite(buf, 1, size, stdout);
	}
};

void run(unsigned windowsize, unsigned outputsize) {
	/*
	 * arguments:
	 *   window size: constant for one lzx stream; given in the cab header (as part of the compression algorithm id)
	 *   reset interval: 0 for cab (otherwise, streamstate would be reset regularily, allowing seeking)
	 *   input_bufsize: guess what. anything >= 2 should be OK. original code used 4096.
	 *   outputsize: size of uncompressed output, in bytes, needed for E8 postprocessing.
	 *   iowrapper: feeds decompressor/gets output
	 */
	decompress(windowsize, 0, 4096, outputsize, new STDIOWrapper{} /* whee! memory leeks! I don't give a fuck! */);
}

unsigned toint(char *str, unsigned min = 0, unsigned max = 0xffffffff) {
	char *endptr = str;
	long int res = strtoll(str, &endptr, 10);
	if (*endptr != '\0') {
		throwerr("not a valid integer: %s", str);
	}
	if (res < (long int) min) {
		throwerr("must be greater than %u: %s", min, str);
	}
	if (res > (long int) max) {
		throwerr("must be smaller than %u: %s", max, str);
	}
	return (unsigned) res;
}

int main(int argc, char **argv) {
	try {
		if (argc != 3) {
			throwerr("usage: %s windowsize outputsize", argv[0]);
		}
		// windowsize is between 15 and 21, and comes from the file compression type in the cab file, shifted >> 8.
		unsigned long int windowsize = toint(argv[1], 15, 21);
		// outputsize is the expected decompressed output size. it is available from the cabfile metadata.
		unsigned long int outputsize = toint(argv[2]);
		run(windowsize, outputsize);
		return 0;
	} catch (const char *s) {
		fprintf(stderr, "\x1b[31;1mERROR\x1b[m %s\n", s);
		return 1;
	}
}
