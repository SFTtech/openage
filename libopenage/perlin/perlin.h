// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <unordered_map>

#include "../coord/decl.h"
#include "../rng/box_muller.h"
#include "../rng/rng.h"
#include "../util/hash.h"
#include "../util/vector.h"

namespace openage {
namespace perlin {

using seed_t = size_t; // std::hash return value
using coord_t = coord::phys_t;
using value_t = int64_t;
template<size_t N>
using coord_vec = util::Vector<N, coord_t>;
template<size_t N>
using value_vec = util::Vector<N, value_t>;

/**
 */
template<size_t N>
class Perlin {
	using cvec_t = coord_vec<N>;
	using vvec_t = value_vec<N>;

	seed_t seed;
	size_t granularity;
	std::unordered_map<cvec_t, vvec_t> gradient_cache;
	// Maximum possible gradient vector component value without risking
	// overflows during dot_product calculations.
	value_t comp_max = static_cast<value_t>(sqrt(static_cast<double>(INT64_MAX) / N));

	value_t interpolate(value_t x, value_t y, double pos) const {
		double factor = (3 * std::pow(pos, 2)) - (2 * std::pow(pos, 3));
		return (value_t)((1 - factor) * x + factor * y);
	}

	vvec_t calculate_gradient(const cvec_t &node) const {
		rng::RNG rng = rng::RNG{util::hash_combine(
			this->seed,
			std::hash<cvec_t>{}(this->shrink(node))
		)};
		rng::BoxMuller gauss = rng::BoxMuller{std::bind(&rng::RNG::real, &rng)};
		auto tmp = util::Vector<N, double>{};
		std::generate(tmp.begin(), tmp.end(), gauss);
		tmp.normalize();
		auto res = vvec_t{};
		for (size_t i = 0; i < N; i++) {
			res[i] = static_cast<value_t>(this->comp_max * tmp[i]);
		}
		return res;
	}

	vvec_t get_gradient(const cvec_t &node) {
		auto it = this->gradient_cache.find(node);
		if (it != this->gradient_cache.end()) {
			return it->second;
		}
		vvec_t res = this->calculate_gradient(node);
		this->gradient_cache[node] = res;
		return res;
	}

	value_t magic_dot_product(const cvec_t &node, const cvec_t &point) {
		vvec_t diff = node - point;
		return diff.dot_product(this->get_gradient(node));
	}

	value_t multidim_inter(const size_t dim,
	                       cvec_t &point,
	                       const cvec_t &orig_point) {

		if (dim == N) {
			return this->magic_dot_product(point, orig_point);
		}

		// backup relevant state of point
		coord_t saved_coord = point[dim];

		point[dim] = this->lower_cell_border(saved_coord);
		value_t lo_part = this->multidim_inter(dim + 1, point, orig_point);

		point[dim] += this->granularity;
		value_t hi_part = this->multidim_inter(dim + 1, point, orig_point);

		point[dim] = saved_coord;
		// point state is now restored

		// TODO: can be precalced
		double pos = static_cast<double>(saved_coord) / this->granularity;
		return this->interpolate(lo_part, hi_part, pos);
	}

	coord_t lower_cell_border(coord_t num) const {
		return num - (num % this->granularity);
	}

	cvec_t shrink(const cvec_t &node) const {
		return node / this->granularity;
	}

public:
	Perlin(seed_t seed, size_t granularity)
		:
		seed{seed},
		granularity{granularity},
		gradient_cache{} {};

	~Perlin() = default;

	value_t noise_value(const cvec_t &point) {
		// TODO: normalize for different N
		cvec_t tmp = point;
		return this->multidim_inter(0, tmp, point);
	}

};

}} // openage::perlin
