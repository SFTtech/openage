// Copyright 2015-2015 the openage authors. See copying.md for legal info.
#ifndef OPENAGE_UTIL_ALGORITHM_H_
#define OPENAGE_UTIL_ALGORITHM_H_

#include <algorithm>

// TODO: make general transformation from iterator algorithm to container
// algorithm. so much metaprogramming...

namespace openage {
namespace util {

/**
 * std::for_each except just on containers.
 */
template<class Container, class Function>
inline Function for_each(Container &&container, Function &&func) {
	// why cpp why...
	return std::for_each(std::begin(std::forward<Container>(container)),
	                     std::end(std::forward<Container>(container)),
	                     std::forward<Function>(func));
}

/**
 * Filters items from a container which satisfy a certain predicate.
 */
template<class Container, class Function>
inline void remove_from(Container &container, Function &&func) {
	container.erase(std::remove_if(std::begin(container),
	                               std::end(container),
	                               std::forward<Function>(func)),
	                std::end(container));
}


} // namespace util
} // namespace openage

#endif
