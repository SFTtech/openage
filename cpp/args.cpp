// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#include "args.h"

#include <iostream>
#include <getopt.h>
#include <string.h>

#include "log/log.h"
#include "util/error.h"
#include "util/strings.h"

#include "main.h"

namespace openage {

void print_usage() {
	std::cout << PROJECT_NAME " - a free (as in freedom) real time strategy game" << std::endl;
	std::cout << std::endl;
	std::cout << "usage:" << std::endl;
	std::cout << "    " PROJECT_NAME " [OPTION]" << std::endl;
	std::cout << "available options:" << std::endl;
	std::cout << "-h, --help                              display this help" << std::endl;
	std::cout << "-t, --test=NAME                         run test NAME" << std::endl;
	std::cout << "-d, --demo=NAME [arg [arg [...]]]       run demo NAME" << std::endl;
	std::cout << "--list-tests                            print a list of all available tests" << std::endl;
	std::cout << "--version                               print the openage version to the console" << std::endl;
	std::cout << "--data=FOLDER                           specify the data folder" << std::endl;
	std::cout << std::endl << std::endl << std::endl;
}

Arguments::Arguments()
	:
	argc{0},
	argv{nullptr},
	data_directory{"./assets"},
	demo_specified{false},
	demo_argc{0},
	demo_argv{nullptr},
	list_tests{false},
	version{false},
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
			{"version",              no_argument, 0,  0 },
			{0,                                0, 0,  0 }
		};

		int c = getopt_long(ret.argc, ret.argv, "htd:", long_options, &option_index);

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
					log::log(MSG(info) << "data folder will be " << optarg);
					ret.data_directory = optarg;
				} else {
					handled = false;
				}
			} else {
				// without arg
				if (0 == strcmp("list-tests", opt_name)) {
					ret.list_tests = true;
				} else if (0 == strcmp("version", opt_name)) {
					ret.version = true;
				} else {
					handled = false;
				}
			}

			if (not handled) {
				ret.error_occured = true;
				throw util::Error(MSG(err) << "Internal error: argument registered but unhandled: " << opt_name);
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
				throw util::Error(MSG(err) << "--demo may be used only once");
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
			throw util::Error(MSG(err) << "Unknown argument: " << ret.argv[optind - 1]);
			break;

		default:
			ret.error_occured = true;
			throw util::Error(MSG(err) <<
				"getopt returned code 0x" <<
				std::setw(2) << std::hex << c << ", wtf?");

			break;
		}
	}

	if (optind < ret.argc and not aborted) {
		//more arguments than processed options
		ret.error_occured = true;
		throw util::Error(MSG(err) <<
			(ret.argc - optind) << " unknown additional arguments, starting with " <<
			ret.argv[optind]);
	}

	return ret;
}

} //namespace openage
