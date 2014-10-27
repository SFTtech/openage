#ifndef OPENAGE_ARGS_H_5F69D09A02864DE78EE4C944F382EAD6
#define OPENAGE_ARGS_H_5F69D09A02864DE78EE4C944F382EAD6

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
	bool display_help;
	bool error_occured;
};

Arguments parse_args(int argc, char **argv);

} //namespace openage

#endif //_ARGS_H_
