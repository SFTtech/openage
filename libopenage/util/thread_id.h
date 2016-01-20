// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <stddef.h>

namespace openage {
namespace util {

/**
 * Returns the unique identifier of the current thread.
 *
 * For compilers with support for thread_local storage, the function also guarantees
 * to return strictly monotonically increasing identifier (no collision).
 */
size_t get_current_thread_id();

}} // namespace openage::util
