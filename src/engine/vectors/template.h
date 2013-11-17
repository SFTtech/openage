#error "this header serves as a template for gen.py. it must not be included."

#ifndef _ENGINE_VECTORS_[uppername]_H__
#define _ENGINE_VECTORS_[uppername]_H__

namespace openage {
namespace engine {
namespace vectors {

template<class T, bool absolute, template <bool> class derived>
struct [name] {};

//template specialization for absolute vectors
template<class T, template <bool> class derived>
struct [name]<T, true, derived> {
	T [vlist];

	[name]() {}
	[name]([tlist]): [ilist] {}

	//conversion to relative vector
	derived<false> as_relative() {
		return derived<false>{[vlist]};
	}

	//-abs -> abs
	derived<true> operator-() {
		return derived<true>{[negation]};
	}

	//abs + rel -> abs
	derived<true> operator+(derived<false> arg) {
		return derived<true>{[addition]};
	}

	//abs - abs -> rel
	derived<false> operator-(derived<true> arg) {
		return derived<false>{[difference]};
	}

	//abs - rel -> abs
	derived<true> operator-(derived<false> arg) {
		return derived<true>{[difference]};
	}

	//abs * scalar -> abs
	derived<true> operator*(T arg) {
		return derived<true>{[scalarmult]};
	}

	//abs / scalar -> abs
	derived<true> operator/(T arg) {
		return derived<true>{[scalardiv]};
	}

	//abs += rel
	derived<true> &operator+=(derived<false> arg) {
		*this = *this + arg;
		return *static_cast<derived<true> *>(this);
	}

	//abs -= rel
	derived<true> &operator-=(derived<false> arg) {
		*this = *this - arg;
		return *static_cast<derived<true> *>(this);
	}

	//abs *= scalar
	derived<true> &operator*=(T arg) {
		*this = *this * arg;
		return *static_cast<derived<true> *>(this);
	}

	//abs /= scalar
	derived<true> &operator/=(T arg) {
		*this = *this / arg;
		return *static_cast<derived<true> *>(this);
	}
};

//template specialization for relative vectors
template<class T, template <bool> class derived>
struct [name]<T, false, derived> {
	T [vlist];

	[name]() {}
	[name]([tlist]): [ilist] {}

	//conversion to absolute vector
	derived<true> as_absolute() {
		return derived<true>{[vlist]};
	}

	//-rel -> rel
	derived<false> operator-() {
		return derived<false>{[negation]};
	}

	//rel + abs -> abs
	derived<true> operator+(derived<true> arg) {
		return derived<true>{[addition]};
	}

	//rel + rel -> rel
	derived<false> operator+(derived<false> arg) {
		return derived<false>{[addition]};
	}

	//rel - rel -> rel
	derived<false> operator-(derived<false> arg) {
		return derived<false>{[difference]};
	}

	//rel * scalar -> rel
	derived<false> operator*(T arg) {
		return derived<false>{[scalarmult]};
	}

	//rel / scalar -> rel
	derived<false> operator/(T arg) {
		return derived<false>{[scalardiv]};
	}

	//rel += rel
	derived<false> &operator+=(derived<false> arg) {
		*this = *this + arg;
		return *static_cast<derived<false> *>(this);
	}

	//rel -= rel
	derived<false> &operator-=(derived<false> arg) {
		*this = *this - arg;
		return *static_cast<derived<false> *>(this);
	}

	//rel *= scalar
	derived<false> &operator*=(T arg) {
		*this = *this * arg;
		return *static_cast<derived<false> *>(this);
	}

	//rel /= scalar
	derived<false> &operator/=(T arg) {
		*this = *this / arg;
		return *static_cast<derived<false> *>(this);
	}
};

//scalar * {rel, abs} = {rel, abs}
template<class T, bool absolute, template <bool> class derived>
struct derived<absolute> operator*(T scalar, derived<absolute> vectorval) {
	return vectorval * scalar;
}

/*
 * example code for a class that uses this vector type:
 * note that the 'using' directive requires g++ 4.8/clang++ 3.3
 *
 * template<bool absolute>
 * struct cameracoord: [name]<float, absolute, cameracoord> {
 *         using [name]<float, absolute, cameracoord>::[name];
 *
 *         float lensq();
 * };
 *
 * template<>
 * float cameracoord<true>::lensq() {
 *         return [sqsum];
 * }
 *
 */

} //namespace vectors
} //namespace engine
} //namespace openage

#endif //_ENGINE_VECTORS_[uppername]_H__
