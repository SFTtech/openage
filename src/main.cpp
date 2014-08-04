#include "main.h"

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

		testing::register_all_tests();

		if (args.list_tests) {
			testing::list_tests();
		}

		if (args.test_invocations.size() > 0) {
			for (test_invocation &ti : args.test_invocations) {
				testing::run_tests(ti.argv[0], args.disable_interactive_tests, ti.argc, ti.argv);
			}
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
