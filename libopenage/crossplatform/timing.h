// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_CROSSPLATFORM_TIMING_H_
#define OPENAGE_CROSSPLATFORM_TIMING_H_

#include <cstdint>

namespace openage {
namespace timing {

/**
 * returns a monotonically-increasing time that returns the number of
 * nanoseconds since an unspecified event.
 *
 * not influenced by system time changes.
 */
int64_t get_monotonic_time();

/**
 * returns the number of nanoseconds since the UNIX epoch.
 */
int64_t get_real_time();

}} // namespace openage::timing

#endif
