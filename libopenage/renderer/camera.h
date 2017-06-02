// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#pragma once

#include <eigen3/Eigen/Core>

namespace openage {
namespace renderer {

    class Camera {
		public:
            Camera();

            void set_aspect_ratio(const float ratio);

            void translate(const Eigen::Vector3f &translate_vector);
            void rotate_x(const float radians);
            void rotate_axis(const float radians, const Eigen::Vector3f &axis);

            Eigen::Matrix4f get_view_project_transform_2d() const;

        private:
            static const Eigen::Vector3f LOCAL_LOOK_DIRECTION_2D;
            static const Eigen::Vector3f LOCAL_LOOK_DIRECTION_3D;
            static const Eigen::Vector3f UP_VECTOR;

            float aspect_ratio;
            Eigen::Matrix4f translate_transform;
            Eigen::Matrix4f rotate_transform;
            Eigen::Matrix4f view_transform_3d;
            Eigen::Matrix4f view_transform_2d;
            Eigen::Matrix4f project_transform;
            Eigen::Matrix4f view_project_transform_3d;
            Eigen::Matrix4f view_project_transform_2d;

            void calculate_view_project_transform();
            void calculate_view_transform();
            void calculate_project_transform();

            static Eigen::Matrix4f look_at_direction(const Eigen::Vector3f &eye, const Eigen::Vector3f &look_direction, const Eigen::Vector3f &up);
            static Eigen::Matrix4f ortho(const float left, const float right, const float top, const float bottom, const float near, const float far);
	};

}}
