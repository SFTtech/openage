#ifndef _ARGS_H_
#define _ARGS_H_

#include <vector>

struct test_invocation {
	int argc;
	char **argv;
};

struct args_struct {
	int argc;
	char **argv;

	bool run_game = true;
	const char *data_directory = "./";
	std::vector<test_invocation> test_invocations;
	bool disable_interactive_tests = false;
	bool list_tests = false;
};

extern args_struct args;

void parse_args();

#endif //_ARGS_H_
