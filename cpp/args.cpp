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
		"-t, --test=NAME                         run test NAME\n"
		"-d, --demo=NAME [arg [arg [...]]]       run demo NAME\n"
		"--list-tests                            print a list of all available tests\n"
		"--data=FOLDER                           specify the data folder\n"
		"\n"
		""
		"\n\n"
	);
}

Arguments::Arguments()
	:
	argc{0},
	argv{nullptr},
	data_directory{"./"},
	demo_specified{false},
	demo_argc{0},
	demo_argv{nullptr},
	list_tests{false},
	display_help{false},
	error_occured{false} {
}

Arguments::~Arguments() {}

Arguments parse_args(int argc, char **argv) {
	Arguments ret;
	ret.argc = argc;
	ret.argv = argv;

	bool aborted = false;
	while (not aborted) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help",                 no_argument, 0, 'h'},
			{"test",           required_argument, 0, 't'},
			{"demo",           required_argument, 0, 'd'},
			{"data",           required_argument, 0,  0 },
			{"list-tests",           no_argument, 0,  0 },
			{0,                                0, 0,  0 }
		};

		int c = getopt_long(ret.argc, ret.argv, "ht:", long_options, &option_index);

		if (c == -1) {
			break;
		}

		switch (c) {
		case 0: {
			const char *opt_name = long_options[option_index].name;
			bool handled = true;

			if (optarg) {
				// with arg
				if (0 == strcmp("data", opt_name)) {
					log::msg("data folder will be %s", optarg);
					ret.data_directory = optarg;
				} else {
					handled = false;
				}
			} else {
				// without arg
				if (0 == strcmp("list-tests", opt_name)) {
					ret.list_tests = true;
				} else {
					handled = false;
				}
			}

			if (not handled) {
				ret.error_occured = true;
				throw util::Error("internal error: argument %s registered but unhandled", opt_name);
			}

			break;
		}
		case 'h':
			print_usage();
			ret.display_help = true;
			break;

		case 't':
			ret.tests.push_back(optarg);
			break;

		case 'd':
			if (ret.demo_specified) {
				throw util::Error("--demo may be used only once");
			}

			ret.demo_specified = true;
			ret.demo = optarg;
			ret.demo_argc = argc + 1 - optind;
			ret.demo_argv = &argv[optind - 1];
			aborted = true;
			break;

		case '?':
			print_usage();
			ret.error_occured = true;
			throw util::Error("unknown argument: %s", ret.argv[optind - 1]);
			break;

		default:
			ret.error_occured = true;
			throw util::Error("getopt returned code 0x%02x, wtf?", c);
			break;
		}
	}

	if (optind < ret.argc and not aborted) {
		//more arguments than processed options
		ret.error_occured = true;
		throw util::Error("%d unknown additional arguments, starting with %s",
		                  ret.argc - optind, ret.argv[optind]);
	}

	return ret;
}

} //namespace openage
