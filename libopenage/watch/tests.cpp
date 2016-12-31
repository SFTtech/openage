// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "watch.h"

#include <memory>

namespace openage {
namespace watch {
namespace tests {

void run() {
	std::unique_ptr<WatchManager> manager = WatchManager::create();

	manager->watch_file("/etc/passwd", [](event_type, const std::string&) { } );

	manager->check_changes();
}

}}} // openage::watch::tests
