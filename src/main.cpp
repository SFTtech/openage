#include <getopt.h>

#include "openage/main.h"
#include "engine/log.h"
#include "engine/util/error.h"

int main(int argc, char **argv) {

	bool run_game = true;


	// ===== argument parsing
	int c;

	while (true) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help",    no_argument,       0, 'h'},
			{"test",    required_argument, 0, 't'},
			{0,         0,                 0,  0 }
		};

		c = getopt_long(argc, argv, "ht:", long_options, &option_index);

		if (c == -1) {
			break;
		}

		switch (c) {
			/*
		case 0:
			engine::log::msg("long option %s", long_options[option_index].name);
			if (optarg) {
				engine::log::msg(" with arg %s", optarg);
			}
			engine::log::msg("\n");
			break;
			*/

		case 'h':
			run_game = false;

			#define PROJECT_NAME "openage"
			engine::log::msg(PROJECT_NAME " - a free (as in freedom) real time strategy game\n"
			       "\n"
			       "usage:\n"
			       "   " PROJECT_NAME " [OPTION]\n"
			       "available options:\n"
			       "-h, --help                 display this help\n"
			       "-t, --test=TESTNAME        run the given test\n"
			       "\n"
			       );
			break;

		case 't':
			run_game = false;

			engine::log::msg("should run test '%s' nao\n", optarg);
			break;

		case '?':
			run_game = false;
			break;

		default:
			engine::log::err("?? getopt returned character code 0x%04x, wtf?\n", c);
		}
	}

	//more arguments than processed options
	if (optind < argc) {
		run_game = false;

		engine::log::err("got unknown additional parameters: ");
		int i = 0;
		while (optind < argc) {
			engine::log::err("%d: %s ", i, argv[optind]);
			i += 1;
			optind += 1;
		}
	}
	// ===== end argument parsing

	try {
		if (run_game) {
			return openage::main();
		}
	} catch (engine::Error e) {
		engine::log::fatal("Exception: %s", e.str());
		return 1;
	}
}
