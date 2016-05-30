// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <type_traits>

#include "qtsdl_checked_static_cast.h"

namespace qtsdl {

/**
 * Base for QObject wrapper of the domain-specific classes.
 */
class GuiItemLink {
#ifndef NDEBUG
public:
	virtual ~GuiItemLink() {
	}
#endif
};

/**
 * If the core 'MyClass' has a shell 'MyClassLink' then 'Wrap<MyClass>' must have a 'using Type = MyClassLink'
 */
template<typename U>
struct Wrap {
};

/**
 * If the core 'MyClass' has a shell 'MyClassLink' then 'Unwrap<MyClassLink>' must have a 'using Type = MyClass'
 */
template<typename T>
struct Unwrap {
};

template<typename T>
typename Unwrap<T>::Type* unwrap(T *t) {
	return t ? t->template get<typename Unwrap<T>::Type>() : nullptr;
}

template<typename T>
const typename Unwrap<T>::Type* unwrap(const T *t) {
	return t ? t->template get<typename Unwrap<T>::Type>() : nullptr;
}

template<typename U>
const typename Wrap<U>::Type* wrap(const U *u) {
	return checked_static_cast<typename Wrap<U>::Type*>(u->gui_link);
}

/**
 * QML singletons should be unwrapped differently because several QML singletons can point to one C++ singleton/object. But not implementing that for now.
 */
class GuiSingletonItem;

template<typename U>
typename Wrap<U>::Type* wrap(U *u, typename std::enable_if<!std::is_base_of<GuiSingletonItem, typename Wrap<U>::Type>::value>::type* = nullptr) {
	return u ? checked_static_cast<typename Wrap<U>::Type*>(u->gui_link) : nullptr;
}

template<typename U>
typename Wrap<U>::Type* wrap(U *u, typename std::enable_if<std::is_base_of<GuiSingletonItem, typename Wrap<U>::Type>::value>::type* = nullptr) {
	return u ? checked_static_cast<typename Wrap<U>::Type*>(u->gui_link) : nullptr;
}

template<typename P>
constexpr P&& wrap_if_can(typename std::remove_reference<P>::type&& p) noexcept {
	return std::forward<P>(p);
}

template<typename T>
T wrap_if_can(typename Unwrap<typename std::remove_pointer<T>::type>::Type *u) {
	return wrap(u);
}

template<typename P>
P unwrap_if_can(P& p) {
	return p;
}

template<typename T, typename Unwrap<T>::Type* = nullptr>
typename Unwrap<T>::Type* unwrap_if_can(T *t) {
	return unwrap(t);
}

} // namespace qtsdl
