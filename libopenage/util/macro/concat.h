// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#pragma once

#define CONCAT_1(OP, X)       (X)
#define CONCAT_2(OP, X, Y)    (X) OP (Y)
#define CONCAT_3(OP, X, Y, Z) (X) OP (Y) OP (Z)

#define CONCAT_N(_1, _2, _3, NAME, ...) NAME

#ifdef _MSC_VER
#define CONCAT_COUNT_ARGS_IMPL(args) CONCAT_N args
#define CONCAT_COUNT_ARGS(...)       CONCAT_COUNT_ARGS_IMPL((__VA_ARGS__, 3, 2, 1))
#define CONCAT_HELPER2(count) CONCAT_##count
#define CONCAT_HELPER1(count) CONCAT_HELPER2(count)
#define CONCAT_HELPER(count)  CONCAT_HELPER1(count)
#define CONCAT_GLUE(x, y) x y
#define CONCAT(OP, ...) CONCAT_GLUE(CONCAT_HELPER(CONCAT_COUNT_ARGS(__VA_ARGS__)), (OP, __VA_ARGS__))
#else
#define CONCAT(OP, ...) CONCAT_N(__VA_ARGS__, \
	CONCAT_3, CONCAT_2, CONCAT_1 \
	) (OP, __VA_ARGS__)
#endif
