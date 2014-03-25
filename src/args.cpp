#include "args.h"

#include <getopt.h>
#include <string.h>

#include "engine/log.h"
#include "engine/util/error.h"
#include "engine/util/strings.h"

#include "main.h"

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

args_struct args;

void add_test_invocation(const char *arg) {
	size_t len = strlen(arg);
	char *argbuf = new char[len + 1];
	memcpy(argbuf, arg, len + 1);

	char **argv;
	int argc = util::string_tokenize_dynamic(argbuf, ',', &argv);

	args.test_invocations.push_back(test_invocation{argc, argv});
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

		c = getopt_long(args.argc, args.argv,
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
					args.data_directory = optarg;
				} else {
					throw Error("internal error: argument %s registered, but unhandled", opt_name);
				}
			} else {
				//without arg
				if (0 == strcmp("no-interactive-tests", opt_name)) {
					log::msg("disabling interactive tests");
					args.disable_interactive_tests = true;
				} else if (0 == strcmp("list-tests", opt_name)) {
					args.run_game = false;
					args.list_tests = true;
				} else {
					throw Error("internal error: argument %s registered, but unhandled", opt_name);
				}
			}

			break;
		}
		case 'h':
			args.run_game = false;
			print_usage();
			break;

		case 't':
			args.run_game = false;
			add_test_invocation(optarg);
			break;

		case '?':
			print_usage();
			throw Error("unknown argument: %s", args.argv[optind - 1]);
			break;

		default:
			throw Error("getopt returned code 0x%02x, wtf?", c);
			break;
		}
	}

	if (optind < args.argc) {
		//more arguments than processed options
		throw Error("%d unknown additional arguments, starting with %s",
		 args.argc - optind, args.argv[optind]);

	}
}
