// Copyright 2018-2022 the openage authors. See copying.md for legal info.

#pragma once


#include <eigen3/Eigen/Dense>


namespace openage::renderer::util {

/**
 * Creates a orthographic projection matrix.
 *
 * @param left Left vertical clipping plane coordinates.
 * @param right Right vertical clipping plane coordinates.
 * @param bottom: Bottom horizontal clipping plane coordinates.
 * @param top: Top horizontal clipping plane coordinates.
 * @param near: Distances to nearer depth clipping planes. Specify as negative if the plane is behind the viewpoint.
 * @param far: Distances to farther depth clipping planes. Specify as negative if the plane is behind the viewpoint.
 */
template <typename matrix_t, typename number_t>
matrix_t ortho_matrix(number_t left, number_t right, number_t bottom, number_t top, number_t near, number_t far) {
	matrix_t ortho;
	ortho << 2 / (right - left), 0, 0, -(right + left) / (right - left),
		0, 2 / (top - bottom), 0, -(top + bottom) / (top - bottom),
		0, 0, -2 / (far - near), -(far + near) / (far - near),
		0, 0, 0, 1;

	return ortho;
}


/**
 * Create a orthographic projection matrix. Double variant.
 *
 * @see ortho_matrix
 */
Eigen::Matrix4d ortho_matrix_d(double left, double right, double bottom, double top, double near, double far);


/**
 * Create a orthographic projection matrix. Float variant.
 *
 * @see ortho_matrix
 */
Eigen::Matrix4f ortho_matrix_f(float left, float right, float bottom, float top, float near, float far);

} // namespace openage::renderer::util
