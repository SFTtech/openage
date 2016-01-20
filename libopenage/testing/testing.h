// Copyright 2014-2016 the openage authors. See copying.md for legal info.

#pragma once

#include "../error/error.h"

namespace openage {
namespace testing {


/**
 * To be used by all tests to indicate an error condition.
 *
 * An optional error message may be passed as part of the message,
 * but the file name and line number are of primary concern.
 */
class TestError : public Error {
	using Error::Error;
};


/**
 * Raises TestError with the message msg.
 * Mostly designed for usage with the TESTFAIL macro.
 */
bool fail(const log::message &msg);


/**
 * Convenience macro, for usage in test functions:
 *
 * test_stuff() or TESTFAIL;
 */
#define TESTFAIL ::openage::testing::fail(MSG(err))


/**
 * As above, but allows printing a message:
 *
 * test_stuff() or TESTFAIL("lolnope");
 */
#define TESTFAILMSG(...) ::openage::testing::fail(MSG(err) << __VA_ARGS__)


/**
 * Asserts that the left expression equals the right expression,
 * and that no exception is thrown.
 */
#define TESTEQUALS(left, right) \
	do { \
		try { \
			auto &&test_result_left = (left); \
			if (test_result_left != (right)) { \
				TESTFAILMSG("unexpected value: " << (test_result_left)); \
			} \
		} catch (::openage::testing::TestError &e) { \
			throw; \
		} catch (::openage::error::Error &e) { \
			TESTFAILMSG("unexpected exception: " << e); \
		} \
	} while (0)


/**
 * Asserts that the left expression equals the right expression,
 * within a margin of error epsilon, and that no exception is thrown.
 */
#define TESTEQUALS_FLOAT(left, right, epsilon) \
	do { \
		try { \
			auto &&test_result_left = (left); \
			if ((test_result_left < (right - epsilon)) or \
			    (test_result_left > (right + epsilon))) { \
				TESTFAILMSG("unexpected value: " << (test_result_left)); \
			} \
		} catch (::openage::testing::TestError &e) { \
			throw; \
		} catch (::openage::error::Error &e) { \
			TESTFAILMSG("unexpected exception: " << e); \
		} \
	} while (0)


/**
 * Asserts that the expression throws an exception.
 */
#define TESTTHROWS(expression) \
	do { \
		bool expr_has_thrown = false; \
		try { \
			expression; \
		} catch (::openage::error::Error &e) { \
			expr_has_thrown = true; \
		} \
		if (not expr_has_thrown) { \
			TESTFAILMSG("no exception"); \
		} \
	} while (0)


/**
 * Assets that the expression throws no exception (mostly for void expressions).
 */
#define TESTNOEXCEPT(expression) \
	do { \
		try { \
			expression; \
		} catch (::openage::error::Error &e) { \
			TESTFAILMSG("unexpected exception"); \
		} \
	} while (0)


}} // openage::testing
