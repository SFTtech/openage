# Copyright 2015-2021 the openage authors. See copying.md for legal info.

include(CheckCXXSourceRuns)

if(cxx_thread_local IN_LIST CMAKE_CXX_COMPILE_FEATURES)
	set(HAVE_THREAD_LOCAL_STORAGE true)
else()
	set(HAVE_THREAD_LOCAL_STORAGE false)
endif()

check_cxx_source_runs("
#include <compare>
#include <concepts>

class Cat {
public:
	virtual int maunz() const { return 42; };

	auto operator <=>(const Cat &other) const = default;
};

template <typename T>
concept Meowable = std::same_as<T, Cat> || std::derived_from<T, Cat>;

template <Meowable T>
int meow(const T &cat) {
	return cat.maunz();
}

class HouseCat : public Cat {
public:
	int maunz() const override { return 1337; };
};

int main() {
	HouseCat mrr{};
	Cat rrarrr{};
	return meow(mrr) > meow(rrarrr) ? 0 : 1;
}
"
HAVE_REQUIRED_CXX20_SUPPORT
)


# probe the compiler for C++ module support (CXX_MODULES file sets)
set(cxx_module_check "${CMAKE_CURRENT_BINARY_DIR}/check_cxx_module_support")
file(WRITE "${cxx_module_check}/CMakeLists.txt" [=[
cmake_minimum_required(VERSION 3.28)
project(cxx_module_check LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 26)
add_library(cxx_module_check STATIC)
target_sources(cxx_module_check
	PUBLIC
		FILE_SET CXX_MODULES
		BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
		FILES cxx_module_check.cppm
)
add_executable(cxx_module_use cxx_module_use.cpp)
target_link_libraries(cxx_module_use PRIVATE cxx_module_check)
]=])

file(WRITE "${cxx_module_check}/cxx_module_check.cppm" [=[
export module cxx_module_check;
export int cxx_module_check_answer() { return 42; }
]=])

file(WRITE "${cxx_module_check}/cxx_module_use.cpp" [=[
import cxx_module_check;
int main() {
	return cxx_module_check_answer() == 42 ? 0 : 1;
}
]=])

try_compile(HAVE_CXX_MODULE_SUPPORT
	PROJECT cxx_module_check
	SOURCE_DIR "${cxx_module_check}"
	BINARY_DIR "${cxx_module_check}-build"
)

if(NOT HAVE_REQUIRED_CXX20_SUPPORT OR NOT HAVE_CXX_MODULE_SUPPORT)
	message("
The compiler doesn't support at least one of the required features:
  * Concepts: ${HAVE_REQUIRED_CXX20_SUPPORT}
  * Default comparisons: ${HAVE_REQUIRED_CXX20_SUPPORT}
  * C++ Modules: ${HAVE_CXX_MODULE_SUPPORT}

The minimum supported compiler versions are:
  * clang++ >= 21
  * g++ >= 16
  * Microsoft Visual Studio 2022 >= 17.4
Please upgrade your compiler to build openage.

")
	message(FATAL_ERROR "aborting")
endif()
