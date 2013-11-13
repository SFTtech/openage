#ifndef _ENGINE_VECTOR2_H_
#define _ENGINE_VECTOR2_H_

namespace openage {
namespace engine {

template<typename T, bool absolute>
struct vector2 {
	T x;
	T y;

	T len_square() {
		return x * x + y * y;
	}
};

template<typename T>
vector2<T, false> rel(vector2<T, true> v) {
	return vector2<T, false> {v.x, v.y};
}

template<typename T>
vector2<T, true> abs(vector2<T, false> v) {
	return vector2<T, true> {v.x, v.y};
}

//additions
template<typename T>
vector2<T, true> operator+(vector2<T, true> v, vector2<T, false> w) {
	return vector2<T, true> {(T) (v.x + w.x), (T) (v.y + w.y)};
}

template<typename T>
vector2<T, true> operator+(vector2<T, false> v, vector2<T, true> w) {
	return w + v;
}

template<typename T>
vector2<T, false> operator+(vector2<T, false> v, vector2<T, false> w) {
	return vector2<T, false> {(T) (v.x + w.x), (T) (v.y + w.y)};
}

//subtractions
template<typename T>
vector2<T, false> operator-(vector2<T, false> v, vector2<T, false> w) {
	return vector2<T, false> {(T) (v.x - w.x), (T) (v.y - w.y)};
}

template<typename T>
vector2<T, false> operator-(vector2<T, true> v, vector2<T, true> w) {
	return vector2<T, false> {(T) (v.x - w.x), (T) (v.y - w.y)};
}

template<typename T>
vector2<T, true> operator-(vector2<T, true> v, vector2<T, false> w) {
	return vector2<T, true> {(T) (v.x - w.x), (T) (v.y - w.y)};
}

//scalar multiplication/division of relative vectors
template<typename T, bool absolute>
vector2<T, absolute> operator-(vector2<T, absolute> v) {
	return vector2<T, absolute> {-v.x, -v.y};
}

template <typename T, bool absolute>
vector2<T, absolute> operator*(T f, vector2<T, absolute> v) {
	return v * f;
}

template <typename T, bool absolute>
vector2<T, absolute> operator*(vector2<T, absolute> v, T f) {
	return vector2<T, absolute> {(T) (v.x * f), (T) (v.y * f)};
}

template <typename T, bool absolute>
vector2<T, absolute> operator/(vector2<T, absolute> v, T f) {
	return vector2<T, absolute> {(T) (v.x / f), (T) (v.y / f)};
}

//composite arithmetic/assignment operators:
//internally use the purely arithmetic operators
template<typename T>
vector2<T, true> &operator+=(vector2<T, true> &v, vector2<T, false> w) {
	v = v + w;
	return v;
}

template<typename T>
vector2<T, true> &operator-=(vector2<T, true> &v, vector2<T, false> w) {
	v = v - w;
	return v;
}

template<typename T>
vector2<T, false> &operator+=(vector2<T, false> &v, vector2<T, false> w) {
	v = v + w;
	return v;
}

template<typename T>
vector2<T, false> &operator-=(vector2<T, false> &v, vector2<T, false> w) {
	v = v - w;
	return v;
}

template<typename T, bool absolute>
vector2<T, absolute> &operator*=(vector2<T, absolute> &v, T f) {
	v = v * f;
	return v;
}

template<typename T, bool absolute>
vector2<T, absolute> &operator/=(vector2<T, absolute> &v, T f) {
	v = v / f;
	return v;
}

} //namespace engine
} //namespace openage

#endif //_ENGINE_VECTOR2_H_
