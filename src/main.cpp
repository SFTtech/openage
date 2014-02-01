#include "openage/main.h"
#include "engine/log.h"
#include "engine/util/error.h"

#include "engine/console/buf.h"
using namespace engine::console;
using namespace engine::coord;

void test() {
	Buf buf{{80, 25}, 1337, 80};
	buf.write("Hello, brave new console world!\n\n\n\n");
	buf.write("stuff, lol.\n\n");
	buf.write("\x1b[1mbold stuff, lol.\x1b[m\n\n");
	buf.write("\x1b[5;31;1mredbold stuff, lol.\x1b[m\n\n");
	for (int i = 0; i < 18; i++) {
		buf.write("asdf\n");
	}
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
