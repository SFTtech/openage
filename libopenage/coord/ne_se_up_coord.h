// Copyright 2013-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_NE_SE_UP_COORD_H_
#define OPENAGE_COORD_NE_SE_UP_COORD_H_

namespace openage {
namespace coord {

template<typename ST, unsigned int N>
struct ne_se_up_coord {
};

template <typename ST>
struct ne_se_up_coord<ST, 1> {
	ST ne;

	template<unsigned int I>
	ST& get();
};

template <typename ST>
struct ne_se_up_coord<ST, 2> {
	ST ne, se;

	template<unsigned int I>
	ST& get();
};

template <typename ST>
struct ne_se_up_coord<ST, 3> {
	ST ne, se, up;

	template<unsigned int I>
	ST& get();
};


template<typename ST>
template<unsigned int I>
ST& ne_se_up_coord<ST, 1>::get() {
	if(I == 0) {
		return ne;
	}
}

template<typename ST>
template<unsigned int I>
ST& ne_se_up_coord<ST, 2>::get() {
	if(I == 0) {
		return ne;
	} else if(I == 1) {
		return se;
	}
}

template<typename ST>
template<unsigned int I>
ST& ne_se_up_coord<ST, 3>::get() {
	if(I == 0) {
		return ne;
	} else if(I == 1) {
		return se;
	} else if(I == 2) {
		return up;
	}
}

template<typename AT, typename RT, typename ST, unsigned int N>
struct absolute_ne_se_up_coord : public ne_se_up_coord<ST, N> {
};

template<typename AT, typename RT, typename ST>
struct absolute_ne_se_up_coord<AT, RT, ST, 1> : public ne_se_up_coord<ST, 1> {
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
struct absolute_ne_se_up_coord<AT, RT, ST, 2> : public ne_se_up_coord<ST, 2> {
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
struct absolute_ne_se_up_coord<AT, RT, ST, 3> : public ne_se_up_coord<ST, 3> {
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
RT absolute_ne_se_up_coord<AT, RT, ST, 1>::as_relative() const {
	return RT{this->ne};
}

template <typename AT, typename RT, typename ST>
RT absolute_ne_se_up_coord<AT, RT, ST, 2>::as_relative() const {
	return RT{this->ne, this->se};
}

template <typename AT, typename RT, typename ST>
RT absolute_ne_se_up_coord<AT, RT, ST, 3>::as_relative() const {
	return RT{this->ne, this->se, this->up};
}

//-abs -> abs
template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 1>::operator-() const {
	return AT{-this->ne};
}

template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 2>::operator-() const {
	return AT{-this->ne, -this->se};
}

template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 3>::operator-() const {
	return AT{-this->ne, -this->se, -this->up};
}

//abs + rel -> abs
template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 1>::operator+(RT arg) const {
	return AT{this->ne + arg.ne};
}

template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 2>::operator+(RT arg) const {
	return AT{this->ne + arg.ne, this->se + arg.se};
}

template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 3>::operator+(RT arg) const {
	return AT{this->ne + arg.ne, this->se + arg.se, this->up + arg.up};
}

//abs - abs -> rel
template <typename AT, typename RT, typename ST>
RT absolute_ne_se_up_coord<AT, RT, ST, 1>::operator-(AT arg) const {
	return RT{this->ne - arg.ne};
}
template <typename AT, typename RT, typename ST>
RT absolute_ne_se_up_coord<AT, RT, ST, 2>::operator-(AT arg) const {
	return RT{this->ne - arg.ne, this->se - arg.se};
}
template <typename AT, typename RT, typename ST>
RT absolute_ne_se_up_coord<AT, RT, ST, 3>::operator-(AT arg) const {
	return RT{this->ne - arg.ne, this->se - arg.se, this->up - arg.up};
}

//abs - rel -> abs
template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 1>::operator-(RT arg) const {
	return AT{this->ne - arg.ne};
}

template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 2>::operator-(RT arg) const {
	return AT{this->ne - arg.ne, this->se - arg.se};
}

template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 3>::operator-(RT arg) const {
	return AT{this->ne - arg.ne, this->se - arg.se, this->up - arg.up};
}

//abs * scalar -> abs
template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 1>::operator*(ST arg) const {
	return AT{this->ne * arg};
}

template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 2>::operator*(ST arg) const {
	return AT{this->ne * arg, this->se * arg};
}

template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 3>::operator*(ST arg) const {
	return AT{this->ne * arg, this->se * arg, this->up * arg};
}

//abs / scalar -> abs
template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 1>::operator/(ST arg) const {
	return AT{this->ne / arg};
}

template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 2>::operator/(ST arg) const {
	return AT{this->ne / arg, this->se / arg};
}

template <typename AT, typename RT, typename ST>
AT absolute_ne_se_up_coord<AT, RT, ST, 3>::operator/(ST arg) const {
	return AT{this->ne / arg, this->se / arg, this->up / arg};
}

//abs += rel
template <typename AT, typename RT, typename ST>
AT &absolute_ne_se_up_coord<AT, RT, ST, 1>::operator+=(RT arg) {
	*this = *this + arg;
	return (AT&)*this;
}

template <typename AT, typename RT, typename ST>
AT &absolute_ne_se_up_coord<AT, RT, ST, 2>::operator+=(RT arg) {
	*this = *this + arg;
	return (AT&)*this;
}

template <typename AT, typename RT, typename ST>
AT &absolute_ne_se_up_coord<AT, RT, ST, 3>::operator+=(RT arg) {
	*this = *this + arg;
	return (AT&)*this;
}

//abs -= rel
template <typename AT, typename RT, typename ST>
AT &absolute_ne_se_up_coord<AT, RT, ST, 1>::operator-=(RT arg) {
	*this = *this - arg;
	return (AT&)*this;
}

template <typename AT, typename RT, typename ST>
AT &absolute_ne_se_up_coord<AT, RT, ST, 2>::operator-=(RT arg) {
	*this = *this - arg;
	return (AT&)*this;
}

template <typename AT, typename RT, typename ST>
AT &absolute_ne_se_up_coord<AT, RT, ST, 3>::operator-=(RT arg) {
	*this = *this - arg;
	return (AT&)*this;
}

//abs *= scalar
template <typename AT, typename RT, typename ST>
AT &absolute_ne_se_up_coord<AT, RT, ST, 1>::operator*=(ST arg) {
	*this = *this * arg;
	return (AT&)*this;
}

template <typename AT, typename RT, typename ST>
AT &absolute_ne_se_up_coord<AT, RT, ST, 2>::operator*=(ST arg) {
	*this = *this * arg;
	return (AT&)*this;
}

template <typename AT, typename RT, typename ST>
AT &absolute_ne_se_up_coord<AT, RT, ST, 3>::operator*=(ST arg) {
	*this = *this * arg;
	return (AT&)*this;
}

//abs /= scalar
template <typename AT, typename RT, typename ST>
AT &absolute_ne_se_up_coord<AT, RT, ST, 1>::operator/=(ST arg) {
	*this = *this / arg;
	return (AT&)*this;
}
template <typename AT, typename RT, typename ST>
AT &absolute_ne_se_up_coord<AT, RT, ST, 2>::operator/=(ST arg) {
	*this = *this / arg;
	return (AT&)*this;
}
template <typename AT, typename RT, typename ST>
AT &absolute_ne_se_up_coord<AT, RT, ST, 3>::operator/=(ST arg) {
	*this = *this / arg;
	return (AT&)*this;
}

//abs == abs
template <typename AT, typename RT, typename ST>
bool absolute_ne_se_up_coord<AT, RT, ST, 1>::operator==(AT arg) const {
	return this->ne == arg.ne;
}

template <typename AT, typename RT, typename ST>
bool absolute_ne_se_up_coord<AT, RT, ST, 2>::operator==(AT arg) const {
	return this->ne == arg.ne && this->se == arg.se;
}

template <typename AT, typename RT, typename ST>
bool absolute_ne_se_up_coord<AT, RT, ST, 3>::operator==(AT arg) const {
	return this->ne == arg.ne && this->se == arg.se && this->up == arg.up;
}


template<typename AT, typename RT, typename ST, unsigned int N>
struct relative_ne_se_up_coord : public ne_se_up_coord<ST, N> {
};

template<typename AT, typename RT, typename ST>
struct relative_ne_se_up_coord<AT, RT, ST, 1> : public ne_se_up_coord<ST, 1> {
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
struct relative_ne_se_up_coord<AT, RT, ST, 2> : public ne_se_up_coord<ST, 2> {
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
struct relative_ne_se_up_coord<AT, RT, ST, 3> : public ne_se_up_coord<ST, 3> {
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
AT relative_ne_se_up_coord<AT, RT, ST, 1>::as_absolute() const {
	return AT{this->ne};
}

template<typename AT, typename RT, typename ST>
AT relative_ne_se_up_coord<AT, RT, ST, 2>::as_absolute() const {
	return AT{this->ne, this->se};
}

template<typename AT, typename RT, typename ST>
AT relative_ne_se_up_coord<AT, RT, ST, 3>::as_absolute() const {
	return AT{this->ne, this->se, this->up};
}

//-rel -> rel
template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 1>::operator-() const {
	return RT{-this->ne};
}

template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 2>::operator-() const {
	return RT{-this->ne, -this->se};
}

template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 3>::operator-() const {
	return RT{-this->ne, -this->se, -this->up};
}

//rel + abs -> abs
template<typename AT, typename RT, typename ST>
AT relative_ne_se_up_coord<AT, RT, ST, 1>::operator+(AT arg) const {
	return AT{this->ne + arg.ne};
}

template<typename AT, typename RT, typename ST>
AT relative_ne_se_up_coord<AT, RT, ST, 2>::operator+(AT arg) const {
	return AT{this->ne + arg.ne, this->se + arg.se};
}

template<typename AT, typename RT, typename ST>
AT relative_ne_se_up_coord<AT, RT, ST, 3>::operator+(AT arg) const {
	return AT{this->ne + arg.ne, this->se + arg.se, this->up + arg.up};
}

//rel + rel -> rel
template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 1>::operator+(RT arg) const {
	return RT{this->ne + arg.ne};
}

template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 2>::operator+(RT arg) const {
	return RT{this->ne + arg.ne, this->se + arg.se};
}

template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 3>::operator+(RT arg) const {
	return RT{this->ne + arg.ne, this->se + arg.se, this->up + arg.up};
}

//rel - rel -> rel
template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 1>::operator-(RT arg) const {
	return RT{this->ne - arg.ne};
}

template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 2>::operator-(RT arg) const {
	return RT{this->ne - arg.ne, this->se - arg.se};
}

template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 3>::operator-(RT arg) const {
	return RT{this->ne - arg.ne, this->se - arg.se, this->up - arg.up};
}

//rel * scalar -> rel
template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 1>::operator*(ST arg) const {
	return RT{this->ne * arg};
}

template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 2>::operator*(ST arg) const {
	return RT{this->ne * arg, this->se * arg};
}

template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 3>::operator*(ST arg) const {
	return RT{this->ne * arg, this->se * arg, this->up * arg};
}

//rel / scalar -> rel
template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 1>::operator/(ST arg) const {
	return RT{this->ne / arg};
}

template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 2>::operator/(ST arg) const {
	return RT{this->ne / arg, this->se / arg};
}

template<typename AT, typename RT, typename ST>
RT relative_ne_se_up_coord<AT, RT, ST, 3>::operator/(ST arg) const {
	return RT{this->ne / arg, this->se / arg, this->up / arg};
}

//rel += rel
template<typename AT, typename RT, typename ST>
RT &relative_ne_se_up_coord<AT, RT, ST, 1>::operator+=(RT arg) {
	*this = *this + arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_ne_se_up_coord<AT, RT, ST, 2>::operator+=(RT arg) {
	*this = *this + arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_ne_se_up_coord<AT, RT, ST, 3>::operator+=(RT arg) {
	*this = *this + arg;
	return (RT&)*this;
}

//rel -= rel
template<typename AT, typename RT, typename ST>
RT &relative_ne_se_up_coord<AT, RT, ST, 1>::operator-=(RT arg) {
	*this = *this - arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_ne_se_up_coord<AT, RT, ST, 2>::operator-=(RT arg) {
	*this = *this - arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_ne_se_up_coord<AT, RT, ST, 3>::operator-=(RT arg) {
	*this = *this - arg;
	return (RT&)*this;
}

//rel *= scalar
template<typename AT, typename RT, typename ST>
RT &relative_ne_se_up_coord<AT, RT, ST, 1>::operator*=(ST arg) {
	*this = *this * arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_ne_se_up_coord<AT, RT, ST, 2>::operator*=(ST arg) {
	*this = *this * arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_ne_se_up_coord<AT, RT, ST, 3>::operator*=(ST arg) {
	*this = *this * arg;
	return (RT&)*this;
}

//rel /= scalar
template<typename AT, typename RT, typename ST>
RT &relative_ne_se_up_coord<AT, RT, ST, 1>::operator/=(ST arg) {
	*this = *this / arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_ne_se_up_coord<AT, RT, ST, 2>::operator/=(ST arg) {
	*this = *this / arg;
	return (RT&)*this;
}

template<typename AT, typename RT, typename ST>
RT &relative_ne_se_up_coord<AT, RT, ST, 3>::operator/=(ST arg) {
	*this = *this / arg;
	return (RT&)*this;
}

//rel == rel
template<typename AT, typename RT, typename ST>
bool relative_ne_se_up_coord<AT, RT, ST, 1>::operator==(RT arg) const {
	return this->ne == arg.ne;
}

template<typename AT, typename RT, typename ST>
bool relative_ne_se_up_coord<AT, RT, ST, 2>::operator==(RT arg) const {
	return this->ne == arg.ne && this->se == arg.se;
}

template<typename AT, typename RT, typename ST>
bool relative_ne_se_up_coord<AT, RT, ST, 3>::operator==(RT arg) const {
	return this->ne == arg.ne && this->se == arg.se && this->up == arg.up;
}

} // namespace coord
} // namespace openage

#endif
