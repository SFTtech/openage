# Copyright 2015-2018 the openage authors. See copying.md for legal info.

include(CheckCXXSourceCompiles)
check_cxx_source_compiles("thread_local int i = 0; int main() { return i; }" HAVE_THREAD_LOCAL_STORAGE)
if(HAVE_THREAD_LOCAL_STORAGE)
	set(HAVE_THREAD_LOCAL_STORAGE true)
else()
	set(HAVE_THREAD_LOCAL_STORAGE false)
endif()

check_cxx_source_compiles("
namespace nested::check {
struct A {
	int i;
};
} // namespace nested::check
int main() {
	nested::check::A a{10};
	auto [i] = a;
	return 10 - i;
}
"
HAVE_REQUIRED_CXX17_SUPPORT
)
if(NOT HAVE_REQUIRED_CXX17_SUPPORT)
	message("

The compiler doesn't support required C++17 features.
Please upgrade your compiler to build openage.

")
	message(FATAL_ERROR "aborting")
endif()
