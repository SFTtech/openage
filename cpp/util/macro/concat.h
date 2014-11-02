// Copyright 2013-2014 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_MACRO_CONCAT_H_
#define OPENAGE_UTIL_MACRO_CONCAT_H_

#define CONCAT_1(OP, X)      (X)
#define CONCAT_2(OP, X, ...) (X) OP CONCAT_1(OP, __VA_ARGS__)
#define CONCAT_3(OP, X, ...) (X) OP CONCAT_2(OP, __VA_ARGS__)
#define CONCAT_4(OP, X, ...) (X) OP CONCAT_3(OP, __VA_ARGS__)
#define CONCAT_5(OP, X, ...) (X) OP CONCAT_4(OP, __VA_ARGS__)

#define CONCAT_N(_1, _2, _3, _4, _5, NAME, ...) NAME
#define CONCAT(OP, ...) CONCAT_N(__VA_ARGS__, \
	CONCAT_5, CONCAT_4, CONCAT_3, CONCAT_2, CONCAT_1 \
	) (OP, __VA_ARGS__)

#endif
