// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <unordered_map>
#include <cmath>

#include "../coord/decl.h"
#include "../util/vector.h"

namespace openage {
namespace perlin {

using value_t = int64_t;
using coord_t = coord::phys_t;
template<size_t N>
using perlin_vec = util::Vector<N, coord::phys_t>;
using seed_t = size_t; // std::hash return value

/**
 */
template<size_t N>
class Perlin {
using vector_t = perlin_vec<N>;

	seed_t seed;
	size_t granularity;
	std::unordered_map<vector_t, vector_t> gradient_cache;

	value_t interpolate(value_t x, value_t y, double pos) const {
		double factor = (3 * std::pow(pos, 2)) - (2 * std::pow(pos, 3));
		return (value_t)((1 - factor) * x + factor * y);
	}

	vector_t calculate_gradient(vector_t node) {
		// FIXME
		// seed prng with this->shrink(node) so that granularity does not
		// influence the overall look of the noise
		return vector_t{};
	}

	vector_t get_gradient(vector_t node) {
		auto it = this->gradient_cache.find(node);
		if (it != this->gradient_cache.end()) {
			return it->second;
		}
		vector_t res = this->calculate_gradient(node);
		this->gradient_cache[node] = res;
		return res;
	}

	value_t magic_dot_product(vector_t node, vector_t point) {
		vector_t diff = node - point;
		return diff.dot_product(this->get_gradient(node));
	}

	value_t multidim_inter(size_t dim,
	                       vector_t &point,
	                       const vector_t &orig_point) {

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

	coord_t lower_cell_border(coord_t num) {
		return num - (num % this->granularity);
	}

	vector_t shrink(vector_t node) {
		return node / this->granularity;
	}

public:
	Perlin(seed_t seed, size_t granularity)
		:
		seed{seed},
		granularity{granularity},
		gradient_cache{} {};

	~Perlin() = default;

	value_t noise_value(const vector_t &point) {
		// TODO: normalize for different N
		vector_t tmp = point;
		return this->multidim_inter(0, tmp, point);
	}

};

}} // openage::perlin
