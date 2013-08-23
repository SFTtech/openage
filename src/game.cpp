#include "game.h"

#include "engine/engine.h"

int main() {
	return openage::main();
}

namespace openage {

int main() {
	return engine::main();
}

} //namespace openage
