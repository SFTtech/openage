#pragma once

#include <vulkan/vulkan.h>

#include "../../error/error.h"


template <typename R, typename R2>
std::vector<R> vk_do_ritual(R2 (*func)(uint32_t*, R*)) {
	uint32_t count = 0;
	func(&count, nullptr);
	std::vector<R> ret(count);
	func(&count, ret.data());

	return ret;
}

template <typename T, typename T2, typename R, typename R2>
std::vector<R> vk_do_ritual(R2 (*func)(T, uint32_t*, R*), T2&& a) {
	uint32_t count = 0;
	func(std::forward<T2>(a), &count, nullptr);
	std::vector<R> ret(count);
	func(std::forward<T2>(a), &count, ret.data());

	return ret;
}

template <typename T, typename T2, typename U, typename U2, typename R, typename R2>
std::vector<R> vk_do_ritual(R2 (*func)(T, U, uint32_t*, R*), T2&& a, U2&& b) {
	uint32_t count = 0;
	func(std::forward<T2>(a), std::forward<U2>(b), &count, nullptr);
	std::vector<R> ret(count);
	func(std::forward<T2>(a), std::forward<U2>(b), &count, ret.data());

	return ret;
}

template <typename T, typename T2, typename U, typename U2, typename V, typename V2, typename R, typename R2>
std::vector<R> vk_do_ritual(R2 (*func)(T, U, V, uint32_t*, R*), T2&& a, U2&& b, V2&& c) {
	uint32_t count = 0;
	func(std::forward<T2>(a), std::forward<U2>(b), std::forward<V2>(c), &count, nullptr);
	std::vector<R> ret(count);
	func(std::forward<T2>(a), std::forward<U2>(b), std::forward<V2>(c), &count, ret.data());

	return ret;
}

#define VK_CALL_CHECKED(fun, ...)                                       \
	{ \
		VkResult res = fun(__VA_ARGS__); \
		if (res != VK_SUCCESS) { \
		} \
	}

//throw Error(MSG(err) << "Call to Vulkan function " << #fun << " failed with " << vk::to_string(vk::Result(res)) << "."); \
//
