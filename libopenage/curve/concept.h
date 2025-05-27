// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <concepts>

namespace openage::curve {

/**
 * Concept for keyframe values.
 */
template <typename T>
concept KeyframeValueLike = std::copyable<T> && std::equality_comparable<T>;

} // namespace openage::curve
