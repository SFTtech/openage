// Copyright 2013-2015 the openage authors. See copying.md for legal info.

// has no header guard: used in a preprocessor hack
// TODO obsolete this file using the python codegen

	//operators for RELATIVE_TYPE

	ABSOLUTE_TYPE as_absolute() const;
	RELATIVE_TYPE operator-() const;
	ABSOLUTE_TYPE operator+(ABSOLUTE_TYPE arg) const;
	RELATIVE_TYPE operator+(RELATIVE_TYPE arg) const;
	RELATIVE_TYPE operator-(RELATIVE_TYPE arg) const;
	RELATIVE_TYPE operator*(SCALAR_TYPE arg) const;
	RELATIVE_TYPE operator/(SCALAR_TYPE arg) const;
	RELATIVE_TYPE &operator+=(RELATIVE_TYPE arg);
	RELATIVE_TYPE &operator-=(RELATIVE_TYPE arg);
	RELATIVE_TYPE &operator*=(SCALAR_TYPE arg);
	RELATIVE_TYPE &operator/=(SCALAR_TYPE arg);
	bool operator==(RELATIVE_TYPE arg) const;
