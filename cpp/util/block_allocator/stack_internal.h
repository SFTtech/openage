// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_BLOCK_ALLOCATOR_STACK_INTERNAL_H_
#define OPENAGE_UTIL_BLOCK_ALLOCATOR_STACK_INTERNAL_H_

#include <cstdlib>
#include <memory>
#include <vector>

namespace openage {
namespace util {
namespace _alloc {


/**
 * Class for common stack allocator functionality.
 *
 * Base implementation must provide new_ptr() and template<bool> releaser(),
 * which releases and frees if the template parameter is true.
 * The size of each allcoation should be sizeof(T).
 *
 * Unique and shared manipulation are not provided
 * since memory cannot be deallocated in arbitrary order
 */
template<class T, template<class> class core_alloc>
class stack_allocator_common : public core_alloc<T> {

public:
	~stack_allocator_common() = default;
	/**
	 * forward the constructor to the stored.
	 */
	template<class... Args>
	stack_allocator_common(Args &&... vargs)
		:
		core_alloc<T>(std::forward<Args>(vargs)...) {
	}

	/**
	 * Returns a pointer to an object of type T,
	 * initialized with T(...vargs)
	 * @param vargs The arguments passed to the constructor
	 * @return A pointer to a newly constructed T(...vargs)
	 */
	template<class... Args>
	inline T *create(Args &&... vargs) {
		return new(this->new_ptr()) T(std::forward<Args>(vargs)...);
	}

	/**
	 * Releases the passed pointer, but does not call the destructor
	 * @param to_release The pointer whose memory is being released
	 */
	inline void release() {
		this->template releaser<false>();
	}

	/**
	 * Releases the passed pointer, and calls the destructor
	 * @param to_release The pointer whose memory is being released
	 */
	inline void free() {
		this->template releaser<true>();
	}

	/**
	 * Deallocates the extra memory and destroys allocated objects
	 */
	inline void clear() {
		this->template doclear<true>();
	}

	/**
	 * Deallocates the extra memory, but does not call any destructors
	 */
	inline void deallocate() {
		this->template doclear<false>();
	}

	/**
	 * Returns the size of each data block
	 */
	inline size_t alloc_size() const {
		return sizeof(T);
	}
};


} // namespace _alloc
} // namespace util
} // namespace openage


#endif
