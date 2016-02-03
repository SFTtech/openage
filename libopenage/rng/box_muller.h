// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <functional>
#include <cmath>

#include "../util/generator.h"
#include "../crossplatform/math_constants.h"

namespace openage {
namespace rng {

class BoxMuller : public util::Generator<double> {
private:
	std::function<double()> random;

protected:
	virtual void generate();

public:
	BoxMuller(std::function<double()> random);

	~BoxMuller() = default;

};

}} // openage::rng
