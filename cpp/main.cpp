#include "main.h"

#include <stdlib.h>
#include <time.h>

#include "args.h"
#include "log.h"
#include "util/error.h"
#include "testing/engine_test.h"
#include "testing/testing.h"
#include "testing/register.h"


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

		testing::register_all_tests();

		if (args.list_tests) {
			testing::list_tests();
			return 0;
		}

		// set global random seed
		srand(time(NULL));

		// shall we run a test?
		if (args.test_invocations.size() > 0) {
			bool result = true;
			for (test_invocation &ti : args.test_invocations) {
				result = result and testing::run_tests(ti.argv[0], args.disable_interactive_tests, ti.argc, ti.argv);
			}
			return (result == false) ? 1 : 0;
		}
		else {
			return testing::run_game(&args);
		}

		return 0;
	} catch (util::Error e) {
		log::fatal("Exception: %s", e.str());
		return 1;
	}
}
