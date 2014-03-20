#include "testing.h"

#include "term.h"

namespace testing {

void register_all_tests() {
	register_test("term:0", "prints a few test lines to a buffer, and draws it to stdout.", false, &tests::term0);
	register_test("term:1", "opens an interactive terminal with the user's shell.", true, &tests::term1);
}

}
