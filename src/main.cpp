#include "openage/main.h"
#include "engine/util/error.h"
#include "engine/util/log.h"

int main() {
        try {
                return openage::mainmethod();
        } catch (engine::Error e) {
                engine::log::fatal("Exception: %s", e.str());
                return 1;
        }
}
