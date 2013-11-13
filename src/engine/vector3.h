#ifndef _ENGINE_VECTOR3_H_
#define _ENGINE_VECTOR3_H_

namespace openage {
namespace engine {

template<typename T, bool absolute>
struct vector3 {
	T ne, se, up;

	T len_square() {
		return ne * ne + se * se + up * up;
	}
};

template<typename T>
vector3<T, false> rel(vector3<T, true> v) {
	return vector3<T, false> {v.ne, v.se, v.up};
}

template<typename T>
vector3<T, true> abs(vector3<T, false> v) {
	return vector3<T, true> {v.ne, v.se, v.up};
}

//additions
template<typename T>
vector3<T, true> operator+(vector3<T, true> v, vector3<T, false> w) {
	return vector3<T, true> {(T) (v.ne + w.ne), (T) (v.se + w.se), (T) (v.up + w.up)};
}

template<typename T>
vector3<T, true> operator+(vector3<T, false> v, vector3<T, true> w) {
	return w + v;
}

template<typename T>
vector3<T, false> operator+(vector3<T, false> v, vector3<T, false> w) {
	return vector3<T, false> {(T) (v.ne + w.ne), (T) (v.se + w.se), (T) (v.up + w.up)};
}

//subtractions
template<typename T>
vector3<T, false> operator-(vector3<T, false> v, vector3<T, false> w) {
	return vector3<T, false> {(T) (v.ne - w.ne), (T) (v.se - w.se), (T) (v.up - w.up)};
}

template<typename T>
vector3<T, false> operator-(vector3<T, true> v, vector3<T, true> w) {
	return vector3<T, false> {(T) (v.ne - w.ne), (T) (v.se - w.se), (T) (v.up - w.up)};
}

template<typename T>
vector3<T, true> operator-(vector3<T, true> v, vector3<T, false> w) {
	return vector3<T, true> {(T) (v.ne - w.ne), (T) (v.se - w.se), (T) (v.up - w.up)};
}

//scalar multiplication/division of relative vectors
template<typename T, bool absolute>
vector3<T, absolute> operator-(vector3<T, absolute> v) {
	return vector3<T, absolute> {-v.ne, -v.se, -v.up};
}

template <typename T, bool absolute>
vector3<T, absolute> operator*(T f, vector3<T, absolute> v) {
	return v * f;
}

template <typename T, bool absolute>
vector3<T, absolute> operator*(vector3<T, absolute> v, T f) {
	return vector3<T, absolute> {(T) (v.ne * f), (T) (v.se * f), (T) (v.up * f)};
}

template <typename T, bool absolute>
vector3<T, absolute> operator/(vector3<T, absolute> v, T f) {
	return vector3<T, absolute> {(T) (v.ne / f), (T) (v.se / f), (T) (v.up / f)};
}

//composite arithmetic/assignment operators:
//internally use the purely arithmetic operators
template<typename T>
vector3<T, true> &operator+=(vector3<T, true> &v, vector3<T, false> w) {
	v = v + w;
	return v;
}

template<typename T>
vector3<T, true> &operator-=(vector3<T, true> &v, vector3<T, false> w) {
	v = v - w;
	return v;
}

template<typename T>
vector3<T, false> &operator+=(vector3<T, false> &v, vector3<T, false> w) {
	v = v + w;
	return v;
}

template<typename T>
vector3<T, false> &operator-=(vector3<T, false> &v, vector3<T, false> w) {
	v = v - w;
	return v;
}

template<typename T, bool absolute>
vector3<T, absolute> &operator*=(vector3<T, absolute> &v, T f) {
	v = v * f;
	return v;
}

template<typename T, bool absolute>
vector3<T, absolute> &operator/=(vector3<T, absolute> &v, T f) {
	v = v / f;
	return v;
}

} //namespace engine
} //namespace openage

#endif //_ENGINE_VECTOR3_H_
