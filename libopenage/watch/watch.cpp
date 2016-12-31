// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "watch.h"

#include "linux.h"

namespace openage {
namespace watch {

std::unique_ptr<WatchManager> WatchManager::create() {
#if WITH_INOTIFY
	return std::make_unique<linux::WatchManager>();
#else
	return std::make_unique<DummyWatchManager>();
#endif
}

void DummyWatchManager::check_changes() {}

void DummyWatchManager::watch_file(const std::string &, const callback_t &) {}



}} // openage::watch
