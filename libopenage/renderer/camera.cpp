// Copyright 2015-2017 the openage authors. See copying.md for legal info.

#include "camera.h"

#include <eigen3/Eigen/Dense>

#define _USE_MATH_DEFINES
#include <math.h>

namespace openage {
namespace renderer {

    const Eigen::Vector3f Camera::LOCAL_LOOK_DIRECTION_2D = -Eigen::Vector3f::UnitY();
    const Eigen::Vector3f Camera::LOCAL_LOOK_DIRECTION_3D = -Eigen::Vector3f::UnitZ();
    const Eigen::Vector3f Camera::UP_VECTOR = Eigen::Vector3f::UnitY();

    Camera::Camera()
        : aspect_ratio{1.0f}, translate_transform{Eigen::Matrix4f::Identity()}, rotate_transform{Eigen::Matrix4f::Identity()}
    {
        this->translate(Eigen::Vector3f(0.0f, 1.0f, 0.0f));
        const float rads = 45.0f * M_PI / 180.0f;
        this->rotate_x(rads);
    }

    void Camera::set_aspect_ratio(const float ratio)
    {
        assert(ratio > 0.0f);
        this->apsect_ratio = ratio;

        this->calculate_view_project_transform();
    }

    void Camera::translate(const Eigen::Vector3f &translate_vector)
    {
        auto transform = Eigen::Affine3f::Identity();
        transform.pretranslate(translate_vector);
        this->translate_transform = transform.matrix() * this->translate_transform;

        calculate_view_project_transform();
    }

    void Camera::rotate_x(const float radians)
    {
        this->rotate_axis(radians, Eigen::Vector3f::UnitX());
    }

    void Camera::rotate_axis(const float radians, const Eigen::Vector3f &axis)
    {
        auto transform = Eigen::Affine3f::Identity();
        transform.prerotate(Eigen::AngleAxisf(radians, axis));
        this->rotate_transform = transform.matrix() * this->rotate_transform;
        calculate_view_project_transform();
    }

    Eigen::Matrix4f Camera::get_view_project_transform_2d() const
    {
        return this->view_project_transform_2d;
    }

    void Camera::calculate_project_transform()
    {
        static const float WANTED_WIDTH = 16.0f;
        const float height = WANTED_WIDTH / this->aspect_ratio;

        this->project_transform = this->ortho(-WANTED_WIDTH / 2.0f, WANTED_WIDTH / 2.0f, height / 2.0f, -height / 2.0f, 0.0f, 20.0f);
    }

    void Camera::calculate_view_transform()
    {
        this->view_transform_2d = this->look_at_direction(this->translate_transform*Eigen::Vector3f(0.0f, 0.0f, 0.0f), this->LOCAL_LOOK_DIRECTION_2D, this->UP_VETOR);
        this->view_transform_3d = this->look_at_direction(this->translate_transform*Eigen::Vector3f(0.0f, 0.0f, 0.0f),
                                                          this->rotate_transform * this->LOCAL_LOOK_DIRECTION_3D, this->UP_VECTOR);
    }

    void Camera::calculate_view_project_transform()
    {
        this->calculate_view_transform();
        this->calculate_project_transform();

        this->view_project_transform_2d = this->project_transform * this->view_transform_2d;
        this->view_project_transform_3d = this->project_transform * this->view_transform_3d;
    }

    Eigen::Matrix4f Camera::look_at_direction(const Eigen::Vector3f &eye, const Eigen::Vector3f &look_direction, const Eigen::Vector3f &up)
    {
        auto look_direction_norm = look_direction.normalized();
        auto camera_x = look_direction_norm.cross(up).normalized();
        auto camera_up = camera_x.cross(look_direction_norm);

        Eigen::Matrix4f result = Eigen::Matrix4f::Identity();
        result(0, 0) = camera_x(0);
        result(1, 0) = camera_x(1);
        result(2, 0) = camera_x(2);

        result(0, 1) = camera_up(0);
        result(1, 1) = camera_up(1);
        result(1, 2) = camera_up(2);

        result(0, 2) = -look_direction_norm(0);
        result(1, 2) = -look_direction_norm(1);
        result(2, 2) = -look_direction_norm(2);

        result(3, 0) = -camera_x.dot(eye);
        result(3, 1) = -camera_up.dot(eye);
        result(3, 2) = look_direction_norm.dot(eye);

        return result;
    }

    Eigen::Matrix4f Camera::ortho(const float left, const float right, const float top, const float bottom, const float near, const float far)
    {
        Eigen::Matrix4f result = Eigen::Matrix4f::Identity();
        result(0, 0) = 2.0f / (right - left);
        result(1, 1) = 2.0f / (top - bottom);
        result(2, 2) = - 2.0f / (far - near);
        result(3, 0) = - (right + left) / (right - left);
        result(3, 1) = - (top + bottom) / (top - bottom);
        result(3, 2) = - (far + near) / (far - near);

        return result;
    }



}}
