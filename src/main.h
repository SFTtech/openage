#ifndef _MAIN_H_
#define _MAIN_H_

#include <vector>

struct test_invocation {
	int argc;
	char **argv;
};

struct program_config_struct {
	int argc;
	char **argv;

	bool run_game = true;
	const char *data_directory = "./";
	std::vector<test_invocation> test_invocations;
	bool disable_interactive_tests = false;
	bool list_tests = false;
};

extern program_config_struct program_config;

#endif //_MAIN_H_
