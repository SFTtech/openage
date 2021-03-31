// Copyright 2018-2019 the openage authors. See copying.md for legal info.


#include "util.h"


namespace openage::renderer::util {

Eigen::Matrix4d ortho_matrix_d(double left,   double right,
                               double bottom, double top,
                               double near,   double far) {
	return ortho_matrix<Eigen::Matrix4d, double>(left, right, bottom, top, near, far);
}


Eigen::Matrix4f ortho_matrix_f(float left,   float right,
                               float bottom, float top,
                               float near,   float far) {
	return ortho_matrix<Eigen::Matrix4f, float>(left, right, bottom, top, near, far);
}

} // openage::renderer::util
