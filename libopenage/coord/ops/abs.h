// Copyright 2013-2015 the openage authors. See copying.md for legal info.

// has no header guard: used in a preprocessor hack
// TODO obsolete this file using the python codegen

	//operators for ABSOLUTE_TYPE

	RELATIVE_TYPE as_relative() const;
	ABSOLUTE_TYPE operator-() const;
	ABSOLUTE_TYPE operator+(RELATIVE_TYPE arg) const;
	RELATIVE_TYPE operator-(ABSOLUTE_TYPE arg) const;
	ABSOLUTE_TYPE operator-(RELATIVE_TYPE arg) const;
	ABSOLUTE_TYPE operator*(SCALAR_TYPE arg) const;
	ABSOLUTE_TYPE operator/(SCALAR_TYPE arg) const;
	ABSOLUTE_TYPE &operator+=(RELATIVE_TYPE arg);
	ABSOLUTE_TYPE &operator-=(RELATIVE_TYPE arg);
	ABSOLUTE_TYPE &operator*=(SCALAR_TYPE arg);
	ABSOLUTE_TYPE &operator/=(SCALAR_TYPE arg);
	bool operator==(ABSOLUTE_TYPE arg) const;
