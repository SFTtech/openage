#include "openage/main.h"
#include "engine/log.h"
#include "engine/util/error.h"

int main() {
	try {
		return openage::main();
	} catch (engine::Error e) {
		engine::log::fatal("Exception: %s", e.str());
		return 1;
	}
}
