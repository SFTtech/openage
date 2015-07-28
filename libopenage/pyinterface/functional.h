// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_PYINTERFACE_FUNCTIONAL_H_
#define OPENAGE_PYINTERFACE_FUNCTIONAL_H_

#include <functional>
#include <mutex>

#include "../util/compiler.h"
#include "../util/language.h"

#include "exctranslate.h"
#include "setup.h"


namespace openage {
namespace pyinterface {


/**
 * Holds a function pointer, with bound arguments.
 *
 * Can be filled via its implicit copy constructor, operator = or bind().
 *
 * operator = is designed for usage from C++, and accepts
 * set() sets it to an existing std::function object one of its bind()
 * members.
 *
 * It's auto-converted to std::function, and can be called via call().
 *
 * Designed to allow Cython to call C++ functions that accept std::function
 * callbacks, and to provide a callback function type for Cython to offer to
 * C++.
 *
 * As of right now, Cython doesn't support variadic templates
 * [https://mail.python.org/pipermail/cython-devel/2015-June/004429.html].
 *
 * Thus, Func has aliases to allow usage like regular templated types,
 * one for each number of arguments:
 *
 *   from libopenage.pyinterface.functional import Func2
 *   void myfunc(float a0, int a1, int a2): pass
 *   Func2[void, int, int] fobj
 *   Func2.bind1[float](myfunc, 5.0)
 *
 * There are definitions up to Func5 and bind3;
 * if you encounter a situation where you'd need more, maybe you should
 * re-think some of your life choices.
 *
 * Note that you shouldn't use this type purely internally in Cython, since
 * if you bind a Cython function and call it from Cython, every Exception
 * would be needlessly converted from Py to C++ to Py...
 *
 * For global function pointers that should be initialized at pyinterface
 * initialization time, use PyIfFunc instead of Func; that class has some
 * additional code to verify successful initialization.
 */
template<typename ReturnType, typename ... ArgTypes>
class Func {
public:
	Func()
		:
		fptr{nullptr} {}

	// for construction from lambdas and other callables (from C++).
	template<typename F>
	Func(F &&f) {
		this->fptr = f;
	}

	template<typename F>
	Func(std::reference_wrapper<F> f) {
		this->fptr = f;
	}

	// for construction from std::function objects (from C++).
	Func(const std::function<ReturnType (ArgTypes ...)> &f) {
		this->fptr = f;
	}

	Func(std::function<ReturnType (ArgTypes ...)> &&f) {
		this->fptr = f;
	}

	// for assignment of lambdas and other callables (from C++).
	template<typename F>
	Func<ReturnType, ArgTypes ...> &operator =(F &&f) {
		this->fptr = f;
		return *this;
	}

	template<typename F>
	Func<ReturnType, ArgTypes ...> &operator =(std::reference_wrapper<F> f) {
		this->fptr = f;
		return *this;
	}

	// for assignment of std::function objects (from C++).
	Func<ReturnType, ArgTypes ...> &operator =(const std::function<ReturnType (ArgTypes ...)> &f) {
		this->fptr = f;
		return *this;
	}

	Func<ReturnType, ArgTypes ...> &operator =(std::function<ReturnType (ArgTypes ...)> &&f) {
		this->fptr = f;
		return *this;
	}

	/**
	 * raises an Error if this->fptr is still uninitialized.
	 */
	inline void check_fptr() const {
		if (unlikely(not this->fptr)) {
			throw Error(
				MSG(err) << "Uninitialized Func object at " <<
				util::symbol_name(static_cast<const void *>(this)) << ": "
				"Can not call or convert to std::function.",

				true // collect backtrace info
			);
		}
	}

	/**
	 * for direct usage (mostly from Cython)
	 */
	ReturnType call(ArgTypes ...args) const {
		this->check_fptr();
		return this->fptr(args...);
	}

	/**
	 * for implicit conversion to std::function,
	 * for usage in a context where std::function would be expected.
	 */
	operator const std::function<ReturnType (ArgTypes ...)> &() const {
		this->check_fptr();
		return this->fptr;
	}

	/**
	 * for explicit conversion to std::function.
	 */
	const std::function<ReturnType (ArgTypes ...)> &get() const {
		this->check_fptr();
		return this->fptr;
	}

	/**
	 * For manual binding of function pointers and arguments (from Cython; in C++, assign lambdas).
	 *
	 * Adds additional code that catches and converts Python exceptions to C++ exceptions.
	 *
	 * The 'util::FunctionPtr' argument is due to http://stackoverflow.com/questions/31040075.
	 * Note that with clang, it's possible to directly pass function pointers, while with
	 * gcc they need to be explicitly converted. Meh.
	 */
	template<typename ... BoundArgTypes>
	inline void bind(util::FunctionPtr<ReturnType, BoundArgTypes ..., ArgTypes ...> f, BoundArgTypes ...bound_args) {
		this->bind_catchexcept_impl<std::is_void<ReturnType>::value, BoundArgTypes ...>(f, bound_args...);
	}


private:
	/**
	 * Specialization for bind() with void return types.
	 */
	template<bool return_type_is_void, typename ... BoundArgTypes>
	inline typename std::enable_if<return_type_is_void>::type bind_catchexcept_impl(util::FunctionPtr<ReturnType, BoundArgTypes ..., ArgTypes ...> f, BoundArgTypes ...bound_args) {
		this->fptr = [=](ArgTypes ...args) -> ReturnType {
			f.ptr(bound_args..., args...);
			translate_exc_py_to_cpp();
		};
	}


	/**
	 * Specialization for bind() with non-void return types.
	 */
	template<bool return_type_is_void, typename ... BoundArgTypes>
	inline typename std::enable_if<not return_type_is_void>::type bind_catchexcept_impl(util::FunctionPtr<ReturnType, BoundArgTypes ..., ArgTypes ...> f, BoundArgTypes ...bound_args) {
		this->fptr = [=](ArgTypes ...args) -> ReturnType {
			ReturnType &&result = f.ptr(bound_args..., args...);
			translate_exc_py_to_cpp();
			return result;
		};
	}


public:
	/**
	 * Like bind, but does _not_ add an exception checker.
	 */
	template<typename ... BoundArgTypes>
	void bind_noexcept(util::FunctionPtr<ReturnType, BoundArgTypes ..., ArgTypes ...> f, BoundArgTypes ...bound_args) {
		this->fptr = [=](ArgTypes ...args) -> ReturnType {
			return f.ptr(bound_args..., args...);
		};
	}

	// non-variadic aliases for bind, for use by Cython
	inline void bind0(ReturnType (*f)(ArgTypes ...)) {
		this->bind<>(
			util::FunctionPtr<ReturnType, ArgTypes ...>(f));
	}

	inline void bind_noexcept0(ReturnType (*f)(ArgTypes ...)) {
		this->bind_noexcept<>(
			util::FunctionPtr<ReturnType, ArgTypes ...>(f));
	}

	template<typename BoundArgType0>
	inline void bind1(ReturnType (*f)(BoundArgType0, ArgTypes ...), BoundArgType0 bound_arg0) {
		this->bind<BoundArgType0>(
			util::FunctionPtr<ReturnType, BoundArgType0, ArgTypes ...>(f), bound_arg0);
	}

	template<typename BoundArgType0>
	inline void bind_noexcept1(ReturnType (*f)(BoundArgType0, ArgTypes ...), BoundArgType0 bound_arg0) {
		this->bind_noexcept<BoundArgType0>(
			util::FunctionPtr<ReturnType, BoundArgType0, ArgTypes ...>(f), bound_arg0);
	}

	template<typename BoundArgType0, typename BoundArgType1>
	inline void bind2(ReturnType (*f)(BoundArgType0, BoundArgType1, ArgTypes ...), BoundArgType0 bound_arg0, BoundArgType1 bound_arg1) {
		this->bind<BoundArgType0, BoundArgType1>(
			util::FunctionPtr<ReturnType, BoundArgType0, BoundArgType1, ArgTypes ...>(f), bound_arg0, bound_arg1);
	}

	template<typename BoundArgType0, typename BoundArgType1>
	inline void bind_noexcept2(ReturnType (*f)(BoundArgType0, BoundArgType1, ArgTypes ...), BoundArgType0 bound_arg0, BoundArgType1 bound_arg1) {
		this->bind_noexcept<BoundArgType0, BoundArgType1>(
			util::FunctionPtr<ReturnType, BoundArgType0, BoundArgType1, ArgTypes ...>(f), bound_arg0, bound_arg1);
	}

	template<typename BoundArgType0, typename BoundArgType1, typename BoundArgType2>
	inline void bind3(ReturnType (*f)(BoundArgType0, BoundArgType1, BoundArgType2, ArgTypes ...), BoundArgType0 bound_arg0, BoundArgType1 bound_arg1, BoundArgType2 bound_arg2) {
		this->bind<BoundArgType0, BoundArgType1, BoundArgType2>(
			util::FunctionPtr<ReturnType, BoundArgType0, BoundArgType1, BoundArgType2, ArgTypes ...>(f), bound_arg0, bound_arg1, bound_arg2);
	}

	template<typename BoundArgType0, typename BoundArgType1, typename BoundArgType2>
	inline void bind_noexcept3(ReturnType (*f)(BoundArgType0, BoundArgType1, BoundArgType2, ArgTypes ...), BoundArgType0 bound_arg0, BoundArgType1 bound_arg1, BoundArgType2 bound_arg2) {
		this->bind_noexcept<BoundArgType0, BoundArgType1, BoundArgType2>(
			util::FunctionPtr<ReturnType, BoundArgType0, BoundArgType1, BoundArgType2, ArgTypes ...>(f), bound_arg0, bound_arg1, bound_arg2);
	}

private:
	std::function<ReturnType (ArgTypes ...)> fptr;
};


/*
 * Now follow the glorious, aforementioned aliases for Cython, and accompanying pxd declarations.
 */

/*
 * No arguments.
 *
 * pxd:
 *
 * cppclass Func0[RT]:
 *     RT call() except +
 *
 *     void bind0                          (RT (*f)()              except * with gil               ) except +
 *     void bind1          [BT0]           (RT (*f)(BT0)           except * with gil, BT0          ) except +
 *     void bind2          [BT0, BT1]      (RT (*f)(BT0, BT1)      except * with gil, BT0, BT1     ) except +
 *     void bind3          [BT0, BT1, BT2] (RT (*f)(BT0, BT1, BT2) except * with gil, BT0, BT1, BT2) except +
 *
 *     void bind_noexcept0                 (RT (*f)()                       with gil               ) except +
 *     void bind_noexcept1 [BT0]           (RT (*f)(BT0)                    with gil, BT0          ) except +
 *     void bind_noexcept2 [BT0, BT1]      (RT (*f)(BT0, BT1)               with gil, BT0, BT1     ) except +
 *     void bind_noexcept3 [BT0, BT1, BT2] (RT (*f)(BT0, BT1, BT2)          with gil, BT0, BT1, BT2) except +
 */
template<typename RT>
using Func0 = Func<RT>;

/*
 * One argument.
 *
 * pxd:
 *
 * cppclass Func1[RT, AT0]:
 *     RT call(AT0) except +
 *
 *     void bind0                          (RT (*f)(AT0)                except * with gil               ) except +
 *     void bind1          [BT0]           (RT (*f)(BT0, AT0)           except * with gil, BT0          ) except +
 *     void bind2          [BT0, BT1]      (RT (*f)(BT0, BT1, AT0)      except * with gil, BT0, BT1     ) except +
 *     void bind3          [BT0, BT1, BT2] (RT (*f)(BT0, BT1, BT2, AT0) except * with gil, BT0, BT1, BT2) except +
 *
 *     void bind_noexcept0                 (RT (*f)(AT0)                         with gil               ) except +
 *     void bind_noexcept1 [BT0]           (RT (*f)(BT0, AT0)                    with gil, BT0          ) except +
 *     void bind_noexcept2 [BT0, BT1]      (RT (*f)(BT0, BT1, AT0)               with gil, BT0, BT1     ) except +
 *     void bind_noexcept3 [BT0, BT1, BT2] (RT (*f)(BT0, BT1, BT2, AT0)          with gil, BT0, BT1, BT2) except +
 */
template<typename RT, typename AT0>
using Func1 = Func<RT, AT0>;

/*
 * Two arguments.
 *
 * pxd:
 *
 * cppclass Func2[RT, AT0, AT1]:
 *     RT call(AT0, AT1) except +
 *
 *     void bind0                          (RT (*f)(AT0, AT1)                except * with gil               ) except +
 *     void bind1          [BT0]           (RT (*f)(BT0, AT0, AT1)           except * with gil, BT0          ) except +
 *     void bind2          [BT0, BT1]      (RT (*f)(BT0, BT1, AT0, AT1)      except * with gil, BT0, BT1     ) except +
 *     void bind3          [BT0, BT1, BT2] (RT (*f)(BT0, BT1, BT2, AT0, AT1) except * with gil, BT0, BT1, BT2) except +
 *
 *     void bind_noexcept0                 (RT (*f)(AT0, AT1)                         with gil               ) except +
 *     void bind_noexcept1 [BT0]           (RT (*f)(BT0, AT0, AT1)                    with gil, BT0          ) except +
 *     void bind_noexcept2 [BT0, BT1]      (RT (*f)(BT0, BT1, AT0, AT1)               with gil, BT0, BT1     ) except +
 *     void bind_noexcept3 [BT0, BT1, BT2] (RT (*f)(BT0, BT1, BT2, AT0, AT1)          with gil, BT0, BT1, BT2) except +
 */

template<typename RT, typename AT0, typename AT1>
using Func2 = Func<RT, AT0, AT1>;

/*
 * Three arguments.
 *
 * pxd:
 *
 * cppclass Func3[RT, AT0, AT1, AT2]:
 *     RT call(AT0, AT1, AT2) except +
 *
 *     void bind0                          (RT (*f)(AT0, AT1, AT2)                except * with gil               ) except +
 *     void bind1          [BT0]           (RT (*f)(BT0, AT0, AT1, AT2)           except * with gil, BT0          ) except +
 *     void bind2          [BT0, BT1]      (RT (*f)(BT0, BT1, AT0, AT1, AT2)      except * with gil, BT0, BT1     ) except +
 *     void bind3          [BT0, BT1, BT2] (RT (*f)(BT0, BT1, BT2, AT0, AT1, AT2) except * with gil, BT0, BT1, BT2) except +
 *
 *     void bind_noexcept0                 (RT (*f)(AT0, AT1, AT2)                         with gil               ) except +
 *     void bind_noexcept1 [BT0]           (RT (*f)(BT0, AT0, AT1, AT2)                    with gil, BT0          ) except +
 *     void bind_noexcept2 [BT0, BT1]      (RT (*f)(BT0, BT1, AT0, AT1, AT2)               with gil, BT0, BT1     ) except +
 *     void bind_noexcept3 [BT0, BT1, BT2] (RT (*f)(BT0, BT1, BT2, AT0, AT1, AT2)          with gil, BT0, BT1, BT2) except +
 */

template<typename RT, typename AT0, typename AT1, typename AT2>
using Func3 = Func<RT, AT0, AT1, AT2>;

/*
 * Four arguments.
 *
 * pxd:
 *
 * cppclass Func4[RT, AT0, AT1, AT2, AT3]:
 *     RT call(AT0, AT1, AT2, AT3) except +
 *
 *     void bind0                          (RT (*f)(AT0, AT1, AT2, AT3)                except * with gil               ) except +
 *     void bind1          [BT0]           (RT (*f)(BT0, AT0, AT1, AT2, AT3)           except * with gil, BT0          ) except +
 *     void bind2          [BT0, BT1]      (RT (*f)(BT0, BT1, AT0, AT1, AT2, AT3)      except * with gil, BT0, BT1     ) except +
 *     void bind3          [BT0, BT1, BT2] (RT (*f)(BT0, BT1, BT2, AT0, AT1, AT2, AT3) except * with gil, BT0, BT1, BT2) except +
 *
 *     void bind_noexcept0                 (RT (*f)(AT0, AT1, AT2, AT3)                         with gil               ) except +
 *     void bind_noexcept1 [BT0]           (RT (*f)(BT0, AT0, AT1, AT2, AT3)                    with gil, BT0          ) except +
 *     void bind_noexcept2 [BT0, BT1]      (RT (*f)(BT0, BT1, AT0, AT1, AT2, AT3)               with gil, BT0, BT1     ) except +
 *     void bind_noexcept3 [BT0, BT1, BT2] (RT (*f)(BT0, BT1, BT2, AT0, AT1, AT2, AT3)          with gil, BT0, BT1, BT2) except +
 */

template<typename RT, typename AT0, typename AT1, typename AT2, typename AT3>
using Func4 = Func<RT, AT0, AT1, AT2, AT3>;


/*
 * Five arguments.
 *
 * pxd:
 *
 * cppclass Func5[RT, AT0, AT1, AT2, AT3, AT4]:
 *     RT call(AT0, AT1, AT2, AT3, AT4) except +
 *
 *     void bind0                          (RT (*f)(AT0, AT1, AT2, AT3, AT4)                except * with gil               ) except +
 *     void bind1          [BT0]           (RT (*f)(BT0, AT0, AT1, AT2, AT3, AT4)           except * with gil, BT0          ) except +
 *     void bind2          [BT0, BT1]      (RT (*f)(BT0, BT1, AT0, AT1, AT2, AT3, AT4)      except * with gil, BT0, BT1     ) except +
 *     void bind3          [BT0, BT1, BT2] (RT (*f)(BT0, BT1, BT2, AT0, AT1, AT2, AT3, AT4) except * with gil, BT0, BT1, BT2) except +
 *
 *     void bind_noexcept0                 (RT (*f)(AT0, AT1, AT2, AT3, AT4)                         with gil               ) except +
 *     void bind_noexcept1 [BT0]           (RT (*f)(BT0, AT0, AT1, AT2, AT3, AT4)                    with gil, BT0          ) except +
 *     void bind_noexcept2 [BT0, BT1]      (RT (*f)(BT0, BT1, AT0, AT1, AT2, AT3, AT4)               with gil, BT0, BT1     ) except +
 *     void bind_noexcept3 [BT0, BT1, BT2] (RT (*f)(BT0, BT1, BT2, AT0, AT1, AT2, AT3, AT4)          with gil, BT0, BT1, BT2) except +
 */

template<typename RT, typename AT0, typename AT1, typename AT2, typename AT3, typename AT4>
using Func5 = Func<RT, AT0, AT1, AT2, AT3, AT4>;


/**
 * For usage by "Py Interface Functions", i.e. empty global function pointers
 * in libopenage that get filled by Cython at initialization time.
 *
 * Use only for global objects that have associated symbol names (this is enforced).
 *
 * The interface for PyIfFunc is identical to that of Func, so we'll simply typedef
 * them to avoid the redundant pxd lines from above.
 *
 * pxd:
 *
 * ctypedef Func0 PyIfFunc0
 * ctypedef Func1 PyIfFunc1
 * ctypedef Func2 PyIfFunc2
 * ctypedef Func3 PyIfFunc3
 * ctypedef Func4 PyIfFunc4
 * ctypedef Func5 PyIfFunc5
 */
template<typename ReturnType, typename ... ArgTypes>
class PyIfFunc : public Func<ReturnType, ArgTypes ...> {
public:
	PyIfFunc() {
		add_py_if_component(this, [=]() -> bool {
			try {
				this->check_fptr();
				return true;
			} catch (Error &) {
				return false;
			}
		});
	}

	~PyIfFunc() {
		destroy_py_if_component(this);
	}

	// no copy construction!
	PyIfFunc<ReturnType, ArgTypes ...>(const PyIfFunc<ReturnType, ArgTypes ...> &other) = delete;
	PyIfFunc<ReturnType, ArgTypes ...>(PyIfFunc<ReturnType, ArgTypes ...> &&other) = delete;
	PyIfFunc<ReturnType, ArgTypes ...> &operator =(const PyIfFunc<ReturnType, ArgTypes ...> &other) = delete;
	PyIfFunc<ReturnType, ArgTypes ...> &operator =(PyIfFunc<ReturnType, ArgTypes ...> &&other) = delete;

	// but you may convert this to a regular Func object.
	operator Func<ReturnType, ArgTypes ...> &() const {
		return static_cast<Func<ReturnType, ArgTypes ...>>(this->fptr);
	}
};


}} // openage::pyinterface

#endif
