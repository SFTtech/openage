// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_XYZ_COORD_H_
#define OPENAGE_COORD_XYZ_COORD_H_

namespace openage {
namespace coord {

template<typename ST, unsigned int N>
struct xyz_coord {
};

template <typename ST>
struct xyz_coord<ST, 1> {
	ST x;

	template<unsigned int I>
	ST& get();
};

template <typename ST>
struct xyz_coord<ST, 2> {
	ST x, y;

	template<unsigned int I>
	ST& get();
};

template <typename ST>
struct xyz_coord<ST, 3> {
	ST x, y, z;

	template<unsigned int I>
	ST& get();
};


template<typename ST>
template<unsigned int I>
ST& xyz_coord<ST, 1>::get() {
	if(I == 0) {
		return x;
	}
}

template<typename ST>
template<unsigned int I>
ST& xyz_coord<ST, 2>::get() {
	if(I == 0) {
		return x;
	} else if(I == 1) {
		return y;
	}
}

template<typename ST>
template<unsigned int I>
ST& xyz_coord<ST, 3>::get() {
	if(I == 0) {
		return x;
	} else if(I == 1) {
		return y;
	} else if(I == 2) {
		return z;
	}
}

template<typename AT, typename RT, typename ST, unsigned int N>
struct absolute_xyz_coord : public xyz_coord<ST, N> {
};

template<typename AT, typename RT, typename ST>
struct absolute_xyz_coord<AT, RT, ST, 1> : public xyz_coord<ST, 1> {
	RT as_relative() const;
	AT operator-() const;
	AT operator+(RT arg) const;
	RT operator-(AT arg) const;
	AT operator-(RT arg) const;
	AT operator*(ST arg) const;
	AT operator/(ST arg) const;
	bool operator==(AT arg) const;

	AT &operator+=(RT arg);
	AT &operator-=(RT arg);
	AT &operator*=(ST arg);
	AT &operator/=(ST arg);
};

template<typename AT, typename RT, typename ST>
struct absolute_xyz_coord<AT, RT, ST, 2> : public xyz_coord<ST, 2> {
	RT as_relative() const;
	AT operator-() const;
	AT operator+(RT arg) const;
	RT operator-(AT arg) const;
	AT operator-(RT arg) const;
	AT operator*(ST arg) const;
	AT operator/(ST arg) const;
	bool operator==(AT arg) const;

	AT &operator+=(RT arg);
	AT &operator-=(RT arg);
	AT &operator*=(ST arg);
	AT &operator/=(ST arg);
};

template<typename AT, typename RT, typename ST>
struct absolute_xyz_coord<AT, RT, ST, 3> : public xyz_coord<ST, 3> {
	RT as_relative() const;
	AT operator-() const;
	AT operator+(RT arg) const;
	RT operator-(AT arg) const;
	AT operator-(RT arg) const;
	AT operator*(ST arg) const;
	AT operator/(ST arg) const;
	bool operator==(AT arg) const;

	AT &operator+=(RT arg);
	AT &operator-=(RT arg);
	AT &operator*=(ST arg);
	AT &operator/=(ST arg);
};

//conversion to relative vector
template <typename AT, typename RT, typename ST>
RT absolute_xyz_coord<AT, RT, ST, 1>::as_relative() const {
	return RT{this->x};
}

template <typename AT, typename RT, typename ST>
RT absolute_xyz_coord<AT, RT, ST, 2>::as_relative() const {
	return RT{this->x, this->y};
}

template <typename AT, typename RT, typename ST>
RT absolute_xyz_coord<AT, RT, ST, 3>::as_relative() const {
	return RT{this->x, this->y, this->z};
}

//-abs -> abs
template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 1>::operator-() const {
	return AT{-this->x};
}

template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 2>::operator-() const {
	return AT{-this->x, -this->y};
}

template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 3>::operator-() const {
	return AT{-this->x, -this->y, -this->z};
}

//abs + rel -> abs
template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 1>::operator+(RT arg) const {
	return AT{this->x + arg.x};
}

template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 2>::operator+(RT arg) const {
	return AT{this->x + arg.x, this->y + arg.y};
}

template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 3>::operator+(RT arg) const {
	return AT{this->x + arg.x, this->y + arg.y, this->z + arg.z};
}

//abs - abs -> rel
template <typename AT, typename RT, typename ST>
RT absolute_xyz_coord<AT, RT, ST, 1>::operator-(AT arg) const {
	return RT{this->x - arg.x};
}
template <typename AT, typename RT, typename ST>
RT absolute_xyz_coord<AT, RT, ST, 2>::operator-(AT arg) const {
	return RT{this->x - arg.x, this->y - arg.y};
}
template <typename AT, typename RT, typename ST>
RT absolute_xyz_coord<AT, RT, ST, 3>::operator-(AT arg) const {
	return RT{this->x - arg.x, this->y - arg.y, this->z - arg.z};
}

//abs - rel -> abs
template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 1>::operator-(RT arg) const {
	return AT{this->x - arg.x};
}

template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 2>::operator-(RT arg) const {
	return AT{this->x - arg.x, this->y - arg.y};
}

template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 3>::operator-(RT arg) const {
	return AT{this->x - arg.x, this->y - arg.y, this->z - arg.z};
}

//abs * scalar -> abs
template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 1>::operator*(ST arg) const {
	return AT{this->x * arg};
}

template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 2>::operator*(ST arg) const {
	return AT{this->x * arg, this->y * arg};
}

template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 3>::operator*(ST arg) const {
	return AT{this->x * arg, this->y * arg, this->z * arg};
}

//abs / scalar -> abs
template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 1>::operator/(ST arg) const {
	return AT{this->x / arg};
}

template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 2>::operator/(ST arg) const {
	return AT{this->x / arg, this->y / arg};
}

template <typename AT, typename RT, typename ST>
AT absolute_xyz_coord<AT, RT, ST, 3>::operator/(ST arg) const {
	return AT{this->x / arg, this->y / arg, this->z / arg};
}

//abs += rel
template <typename AT, typename RT, typename ST>
AT &absolute_xyz_coord<AT, RT, ST, 1>::operator+=(RT arg) {
	*this = *this + arg;
	return (AT&)*this;
}

template <typename AT, typename RT, typename ST>
AT &absolute_xyz_coord<AT, RT, ST, 2>::operator+=(RT arg) {
	*this = *this + arg;
	return (AT&)*this;
}

template <typename AT, typename RT, typename ST>
AT &absolute_xyz_coord<AT, RT, ST, 3>::operator+=(RT arg) {
	*this = *this + arg;
	return (AT&)*this;
}

//abs -= rel
template <typename AT, typename RT, typename ST>
AT &absolute_xyz_coord<AT, RT, ST, 1>::operator-=(RT arg) {
	*this = *this - arg;
	return (AT&)*this;
}

template <typename AT, typename RT, typename ST>
AT &absolute_xyz_coord<AT, RT, ST, 2>::operator-=(RT arg) {
	*this = *this - arg;
	return (AT&)*this;
}

template <typename AT, typename RT, typename ST>
AT &absolute_xyz_coord<AT, RT, ST, 3>::operator-=(RT arg) {
	*this = *this - arg;
	return (AT&)*this;
}

//abs *= scalar
template <typename AT, typename RT, typename ST>
AT &absolute_xyz_coord<AT, RT, ST, 1>::operator*=(ST arg) {
	*this = *this * arg;
	return (AT&)*this;
}

template <typename AT, typename RT, typename ST>
AT &absolute_xyz_coord<AT, RT, ST, 2>::operator*=(ST arg) {
	*this = *this * arg;
	return (AT&)*this;
}

template <typename AT, typename RT, typename ST>
AT &absolute_xyz_coord<AT, RT, ST, 3>::operator*=(ST arg) {
	*this = *this * arg;
	return (AT&)*this;
}

//abs /= scalar
template <typename AT, typename RT, typename ST>
AT &absolute_xyz_coord<AT, RT, ST, 1>::operator/=(ST arg) {
	*this = *this / arg;
	return (AT&)*this;
}
template <typename AT, typename RT, typename ST>
AT &absolute_xyz_coord<AT, RT, ST, 2>::operator/=(ST arg) {
	*this = *this / arg;
	return (AT&)*this;
}
template <typename AT, typename RT, typename ST>
AT &absolute_xyz_coord<AT, RT, ST, 3>::operator/=(ST arg) {
	*this = *this / arg;
	return (AT&)*this;
}

//abs == abs
template <typename AT, typename RT, typename ST>
bool absolute_xyz_coord<AT, RT, ST, 1>::operator==(AT arg) const {
	return this->x == arg.x;
}

template <typename AT, typename RT, typename ST>
bool absolute_xyz_coord<AT, RT, ST, 2>::operator==(AT arg) const {
	return this->x == arg.x && this->y == arg.y;
}

template <typename AT, typename RT, typename ST>
bool absolute_xyz_coord<AT, RT, ST, 3>::operator==(AT arg) const {
	return this->x == arg.x && this->y == arg.y && this->z == arg.z;
}


template<typename AT, typename RT, typename ST, unsigned int N>
struct relative_xyz_coord : public xyz_coord<ST, N> {
};

template<typename AT, typename RT, typename ST>
struct relative_xyz_coord<AT, RT, ST, 1> : public xyz_coord<ST, 1> {
	AT as_absolute() const;
	RT operator-() const;
	AT operator+(AT arg) const;
	RT operator+(RT arg) const;
	RT operator-(RT arg) const;
	RT operator*(ST arg) const;
	RT operator/(ST arg) const;
	bool operator==(RT arg) const;

	RT &operator+=(RT arg);
	RT &operator-=(RT arg);
	RT &operator*=(ST arg);
	RT &operator/=(ST arg);
};

template<typename AT, typename RT, typename ST>
struct relative_xyz_coord<AT, RT, ST, 2> : public xyz_coord<ST, 2> {
	AT as_absolute() const;
	RT operator-() const;
	AT operator+(AT arg) const;
	RT operator+(RT arg) const;
	RT operator-(RT arg) const;
	RT operator*(ST arg) const;
	RT operator/(ST arg) const;
	bool operator==(RT arg) const;

	RT &operator+=(RT arg);
	RT &operator-=(RT arg);
	RT &operator*=(ST arg);
	RT &operator/=(ST arg);
};

template<typename AT, typename RT, typename ST>
struct relative_xyz_coord<AT, RT, ST, 3> : public xyz_coord<ST, 3> {
	AT as_absolute() const;
	RT operator-() const;
	AT operator+(AT arg) const;
	RT operator+(RT arg) const;
	RT operator-(RT arg) const;
	RT operator*(ST arg) const;
	RT operator/(ST arg) const;
	bool operator==(RT arg) const;

	RT &operator+=(RT arg);
	RT &operator-=(RT arg);
	RT &operator*=(ST arg);
	RT &operator/=(ST arg);
};

//conversion to absolute vector
template<typename AT, typename RT, typename ST>
AT relative_xyz_coord<AT, RT, ST, 1>::as_absolute() const {
	return AT{this->x};
}

template<typename AT, typename RT, typename ST>
AT relative_xyz_coord<AT, RT, ST, 2>::as_absolute() const {
	return AT{this->x, this->y};
}

template<typename AT, typename RT, typename ST>
AT relative_xyz_coord<AT, RT, ST, 3>::as_absolute() const {
	return AT{this->x, this->y, this->z};
}

//-rel -> rel
template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 1>::operator-() const {
	return RT{-this->x};
}

template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 2>::operator-() const {
	return RT{-this->x, -this->y};
}

template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 3>::operator-() const {
	return RT{-this->x, -this->y, -this->z};
}

//rel + abs -> abs
template<typename AT, typename RT, typename ST>
AT relative_xyz_coord<AT, RT, ST, 1>::operator+(AT arg) const {
	return AT{this->x + arg.x};
}

template<typename AT, typename RT, typename ST>
AT relative_xyz_coord<AT, RT, ST, 2>::operator+(AT arg) const {
	return AT{this->x + arg.x, this->y + arg.y};
}

template<typename AT, typename RT, typename ST>
AT relative_xyz_coord<AT, RT, ST, 3>::operator+(AT arg) const {
	return AT{this->x + arg.x, this->y + arg.y, this->z + arg.z};
}

//rel + rel -> rel
template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 1>::operator+(RT arg) const {
	return RT{this->x + arg.x};
}

template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 2>::operator+(RT arg) const {
	return RT{this->x + arg.x, this->y + arg.y};
}

template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 3>::operator+(RT arg) const {
	return RT{this->x + arg.x, this->y + arg.y, this->z + arg.z};
}

//rel - rel -> rel
template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 1>::operator-(RT arg) const {
	return RT{this->x - arg.x};
}

template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 2>::operator-(RT arg) const {
	return RT{this->x - arg.x, this->y - arg.y};
}

template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 3>::operator-(RT arg) const {
	return RT{this->x - arg.x, this->y - arg.y, this->z - arg.z};
}

//rel * scalar -> rel
template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 1>::operator*(ST arg) const {
	return RT{this->x * arg};
}

template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 2>::operator*(ST arg) const {
	return RT{this->x * arg, this->y * arg};
}

template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 3>::operator*(ST arg) const {
	return RT{this->x * arg, this->y * arg, this->z * arg};
}

//rel / scalar -> rel
template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 1>::operator/(ST arg) const {
	return RT{this->x / arg};
}

template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 2>::operator/(ST arg) const {
	return RT{this->x / arg, this->y / arg};
}

template<typename AT, typename RT, typename ST>
RT relative_xyz_coord<AT, RT, ST, 3>::operator/(ST arg) const {
	return RT{this->x / arg, this->y / arg, this->z / arg};
}

//rel += rel
template<typename AT, typename RT, typename ST>
RT &relative_xyz_coord<AT, RT, ST, 1>::operator+=(RT arg) {
	*this = *this + arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_xyz_coord<AT, RT, ST, 2>::operator+=(RT arg) {
	*this = *this + arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_xyz_coord<AT, RT, ST, 3>::operator+=(RT arg) {
	*this = *this + arg;
	return (RT&)*this;
}

//rel -= rel
template<typename AT, typename RT, typename ST>
RT &relative_xyz_coord<AT, RT, ST, 1>::operator-=(RT arg) {
	*this = *this - arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_xyz_coord<AT, RT, ST, 2>::operator-=(RT arg) {
	*this = *this - arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_xyz_coord<AT, RT, ST, 3>::operator-=(RT arg) {
	*this = *this - arg;
	return (RT&)*this;
}

//rel *= scalar
template<typename AT, typename RT, typename ST>
RT &relative_xyz_coord<AT, RT, ST, 1>::operator*=(ST arg) {
	*this = *this * arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_xyz_coord<AT, RT, ST, 2>::operator*=(ST arg) {
	*this = *this * arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_xyz_coord<AT, RT, ST, 3>::operator*=(ST arg) {
	*this = *this * arg;
	return (RT&)*this;
}

//rel /= scalar
template<typename AT, typename RT, typename ST>
RT &relative_xyz_coord<AT, RT, ST, 1>::operator/=(ST arg) {
	*this = *this / arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_xyz_coord<AT, RT, ST, 2>::operator/=(ST arg) {
	*this = *this / arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_xyz_coord<AT, RT, ST, 3>::operator/=(ST arg) {
	*this = *this / arg;
	return (RT&)*this;
}

//rel == rel
template<typename AT, typename RT, typename ST>
bool relative_xyz_coord<AT, RT, ST, 1>::operator==(RT arg) const {
	return this->x == arg.x;
}

template<typename AT, typename RT, typename ST>
bool relative_xyz_coord<AT, RT, ST, 2>::operator==(RT arg) const {
	return this->x == arg.x && this->y == arg.y;
}

template<typename AT, typename RT, typename ST>
bool relative_xyz_coord<AT, RT, ST, 3>::operator==(RT arg) const {
	return this->x == arg.x && this->y == arg.y && this->z == arg.z;
}

} // namespace coord
} // namespace openage

#endif
