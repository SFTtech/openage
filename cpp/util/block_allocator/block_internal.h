// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_BLOCK_ALLOCATOR_BLOCK_INTERNAL_H_
#define OPENAGE_UTIL_BLOCK_ALLOCATOR_BLOCK_INTERNAL_H_

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <vector>

#include "block_internal.h"
#include "../compiler.h"
#include "../unique.h"

namespace openage {
namespace util {
namespace _alloc {

/**
 * Class for common block allocator functionality.
 *
 * Base implementation must provide new_ptr() and template<bool>
 * releaser(T*), which releases and frees if the template parameter is true.
 *
 * Also must make a type val_ptr available, which is the type that
 * each allocation block refers to.
 */
template<class T, template<class> class core_alloc>
class block_allocator_common : public core_alloc<T> {
	using this_type = block_allocator_common<T, core_alloc>;

	/** value pointer type fetched from allocator implementation */
	using _val_ptr = typename core_alloc<T>::_val_ptr;


public:
	~block_allocator_common() = default;

	template<class... Args>
	block_allocator_common(Args &&... vargs)
		:
		core_alloc<T>(std::forward<Args>(vargs)...) {
	}

	/** deletion struct for unique_ptr/shared_ptr */
	struct deleter {
		this_type *alloc;
		inline void operator ()(T *to_del) {
			alloc->free(to_del);
		}
		deleter(this_type *a)
			:
			alloc{a} {
		}
	};

	/**
	 * Returns a pointer to an object of type T,
	 * initialized with T(...vargs)
	 * @param vargs The arguments passed to the constructor
	 * @return A pointer to a newly constructed T(...vargs)
	 */
	template<class... Args>
	inline T *create(Args &&...vargs) {
		return new(this->new_ptr()) T(std::forward<Args>(vargs)...);
	}

	/**
	 * Releases the passed pointer, but does not call the destructor
	 * of the object. Will invalidate the memory region, however
	 * @param to_release The pointer whose memory is being released
	 */
	inline void release(T *to_release) {
		this->template releaser<false>(to_release);
	}

	/**
	 * Releases the passed pointer, and calls the destructor
	 * @param to_release The pointer whose memory is being released
	 */
	inline void free(T *to_release) {
		this->template releaser<true>(to_release);
	}

	/** same as create, except returns a unique_pointer */
	template<class... Args>
	inline std::unique_ptr<T, deleter> make_uniqueptr(Args &&...vargs) {
		return std::unique_ptr<T, deleter>(
			create(std::forward<Args>(vargs)...),
			get_deleter()
		);
	}

	/** same as create, except returns a shared pointer */
	template<class... Args>
	inline std::shared_ptr<T> make_sharedptr(Args &&...vargs) {
		return std::shared_ptr<T>(
			create(std::forward<Args>(vargs)...),
			get_deleter()
		);
	}

	inline size_t alloc_size() const {
		return sizeof(_val_ptr);
	}

	inline deleter get_deleter() {
		return deleter{this};
	}
};


} // namespace _alloc
} // namespace util
} // namespace openage

#endif
