#include <getopt.h>
#include <string.h>

#include "openage/main.h"
#include "engine/log.h"
#include "engine/util/error.h"

using namespace engine;


int main(int argc, char **argv) {

	bool run_game = true;


	//default values for arguments

	const char *data_directory = "./";


	// ===== argument parsing
	int c;

	while (true) {
		int option_index = 0;
		static struct option long_options[] = {
			{"help",          no_argument, 0, 'h'},
			{"test",    required_argument, 0, 't'},
			{"data",    required_argument, 0,  0 },
			{0,                         0, 0,  0 }
		};

		c = getopt_long(argc, argv, "ht:", long_options, &option_index);

		if (c == -1) {
			break;
		}

		switch (c) {
		case 0: {
			const char *opt_name = long_options[option_index].name;

			if (optarg) {
				if (0 == strcmp("data", opt_name)) {
					log::msg("data folder will be %s", optarg);
					data_directory = optarg;
				}
			}
			else {
				//long opts without arg
			}

			break;
		}
		case 'h':
			run_game = false;

			log::msg(PROJECT_NAME " - a free (as in freedom) real time strategy game\n"
			         "\n"
			         "usage:\n"
			         "   " PROJECT_NAME " [OPTION]\n"
			         "available options:\n"
			         "-h, --help                 display this help\n"
			         "-t, --test=TESTNAME        run the given test\n"
			         "--data=FOLDER              specify the data folder\n"
			         "\n"
			         );
			break;

		case 't':
			run_game = false;

			log::msg("should run test '%s' nao", optarg);
			break;

		case '?':
			run_game = false;
			break;

		default:
			log::err("?? getopt returned character code 0x%04x, wtf?", c);
		}
	}

	//more arguments than processed options
	if (optind < argc) {
		run_game = false;

		log::err("got unknown additional parameters: ");
		int i = 0;
		while (optind < argc) {
			log::err("%d: %s ", i, argv[optind]);
			i += 1;
			optind += 1;
		}
	}
	// ===== end argument parsing


	try {
		if (run_game) {
			log::msg("launching " PROJECT_NAME);
			return openage::main(data_directory);
		}
	} catch (engine::Error e) {
		log::fatal("Exception: %s", e.str());
		return 1;
	}
}
