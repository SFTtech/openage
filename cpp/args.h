#ifndef _ARGS_H_
#define _ARGS_H_

#include <vector>

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
	std::vector<test_invocation> test_invocations;
	bool disable_interactive_tests;
	bool list_tests;

	void add_test_invocation(const char *arg);
};

Arguments parse_args(int argc, char **argv);

} //namespace openage

#endif //_ARGS_H_
