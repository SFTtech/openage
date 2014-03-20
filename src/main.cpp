#include "main.h"

#include <getopt.h>
#include <string.h>

#include "openage/main.h"
#include "engine/log.h"
#include "engine/util/error.h"
#include "testing/testing.h"
#include "testing/register.h"

using namespace engine;

void print_usage() {
	log::msg(PROJECT_NAME " - a free (as in freedom) real time strategy game\n"
	 "\n"
	 "usage:\n"
	 "   " PROJECT_NAME " [OPTION]\n"
	 "available options:\n"
	 "-h, --help                              display this help\n"
	 "-t, --test=PATTERN[;arg0[;arg1[...]]]   run the tests matching PATTERN\n"
	 "--no-interactive-tests                  run only automated tests\n"
	 "--list-tests                            print a list of all available tests\n"
	 "--data=FOLDER                           specify the data folder\n"
	 "\n\n"
	);
}

program_config_struct program_config;

void add_test_invocation(const char *arg) {
	size_t len = strlen(arg);
	char *argbuf = new char[len + 1];
	memcpy(argbuf, arg, len + 1);
	std::vector<char *> argvvector;
	char *laststart = argbuf;
	for(char *c = argbuf; *c != '\0'; c++) {
		if (*c == ',') {
			*c = '\0';
			argvvector.push_back(laststart);
		}
	}
	argvvector.push_back(laststart);
	size_t argc = argvvector.size();
	char **argv = new char *[argc];
	for(size_t i = 0; i < argc; i++) {
		argv[i] = argvvector[i];
	}

	program_config.test_invocations.push_back(test_invocation{(int) argc, argv});
}

void parse_args() {
	int c;

	while (true) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help",                 no_argument, 0, 'h'},
			{"test",           required_argument, 0, 't'},
			{"data",           required_argument, 0,  0 },
			{"no-interactive-tests", no_argument, 0,  0 },
			{"list-tests",           no_argument, 0,  0 },
			{0,                                0, 0,  0 }
		};

		c = getopt_long(program_config.argc, program_config.argv,
		 "ht:", long_options, &option_index);

		if (c == -1) {
			break;
		}

		switch (c) {
		case 0: {
			const char *opt_name = long_options[option_index].name;

			if (optarg) {
				//with arg
				log::msg("optarg: %s", optarg);
				if (0 == strcmp("data", opt_name)) {
					log::msg("data folder will be %s", optarg);
					program_config.data_directory = optarg;
				} else {
					throw Error("internal error: argument %s registered, but unhandled", opt_name);
				}
			} else {
				//without arg
				if (0 == strcmp("no-interactive-tests", opt_name)) {
					log::msg("disabling interactive tests");
					program_config.disable_interactive_tests = true;
				} else if (0 == strcmp("list-tests", opt_name)) {
					program_config.run_game = false;
					program_config.list_tests = true;
				} else {
					throw Error("internal error: argument %s registered, but unhandled", opt_name);
				}
			}

			break;
		}
		case 'h':
			program_config.run_game = false;
			print_usage();
			break;

		case 't':
			program_config.run_game = false;
			add_test_invocation(optarg);
			break;

		case '?':
			print_usage();
			throw Error("unknown argument: %s", program_config.argv[optind - 1]);
			break;

		default:
			throw Error("getopt returned code 0x%02x, wtf?", c);
			break;
		}
	}

	if (optind < program_config.argc) {
		//more arguments than processed options
		throw Error("%d unknown additional arguments, starting with %s",
		 program_config.argc - optind, program_config.argv[optind]);

	}
}

int main(int argc, char **argv) {
	program_config.argc = argc;
	program_config.argv = argv;

	try {
		parse_args();

		if (program_config.list_tests || program_config.test_invocations.size() > 0) {
			testing::register_all_tests();
		}

		if (program_config.list_tests) {
			testing::list_tests();
		}

		if (program_config.test_invocations.size() > 0) {
			log::msg("running tests...");

			for(test_invocation &ti : program_config.test_invocations) {
				testing::run_tests(ti.argv[0], program_config.disable_interactive_tests, ti.argc, ti.argv);
			}
		}

		if (program_config.run_game) {
			log::msg("launching " PROJECT_NAME);
			return openage::main(program_config.data_directory);
		}
	} catch (engine::Error e) {
		log::fatal("Exception: %s", e.str());
		return 1;
	}
}
