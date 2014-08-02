#ifndef _ENGINE_AUDIO_HASH_FUNCTIONS_H_
#define _ENGINE_AUDIO_HASH_FUNCTIONS_H_

#include <tuple>

#include "category.h"

namespace std {

template<>
struct hash<::engine::audio::category_t> {
	size_t operator()(const ::engine::audio::category_t &c) const {
		return static_cast<int>(c);
	}
};
template<>

struct hash<std::tuple<::engine::audio::category_t,int>> {
	size_t operator()(const std::tuple<::engine::audio::category_t,int> &t)
			const {
		return static_cast<size_t>(std::get<0>(t))<<32 | std::get<1>(t);
	}
};

}

#endif
