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

if(NOT HAVE_REQUIRED_CXX20_SUPPORT)
	message("

The compiler doesn't support required C++20 features:
  * Concepts
  * Default comparisons
The minimum supported compiler versions are:
  * clang++ >= 21
  * g++ >= 16
  * Microsoft Visual Studio 2022 >= 17.4
Please upgrade your compiler to build openage.

")
	message(FATAL_ERROR "aborting")
endif()
