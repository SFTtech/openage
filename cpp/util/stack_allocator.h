// Copyright 2014-2014 the openage authors. See copying.md for legal info.
//
#ifndef OPENAGE_UTIL_STACK_ALLOCATOR_H_
#define OPENAGE_UTIL_STACK_ALLOCATOR_H_
#include <cstdlib>
#include <vector>
#include <new>

#include "compiler.h"
#include "unique.h"
namespace openage{
namespace util{

namespace _alloc{
/**
 * Class for common stack allocator functionality
 * Base implementation must provide get_ptr() ,
 * and template<bool> releaser(), which releases and frees
 * if the template parameter is true. The size of each allcoation
 * should be sizeof(T)
 *
 * Unique and shared manipulation are not provided
 * since memory cannot be deallocated in arbitrary order
 */
template<class T, template<class> class core_alloc>
class stack_allocator_common : public core_alloc<T>{

public:
	~stack_allocator_common() = default;
	//base classes have whatever damn constructor they want
	template<class... Args>
	stack_allocator_common(Args&&... vargs)
		:
		core_alloc<T>(std::forward<Args>(vargs)...){
	}

	/**
	 * Returns a pointer to an object of type T,
	 * initialized with T(...vargs)
	 * @param vargs The arguments passed to the constructor
	 * @return A pointer to a newly constructed T(...vargs)
	 */
	template<class... Args>
	inline T* create(Args&&... vargs){
		return new(this->get_ptr()) T(std::forward<Args>(vargs)...);
	}
		
	/**
	 * Releases the passed pointer, but does not call the destructor
	 * @param to_release The pointer whose memory is being released
	 */
	inline void release(){
		this->template releaser<false>();
	}

	/**
	 * Releases the passed pointer, and calls the destructor
	 * @param to_release The pointer whose memory is being released
	 */
	inline void free(){
		this->template releaser<true>();
	}

	/**
	 * Deallocates the extra memory and destroys allocated objects
	 */
	inline void clear(){
		this->template doclear<true>();
	}

	/**
	 * Deallocates the extra memory, but does not call any destructors
	 */
	inline void deallocate(){
		this->template doclear<false>();
	}

	//!Returns the size of each data block
	inline size_t alloc_size() const {return sizeof(T);}
};

//!implementation of stack allocator
template<class T>
class stack_allocator {
protected:
	struct deleter {
		void operator()(T* del) const {
			::free(del);
		}
	};

	template<bool do_free>
	void releaser();

	template<bool do_free>
	void fancy_release();

	using ptr_type = std::unique_ptr<T[], deleter>;
	std::vector<ptr_type> ptrs;
	size_t stack_size;
	T* cur_ptr;
	T* cur_stackend;

	typename std::vector<ptr_type>::iterator cur_substack;
	
	void add_substack();

	void update_cur_stack();

public:
	stack_allocator(const stack_allocator&) = delete;
	stack_allocator& operator=(const stack_allocator&) = delete;

	stack_allocator(stack_allocator&&) = default;
	stack_allocator& operator=(stack_allocator&&) = default;
	~stack_allocator() = default;

	/**
	 * Constructs a stack allocator.
	 * Each substack contains stack_size elements,
	 * and there can only be stack_limit elements
	 * or unlimited if stack_limit==0
	 * @param stack_size The size of each sub-stack used in allocating
	 * @param stack_limit The limit n the number of sub stacks
	 */
	stack_allocator(size_t stack_size);

	/**
	 * Retrieves a pointer to an location in memory that holds 1 T,
	 * except uninitialized. If allocations fails, return nullptr.
	 * @return A pointer pointing to an uninitialized T
	 */
	T* get_ptr();

	//!Deallocates empty substacks
	void shrink_to_fit();
	
	//!base implementation of the clearing function	
	template<bool destruct>
	void doclear();
};

template<class T>
stack_allocator<T>::stack_allocator(size_t _stack_size)
	:
	stack_size(_stack_size){
	this->add_substack();
}

template<class T>
void stack_allocator<T>::add_substack(){
	T* space = (T*)malloc(this->stack_size*sizeof(T));
	if(unlikely(!space)) {throw std::bad_alloc();}
	
	this->ptrs.emplace_back(space, deleter());
	this->cur_ptr = space;	
	this->cur_stackend = space + this->stack_size;
	this->cur_substack = this->ptrs.end()-1;
}

template<class T>
void stack_allocator<T>::update_cur_stack(){
	++(this->cur_substack);
	if(this->cur_substack == ptrs.end()){
		this->add_substack();
	}
	else{
		this->cur_ptr = this->cur_substack->get();
		this->cur_stackend = this->cur_ptr + this->stack_size;
	}
}

template<class T>
T* stack_allocator<T>::get_ptr(){
	if(unlikely(this->cur_ptr == this->cur_stackend)){
		this->update_cur_stack();
	}
	return this->cur_ptr++;
}

template<class T>
template<bool do_free>
void stack_allocator<T>::fancy_release(){
	if(unlikely(this->cur_substack == this->ptrs.begin())){
		return;
	}
	--(this->cur_substack);
	this->cur_stackend = this->cur_substack->get() + this->stack_size;
	this->cur_ptr = this->cur_stackend - 1;

	if(do_free){
		this->cur_ptr->~T();
	}
}
template<class T>
template<bool do_free>
void stack_allocator<T>::releaser(){
	if(likely(this->cur_ptr != this->cur_substack->get())){
		this->cur_ptr--;
	}
	else{
		this->fancy_release<do_free>();
	}
	if(do_free){
		this->cur_ptr->~T();
	}
}

template<class T>
template<bool destruct>
void stack_allocator<T>::doclear(){
	auto clearstack = [this]() {
		while(this->cur_ptr > this->cur_substack->get()) {
			this->cur_ptr--;
			this->cur_ptr->~T();
		}
	};
	if(destruct){
		clearstack();
		while(this->cur_substack != this->ptrs.begin()){
			--this->cur_substack;
			this->cur_ptr = *(this->cur_substack) + this->stack_size;
			clearstack();
		}
	}
	this->ptrs.resize(1);
	this->cur_ptr = this->ptrs[0].get();
	this->cur_substack = this->ptrs.begin();
}

template<class T>
void stack_allocator<T>::shrink_to_fit(){
	this->ptrs.erase(this->cur_substack + 1, this->ptrs.end());
	this->cur_substack = this->ptrs.end() - 1;
}

//!core of the fixed_stack_allocator
template<class T>
class fixed_stack_allocator{
protected:
	struct deleter{
		void operator()(T* del) const{
			::free(del);
		}
	};

	//does actual release, decision done at compile time
	template<bool do_free>
	void releaser();

	using ptr_type = std::unique_ptr<T[], deleter>;
	ptr_type data;
	T* cur_ptr;
	T* cur_stackend;

public:
	fixed_stack_allocator(const fixed_stack_allocator&) = delete;
	fixed_stack_allocator& operator=(const fixed_stack_allocator&) = delete;

	fixed_stack_allocator(fixed_stack_allocator&&) = default;
	fixed_stack_allocator& operator=(fixed_stack_allocator&&) = default;
	~fixed_stack_allocator() = default;

	/**
	 * Constructs a stack allocator.
	 * Each substack contains stack_size elements,
	 * and there can only be stack_limit elements
	 * or unlimited if stack_limit==0
	 * @param stack_size The size of each sub-stack used in allocating
	 */
	fixed_stack_allocator(size_t stack_size);

	/**
	 * Retrieves a pointer to an location in memory that holds 1 T,
	 * except uninitialized. If allocations fails, return nullptr.
	 */
	T* get_ptr();

	//provided for api symmetry with stack_allocator
	//!Removes empty blocks (Has no effect on this class)
	inline void shrink_to_fit(){}

	//!Resets the allocator to starting state, may destruct
	template<bool destruct>
	void doclear();
};

template<class T>
fixed_stack_allocator<T>::fixed_stack_allocator(size_t stack_size)
	:
	data((T*)malloc(stack_size*sizeof(T)), deleter()),
	cur_ptr(data.get()),
	cur_stackend(cur_ptr + stack_size) {
	if(!this->data) {throw std::bad_alloc();}
}

template<class T>
T* fixed_stack_allocator<T>::get_ptr() {
	if(likely(this->cur_ptr != this->cur_stackend)) {
		return this->cur_ptr++;
	}
	return nullptr;
}

template<class T>
template<bool do_release>
void fixed_stack_allocator<T>::releaser() {	
	if(likely(this->cur_ptr != this->data.get())) {
		this->cur_ptr--;
		if(do_release) {
			this->cur_ptr->~T();
		}
	}
}

template<class T>
template<bool destruct>
void fixed_stack_allocator<T>::doclear(){	
	while(this->cur_ptr > this->data.get()){
		--this->cur_ptr;
		if(destruct){this->cur_ptr->~T();}
	}
}

} //namespace _alloc
    
/**
 * This class emulates a stack, except in dynamic memory.
 * It will grow to accomodate the amount of memory needed,
 * and provides almost free allocations/deallocations.
 * However, memory can only be acquired/released from the top
 * of the stack
 */
template<class T>
using stack_allocator = _alloc::stack_allocator_common<T, _alloc::stack_allocator>;


/**
 * This class emulates a stack, except in dynamic memory.
 * It only holds a single block of memory, and will
 * fail to allocate more once the limit has been reached
 */
template<class T>
using fixed_stack_allocator =
	_alloc::stack_allocator_common<T, _alloc::fixed_stack_allocator>;

} //namespace util
} //namespace openage
#endif
