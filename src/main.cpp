#include "openage/main.h"
#include "engine/log.h"
#include "engine/util/error.h"

#include "engine/console/buf.h"
using namespace engine::console;
using namespace engine::coord;

void test() {
	Buf buf{{80, 25}, 1337, 80};
	buf.write("Hello, brave new console world!\n\n");
	buf.write("Stuff, lol.\n");
	buf.write("\x1b[1mto boldly write a terminal\x1b[m");
	buf.write("\n\nlololo\n");
	buf.to_stdout();
}

int main() {
	test();
	return 0;

	try {
		return openage::main();
	} catch (engine::Error e) {
		engine::log::fatal("Exception: %s", e.str());
		return 1;
	}
}
