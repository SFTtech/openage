#include "main.h"

#include <getopt.h>
#include <string.h>

#include "openage/main.h"
#include "engine/log.h"
#include "engine/util/error.h"
#include "testing/testing.h"
#include "testing/register.h"

#include "args.h"

using namespace engine;

int main(int argc, char **argv) {
	args.argc = argc;
	args.argv = argv;

	try {
		parse_args();

		if (args.list_tests || args.test_invocations.size() > 0) {
			testing::register_all_tests();
		}

		if (args.list_tests) {
			testing::list_tests();
		}

		if (args.test_invocations.size() > 0) {
			log::msg("running tests...");

			for(test_invocation &ti : args.test_invocations) {
				testing::run_tests(ti.argv[0], args.disable_interactive_tests, ti.argc, ti.argv);
			}
		}

		if (args.run_game) {
			log::msg("launching " PROJECT_NAME);
			return openage::main();
		}
	} catch (engine::Error e) {
		log::fatal("Exception: %s", e.str());
		return 1;
	}
}
