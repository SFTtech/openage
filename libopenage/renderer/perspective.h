#include <eigen3/Eigen/Dense>
#include <cmath>
template<class T>
Eigen::Matrix<T,4,4> perspective
(
    double fovy,
    double aspect,
    double zNear,
    double zFar
)
{
    typedef Eigen::Matrix<T,4,4> Matrix4;

    assert(aspect > 0);
    assert(zFar > zNear);

    double radf = (double)(fovy)*(M_PI/180.0);

    double tanHalfFovy = tan(radf / 2.0);
    Matrix4 res = Matrix4::Zero();
    res(0,0) = 1.0 / (aspect * tanHalfFovy);
    res(1,1) = 1.0 / (tanHalfFovy);
    res(2,2) = - (zFar + zNear) / (zFar - zNear);
    res(3,2) = - 1.0;
    res(2,3) = - (2.0 * zFar * zNear) / (zFar - zNear);
    return res;
}

template<class T>
Eigen::Matrix<T,4,4> lookAt
(
    Eigen::Matrix<T,3,1> const & eye,
    Eigen::Matrix<T,3,1> const & center,
    Eigen::Matrix<T,3,1> const & up
)
{
    typedef Eigen::Matrix<T,4,4> Matrix4;
    typedef Eigen::Matrix<T,3,1> Vector3;

    Vector3 f = (center - eye).normalized();
    Vector3 u = up.normalized();
    Vector3 s = f.cross(u).normalized();
    u = s.cross(f);

    Matrix4 res;
    res <<  s.x(),s.y(),s.z(),-s.dot(eye),
            u.x(),u.y(),u.z(),-u.dot(eye),
            -f.x(),-f.y(),-f.z(),f.dot(eye),
            0,0,0,1;

    return res;
}
