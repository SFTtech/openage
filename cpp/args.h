// Copyright 2014-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_ARGS_H_
#define OPENAGE_ARGS_H_

#include <vector>
#include <string>

namespace openage {

struct test_invocation {
	int argc;
	char **argv;
};

class Arguments {
public:
	Arguments();
	~Arguments();

	int argc;
	char **argv;

	const char *data_directory;
	std::vector<std::string> tests;

	bool demo_specified;
	std::string demo;
	int demo_argc;
	char **demo_argv;

	bool list_tests;
	bool version;
	bool display_help;
	bool error_occured;
	bool load_gamedata;
};

Arguments parse_args(int argc, char **argv);

} //namespace openage

#endif
