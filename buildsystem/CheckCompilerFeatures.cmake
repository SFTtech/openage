# Copyright 2015-2016 the openage authors. See copying.md for legal info.

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("thread_local int i = 0; int main() { return i; }" HAVE_THREAD_LOCAL_STORAGE)
if(HAVE_THREAD_LOCAL_STORAGE)
	set(HAVE_THREAD_LOCAL_STORAGE true)
else()
	set(HAVE_THREAD_LOCAL_STORAGE false)
endif()
