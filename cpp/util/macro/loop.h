#ifndef OPENAGE_UTIL_MACRO_LOOP_H_
#define OPENAGE_UTIL_MACRO_LOOP_H_

#define LOOP_1(MACRO, X) MACRO(X)
#define LOOP_2(MACRO, X, ...) MACRO(X), LOOP_1(MACRO, __VA_ARGS__)
#define LOOP_3(MACRO, X, ...) MACRO(X), LOOP_2(MACRO, __VA_ARGS__)
#define LOOP_4(MACRO, X, ...) MACRO(X), LOOP_3(MACRO, __VA_ARGS__)
#define LOOP_5(MACRO, X, ...) MACRO(X), LOOP_4(MACRO, __VA_ARGS__)

#define LOOP_N(_1, _2, _3, _4, _5, NAME, ...) NAME
#define LOOP(MACRO, ...) LOOP_N(__VA_ARGS__, \
	LOOP_5, LOOP_4, LOOP_3, LOOP_2, LOOP_1 \
	) (MACRO, __VA_ARGS__)

#endif
