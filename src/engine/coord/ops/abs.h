	//operators for ABSOLUTE_TYPE

	RELATIVE_TYPE as_relative();
	ABSOLUTE_TYPE operator-();
	ABSOLUTE_TYPE operator+(RELATIVE_TYPE arg);
	RELATIVE_TYPE operator-(ABSOLUTE_TYPE arg);
	ABSOLUTE_TYPE operator-(RELATIVE_TYPE arg);
	ABSOLUTE_TYPE operator*(SCALAR_TYPE arg);
	ABSOLUTE_TYPE operator/(SCALAR_TYPE arg);
	ABSOLUTE_TYPE &operator+=(RELATIVE_TYPE arg);
	ABSOLUTE_TYPE &operator-=(RELATIVE_TYPE arg);
	ABSOLUTE_TYPE &operator*=(SCALAR_TYPE arg);
	ABSOLUTE_TYPE &operator/=(SCALAR_TYPE arg);
	bool operator==(ABSOLUTE_TYPE arg);
