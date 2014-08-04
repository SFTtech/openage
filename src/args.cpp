#include "args.h"

#include <getopt.h>
#include <string.h>

#include "log.h"
#include "util/error.h"
#include "util/strings.h"

#include "main.h"

namespace openage {

void print_usage() {
	log::msg(
		PROJECT_NAME " - a free (as in freedom) real time strategy game\n"
		"\n"
		"usage:\n"
		"   " PROJECT_NAME " [OPTION]\n"
		"available options:\n"
		"-h, --help                              display this help\n"
		"-t, --test=PATTERN[,arg0[,arg1[...]]]   run the tests matching PATTERN\n"
		"--no-interactive-tests                  run only automated tests\n"
		"--list-tests                            print a list of all available tests\n"
		"--data=FOLDER                           specify the data folder\n"
		"\n"
		""
		"\n\n"
	);
}

Arguments::Arguments()
	:
	argc(0),
	data_directory("./"),
	disable_interactive_tests(false),
	list_tests(false)
{}

Arguments::~Arguments() {}

void Arguments::add_test_invocation(const char *arg) {
	size_t len = strlen(arg);
	char *argbuf = new char[len + 1];
	memcpy(argbuf, arg, len + 1);

	char **argv;
	int argc = util::string_tokenize_dynamic(argbuf, ',', &argv);

	this->test_invocations.push_back(test_invocation{argc, argv});
}

Arguments parse_args(int argc, char **argv) {
	Arguments ret;
	int c;

	ret.argc = argc;
	ret.argv = argv;

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

		c = getopt_long(ret.argc, ret.argv, "ht:", long_options, &option_index);

		if (c == -1) {
			break;
		}

		switch (c) {
		case 0: {
			const char *opt_name = long_options[option_index].name;
			bool handled = true;

			if (optarg) {
				//with arg
				if (0 == strcmp("data", opt_name)) {
					log::msg("data folder will be %s", optarg);
					ret.data_directory = optarg;
				} else {
					handled = false;
				}
			} else {
				//without arg
				if (0 == strcmp("no-interactive-tests", opt_name)) {
					log::msg("disabling interactive tests");
					ret.disable_interactive_tests = true;
				} else if (0 == strcmp("list-tests", opt_name)) {
					ret.list_tests = true;
				} else {
					handled = false;
				}
			}

			if (not handled) {
				throw util::Error("internal error: argument %s registered but unhandled", opt_name);
			}

			break;
		}
		case 'h':
			print_usage();
			break;

		case 't':
			ret.add_test_invocation(optarg);
			break;

		case '?':
			print_usage();
			throw util::Error("unknown argument: %s", ret.argv[optind - 1]);
			break;

		default:
			throw util::Error("getopt returned code 0x%02x, wtf?", c);
			break;
		}
	}

	if (optind < ret.argc) {
		//more arguments than processed options
		throw util::Error("%d unknown additional arguments, starting with %s",
		            ret.argc - optind, ret.argv[optind]);
	}

	return ret;
}

} //namespace openage
