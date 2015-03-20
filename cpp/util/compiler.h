// Copyright 2014-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_COMPILER_H_
#define OPENAGE_UTIL_COMPILER_H_

/*
 * Branch prediction tuning.
 * the expression is expected to be true (=likely) or false (=unlikely).
 *
 * btw, this implementation was taken from the Linux kernel.
 */
#define likely(x)    __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)

/*
 * With older compilers, thread_local does not work properly yet.
 */
#if defined(__APPLE__) && defined(__clang__)
  #define thread_local __thread
#endif

#endif
