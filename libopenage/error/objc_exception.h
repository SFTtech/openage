// Copyright 2025 the openage authors. See copying.md for legal info.

#pragma once

#include <string>

namespace openage {
namespace error {

/**
 * Install a process-wide Objective-C exception preprocessor (Apple platforms
 * only; no-op elsewhere) that records the most recent NSException thrown on
 * each thread, so the terminate handler can later report it.
 *
 * Must be paired with uninstall_objc_exception_tracker(). Typically called
 * once at library load and uninstalled at library unload.
 *
 * The preprocessor only observes exceptions (it returns the exception
 * unchanged) so it does not alter program behaviour.
 */
void install_objc_exception_tracker();

/**
 * Undo install_objc_exception_tracker(). No-op on non-Apple platforms.
 */
void uninstall_objc_exception_tracker();

/**
 * Return a dump (name, reason, callStackSymbols) of the most recent NSException
 * recorded on the current thread, or an empty string if there was none.
 *
 * This does NOT rethrow or otherwise disturb the in-flight exception, so it is
 * safe to call from a catch(...) arm of a terminate handler. (An earlier
 * implementation rethrew via objc_exception_rethrow + @catch; that disturbed the
 * C++ exception runtime and turned the subsequent std::terminate() into an
 * infinite std::terminate <-> __terminate loop instead of aborting.)
 */
std::string dump_objc_exception();

} // namespace error
} // namespace openage
