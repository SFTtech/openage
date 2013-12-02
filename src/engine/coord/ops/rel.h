	//operators for RELATIVE_TYPE

	ABSOLUTE_TYPE as_absolute();
	RELATIVE_TYPE operator-();
	ABSOLUTE_TYPE operator+(ABSOLUTE_TYPE arg);
	RELATIVE_TYPE operator+(RELATIVE_TYPE arg);
	RELATIVE_TYPE operator-(RELATIVE_TYPE arg);
	RELATIVE_TYPE operator*(SCALAR_TYPE arg);
	RELATIVE_TYPE operator/(SCALAR_TYPE arg);
	RELATIVE_TYPE &operator+=(RELATIVE_TYPE arg);
	RELATIVE_TYPE &operator-=(RELATIVE_TYPE arg);
	RELATIVE_TYPE &operator*=(SCALAR_TYPE arg);
	RELATIVE_TYPE &operator/=(SCALAR_TYPE arg);
	bool operator==(RELATIVE_TYPE arg);
