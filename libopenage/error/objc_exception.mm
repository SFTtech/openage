#include "objc_exception.h"

#ifdef __APPLE__

#import <Foundation/Foundation.h>
#include <objc/objc-exception.h>
#include <sstream>
#include <string>

namespace openage {
namespace error {

namespace {

// The most recent NSException thrown on the current thread, captured by the
// preprocessor at throw time. Held as a raw pointer (non-ARC); while an
// exception is in flight and propagating to terminate(), the runtime keeps it
// alive via the C++ exception object, so this is valid to read in the
// terminate handler.
thread_local NSException *last_objc_exception = nil;

// Call stack captured at throw time as a plain string (independent of ARC
// reference counting). NSException's own callStackSymbols may not be populated
// yet when the preprocessor runs, so we grab [NSThread callStackSymbols]
// ourselves at the throw point.
thread_local std::string last_objc_callstack;

// Previous preprocessor, restored on uninstall so we do not clobber a host
// application's own ObjC exception preprocessor.
objc_exception_preprocessor previous_preprocessor = nullptr;

} // namespace

// Invoked by objc_exception_throw() for every NSException thrown. We record it
// per-thread (so the terminate handler, which runs on the throwing thread, can
// read it) and return the exception unchanged. Recording at throw time - rather
// than rethrowing inside catch(...) - means we never disturb the C++ exception
// runtime, so std::terminate()/abort() still terminates cleanly.
static id exception_preprocessor(id exception) {
	if (exception != nil && [exception isKindOfClass:[NSException class]]) {
		last_objc_exception = (NSException *)exception;

		std::string s;
		for (NSString *symbol in [NSThread callStackSymbols]) {
			s += "    ";
			s += [symbol UTF8String];
			s += "\n";
		}
		last_objc_callstack = std::move(s);
	}
	return exception;
}

void install_objc_exception_tracker() {
	previous_preprocessor = objc_setExceptionPreprocessor(exception_preprocessor);
}

void uninstall_objc_exception_tracker() {
	objc_setExceptionPreprocessor(previous_preprocessor);
}

std::string dump_objc_exception() {
	NSException *exc = last_objc_exception;
	if (exc == nil) {
		return "";
	}

	std::ostringstream ss;
	ss << "Objective-C NSException caught across C++ boundary:\n";
	ss << "  name:   " << [[exc name] UTF8String] << "\n";
	ss << "  reason: " << [[exc reason] UTF8String] << "\n";
	if (!last_objc_callstack.empty()) {
		ss << "  callStackSymbols (at throw):\n" << last_objc_callstack;
	}
	return ss.str();
}

} // namespace error
} // namespace openage

#else  // !__APPLE__

namespace openage {
namespace error {

void install_objc_exception_tracker() {
}

void uninstall_objc_exception_tracker() {
}

std::string dump_objc_exception() {
	return "";
}

} // namespace error
} // namespace openage

#endif  // __APPLE__
