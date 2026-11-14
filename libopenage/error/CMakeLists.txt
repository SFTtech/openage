add_sources(libopenage
	backtrace.cpp
	demo.cpp
	error.cpp
	handlers.cpp
	stackanalyzer.cpp
)

# Objective-C++ source for Apple-specific exception handling (NSException
# dump from the terminate handler). Only built on Apple platforms; the .mm is
# compiled as Objective-C++ via the OBJCXX language enabled in the top-level
# CMakeLists.txt.
if(APPLE)
	add_sources(libopenage
		objc_exception.mm
	)
endif()

pxdgen(
	backtrace.h
	error.h
	handlers.h
)
