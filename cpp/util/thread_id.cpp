// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#include "thread_id.h"

namespace openage {
namespace util {


ThreadIdSupplier::ThreadIdSupplier()
	:
	val{ThreadIdSupplier::counting_id++} {}


std::atomic<unsigned> ThreadIdSupplier::counting_id{0};

const thread_local ThreadIdSupplier current_thread_id;


}} // namespace openage::util
