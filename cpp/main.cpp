// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#include "main.h"

#include <stdlib.h>
#include <time.h>

#include "args.h"
#include "log.h"
#include "util/error.h"
#include "game.h"
#include "testing/testing.h"

#include "config.h"


using namespace openage;

int main(int argc, char **argv) {
	try {
		Arguments args = parse_args(argc, argv);

		if (args.display_help) {
			if (args.error_occured) {
				return 1;
			} else {
				return 0;
			}
		}

		if (args.list_tests) {
			testing::list_tests();
			return 0;
		}

		if (args.version) {
			printf("openage %s\n%s\n",
			       config::version,
			       config::config_option_string);

			return 0;
		}

		// set global random seed
		srand(time(NULL));

		// shall we run a test?
		if (args.tests.empty() and not args.demo_specified) {
			return run_game(&args);
		}

		bool result = true;
		for (auto &ti : args.tests) {
			if (!testing::run_test(ti)) {
				result = false;
			}
		}

		if (args.demo_specified) {
			if (!testing::run_demo(args.demo, args.demo_argc, args.demo_argv)) {
				result = false;
			};
		}

		return (result == false) ? 1 : 0;

	} catch (util::Error e) {
		log::fatal("Exception: %s", e.str());
		return 1;
	}
}
