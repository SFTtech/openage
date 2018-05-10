// Copyright 2013-2017 the openage authors. See copying.md for legal info.

#pragma once

#define LOOP_1(MACRO, X)       MACRO(X)
#define LOOP_2(MACRO, X, Y)    MACRO(X), MACRO(Y)
#define LOOP_3(MACRO, X, Y, Z) MACRO(X), MACRO(Y), MACRO(Z)

#define LOOP_N(_1, _2, _3, NAME, ...) NAME

#ifdef _MSC_VER
#define LOOP_COUNT_ARGS_IMPL(args) LOOP_N args
#define LOOP_COUNT_ARGS(...)       LOOP_COUNT_ARGS_IMPL((__VA_ARGS__, 3, 2, 1))
#define LOOP_HELPER2(count) LOOP_##count
#define LOOP_HELPER1(count) LOOP_HELPER2(count)
#define LOOP_HELPER(count)  LOOP_HELPER1(count)
#define LOOP_GLUE(x, y) x y
#define LOOP(MACRO, ...) LOOP_GLUE(LOOP_HELPER(LOOP_COUNT_ARGS(__VA_ARGS__)), (MACRO, __VA_ARGS__))
#else
#define LOOP(MACRO, ...) LOOP_N(__VA_ARGS__, \
	LOOP_3, LOOP_2, LOOP_1 \
	) (MACRO, __VA_ARGS__)
#endif
