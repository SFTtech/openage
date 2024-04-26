// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <cmath>
#include <cstddef>
#include <memory>

#include <eigen3/Eigen/Dense>

#include "coord/pixel.h"
#include "coord/scene.h"
#include "util/vector.h"

namespace openage::renderer::camera {

class Frustum
{
public:
    Frustum();

    void Recalculate(util::Vector2s& viewport_size, float near_distance, float far_distance, Eigen::Vector3f& scene_pos, Eigen::Vector3f cam_direction, Eigen::Vector3f up_direction, float real_zoom);

    bool is_in_frustum(Eigen::Vector3f& pos) const;

private:
    Eigen::Vector3f top_face_normal;
    float top_face_distance;

    Eigen::Vector3f bottom_face_normal;
    float bottom_face_distance;

    Eigen::Vector3f right_face_normal;
    float right_face_distance;

    Eigen::Vector3f left_face_normal;
    float left_face_distance;

    Eigen::Vector3f far_face_normal;
    float far_face_distance;

    Eigen::Vector3f near_face_normal;
    float near_face_distance;

};
} // namespace openage::renderer::camera
