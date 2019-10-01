# Copyright 2015-2019 the openage authors. See copying.md for legal info.

include(CheckCXXSourceCompiles)

if(cxx_thread_local IN_LIST CMAKE_CXX_COMPILE_FEATURES)
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

The compiler doesn't support required C++17 features:
  * Nested namespace definition (N4230)
  * Structured Bindings (P0217R3)
The following versions support these features:
  * clang++ >= 5
  * g++ >= 7
  * Microsoft Visual Studio 2017 >= 15.5
Please upgrade your compiler to build openage.

")
	message(FATAL_ERROR "aborting")
endif()
