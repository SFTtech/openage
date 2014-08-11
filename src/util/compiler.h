#ifndef _OPENAGE_UTIL_COMPILER_H_
#define _OPENAGE_UTIL_COMPILER_H_

/*
 * branch prediction tuning.
 * the expression is expected to be true (=likely) or false (=unlikely).
 *
 * btw, this implementation was taken from the Linux kernel.
 */
#define likely(x)    __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)

#endif // _OPENAGE_UTIL_COMPILER_H_
