// Copyright 2014-2025 the openage authors. See copying.md for legal info.

#pragma once

/** @file
 * This file contains the implementation of a pairing heap.
 * It is a priority queue with near-optimal runtime.
 *
 * The main advantage over the STL heap is the presence
 * of the decrease_key operation.
 *
 * Literature:
 *
 * Fredman, Michael L., Robert Sedgewick, Daniel D. Sleator, and Robert
 * E. Tarjan. "The pairing heap: A new form of self-adjusting heap."
 * Algorithmica 1, no. 1-4 (1986): 111-129.
 */

#include <functional>
#include <iterator>
#include <memory>
#include <type_traits>
#include <unordered_set>

#include "../error/error.h"
#include "../util/compiler.h"


#define OPENAGE_PAIRINGHEAP_DEBUG false


namespace openage::datastructure {


template <typename T,
          typename compare,
          typename heapnode_t>
class PairingHeap;

template <typename T, typename compare>
class PairingHeapIterator;


template <typename T, typename compare = std::less<T>>
class PairingHeapNode {
public:
	using this_type = PairingHeapNode<T, compare>;

	friend PairingHeap<T, compare, this_type>;
	friend PairingHeapIterator<T, compare>;

	T data;
	compare cmp;

	PairingHeapNode(const T &data) :
		data{data} {}

	PairingHeapNode(T &&data) :
		data{std::move(data)} {}

	~PairingHeapNode() = default;

	PairingHeapNode(const this_type &other) = delete;

	this_type &operator=(const this_type &other) = delete;

	/**
	 * Get contained node data.
	 */
	const T &get_data() const {
		return this->data;
	}

	/**
	 * Let this node become a child of the given one.
	 */
	void become_child_of(this_type *const node) {
		node->add_child(this);
	}

	/**
	 * Add the given node as a child to this one.
	 */
	void add_child(this_type *const new_child) {
		// first child is the most recently attached one
		// it must not have siblings as they will get lost.

		new_child->prev_sibling = nullptr;
		new_child->next_sibling = this->first_child;

		if (this->first_child != nullptr) {
			this->first_child->prev_sibling = new_child;
		}

		this->first_child = new_child;
		new_child->parent = this;
	}

	/**
	 * This method decides which node becomes the new root node
	 * by comparing `this` with `node`.
	 * The new root is returned, it has the other node as child.
	 */
	this_type *link_with(this_type *const node) {
		this_type *new_root;
		this_type *new_child;

		if (this->cmp(this->data, node->data)) {
			new_root = this;
			new_child = node;
		}
		else {
			new_root = node;
			new_child = this;
		}

		// children of new root become siblings of new new_child
		// -> parent of new child = new root

		// this will be set by the add_child method
		new_child->prev_sibling = nullptr;
		new_child->next_sibling = nullptr;

		// this is then set to the previous pair root:
		new_root->prev_sibling = nullptr;
		new_root->next_sibling = nullptr;

		// set up the child
		new_root->add_child(new_child);

		return new_root;
	}

	/**
	 * Link all siblings backwards from right to left.
	 * Recursive call, one stage for each all childs of the root node.
	 * This results in the computation of the new subtree root.
	 */
	this_type *link_backwards() {
		if (this->next_sibling == nullptr) {
			// reached end, return this as current root,
			// the previous siblings will be linked to it.
			return this;
		}

		// recurse to last sibling,
		this_type *node = this->next_sibling->link_backwards();

		// then link ourself to the new root.
		this->next_sibling = nullptr;
		this->prev_sibling = nullptr;
		node->next_sibling = nullptr;
		node->prev_sibling = nullptr;
		return this->link_with(node);
	}

	/**
	 * Cut this node from all parent and sibling connections,
	 * but keeps the child pointer.
	 * This effectively cuts out the subtree.
	 */
	void loosen() {
		// release us from some other node
		if (this->parent and this->parent->first_child == this) {
			// we are child
			// make the next sibling child
			this->parent->first_child = this->next_sibling;
		}
		// if we have a previous sibling
		if (this->prev_sibling != nullptr) {
			// set its next sibling to skip us.
			this->prev_sibling->next_sibling = this->next_sibling;
		}
		// if we have a next sibling
		if (this->next_sibling != nullptr) {
			// tell its previous sibling to skip us.
			this->next_sibling->prev_sibling = this->prev_sibling;
		}

		// reset sibling and parent ptrs.
		this->prev_sibling = nullptr;
		this->next_sibling = nullptr;
		this->parent = nullptr;
	}

private:
	this_type *first_child = nullptr;
	this_type *prev_sibling = nullptr;
	this_type *next_sibling = nullptr;
	this_type *parent = nullptr; // for decrease-key and delete
};


/**
 * @brief Iterator class for PairingHeap.
 *
 * This class provides a bidirectional iterator for the PairingHeap data structure.
 * It allows traversal of the heap in both forward and backward directions.
 * It is depth-first traversal.
 *
 * @tparam T The type of elements stored in the heap.
 * @tparam compare The comparison functor used to order the elements.
 */
template <typename T, typename compare = std::less<T>>
class PairingHeapIterator {
public:
	using iterator_category = std::bidirectional_iterator_tag;
	using value_type = T;
	using difference_type = std::ptrdiff_t;
	using pointer = T *;
	using reference = T &;

	/**
	 * @brief Constructs an iterator starting at the given node.
	 *
	 * @param node The starting node for the iterator.
	 */
	PairingHeapIterator(PairingHeapNode<T, compare> *node) :
		current(node) {}

	/**
	 * @brief Dereference operator.
	 *
	 * @return A reference to the data stored in the current node.
	 */
	reference operator*() const {
		return current->data;
	}

	/**
	 * @brief Member access operator.
	 *
	 * @return A pointer to the data stored in the current node.
	 */
	pointer operator->() const {
		return &(current->data);
	}


	/**
	 * @brief Get current node.
	 *
	 * @return The current node.
	 */
	PairingHeapNode<T, compare> *node() {
		return current;
	}


	/**
	 * @brief Pre-increment operator.
	 *
	 * Moves the iterator to the next node in the heap.
	 *
	 * @return A reference to the incremented iterator.
	 */
	PairingHeapIterator &operator++() {
		if (current->first_child) {
			current = current->first_child;
		}
		else if (current->next_sibling) {
			current = current->next_sibling;
		}
		else {
			while (current->parent && !current->parent->next_sibling) {
				current = current->parent;
			}
			if (current->parent) {
				current = current->parent->next_sibling;
			}
			else {
				current = nullptr;
			}
		}
		return *this;
	}

	/**
	 * @brief Post-increment operator.
	 *
	 * Moves the iterator to the next node in the heap.
	 *
	 * @return A copy of the iterator before incrementing.
	 */
	PairingHeapIterator operator++(int) {
		PairingHeapIterator tmp = *this;
		++(*this);
		return tmp;
	}

	/**
	 * @brief Pre-decrement operator.
	 *
	 * Moves the iterator to the previous node in the heap.
	 *
	 * @return A reference to the decremented iterator.
	 */
	PairingHeapIterator &operator--() {
		if (current->prev_sibling) {
			current = current->prev_sibling;
			while (current->first_child) {
				current = current->first_child;
				while (current->next_sibling) {
					current = current->next_sibling;
				}
			}
		}
		else if (current->parent) {
			current = current->parent;
		}
		return *this;
	}

	/**
	 * @brief Post-decrement operator.
	 *
	 * Moves the iterator to the previous node in the heap.
	 *
	 * @return A copy of the iterator before decrementing.
	 */
	PairingHeapIterator operator--(int) {
		PairingHeapIterator tmp = *this;
		--(*this);
		return tmp;
	}

	/**
	 * @brief Equality comparison operator.
	 *
	 * @param a The first iterator to compare.
	 * @param b The second iterator to compare.
	 * @return True if both iterators point to the same node, false otherwise.
	 */
	friend bool operator==(const PairingHeapIterator &a, const PairingHeapIterator &b) {
		return a.current == b.current;
	}

	/**
	 * @brief Inequality comparison operator.
	 *
	 * @param a The first iterator to compare.
	 * @param b The second iterator to compare.
	 * @return True if the iterators point to different nodes, false otherwise.
	 */
	friend bool operator!=(const PairingHeapIterator &a, const PairingHeapIterator &b) {
		return a.current != b.current;
	}

private:
	PairingHeapNode<T, compare> *current; ///< Pointer to the current node in the heap.
};


/**
 * (Quite) efficient heap implementation.
 */
template <typename T,
          typename compare = std::less<T>,
          typename heapnode_t = PairingHeapNode<T, compare>>
class PairingHeap final {
public:
	using element_t = heapnode_t *;
	using this_type = PairingHeap<T, compare, heapnode_t>;
	using iterator = PairingHeapIterator<T, compare>;

	/**
	 * create a empty heap.
	 */
	PairingHeap() :
		node_count(0),
		root_node(nullptr) {
	}

	~PairingHeap() {
		this->clear();
	};

	/**
	 * adds the given item to the heap.
	 * O(1)
	 */
	element_t push(const T &item) {
		element_t new_node = new heapnode_t(item);
		this->push_node(new_node);
		return new_node;
	}

	/**
	 * moves the given item to the heap.
	 * O(1)
	 */
	element_t push(T &&item) {
		element_t new_node = new heapnode_t(std::move(item));
		this->push_node(new_node);
		return new_node;
	}

	/**
	 * returns the smallest item on the heap and deletes it.
	 * also known as delete_min.
	 *                       _________
	 * Ω(log log n), O(2^(2*√log log n'))
	 */
	T pop() {
		if (this->root_node == nullptr) {
			throw Error{MSG(err) << "Can't pop an empty heap!"};
		}

		// 0. remove tree root, it's the minimum.
		element_t ret = this->root_node;
		element_t current_sibling = this->root_node->first_child;
		this->root_node = nullptr;

		// 1. link root children pairwise, last node may be alone
		element_t first_pair = nullptr;
		element_t previous_pair = nullptr;

		while (current_sibling != nullptr) [[unlikely]] {
			element_t link0 = current_sibling;
			element_t link1 = current_sibling->next_sibling;

			// pair link0 and link1
			if (link1 != nullptr) {
				// get the first sibling for next pair, just in advance.
				current_sibling = link1->next_sibling;

				// do the link: merges two nodes, smaller one = root.
				element_t link_root = link0->link_with(link1);
				link_root->parent = nullptr;

				if (previous_pair == nullptr) {
					// this was the first pair
					first_pair = link_root;
					first_pair->prev_sibling = nullptr;
				}
				else {
					// store node as next sibling in previous pair
					previous_pair->next_sibling = link_root;
					link_root->prev_sibling = previous_pair;
				}

				previous_pair = link_root;
				link_root->next_sibling = nullptr;
			}
			else {
				// link0 is the last and unpaired root child.
				link0->parent = nullptr;
				if (previous_pair == nullptr) {
					// link0 was the only node
					first_pair = link0;
					link0->prev_sibling = nullptr;
				}
				else {
					previous_pair->next_sibling = link0;
					link0->prev_sibling = previous_pair;
				}
				link0->next_sibling = nullptr;
				current_sibling = nullptr;
			}
		}


		// 2. then link remaining trees to the last one, from right to left
		if (first_pair != nullptr) {
			this->root_node = first_pair->link_backwards();
		}

		this->node_count -= 1;

#if OPENAGE_PAIRINGHEAP_DEBUG
		if (1 != this->nodes.erase(ret)) {
			throw Error{ERR << "didn't remove node"};
		}
#endif

		// (to find those two lines, 14h of debugging passed)
		ret->loosen();
		ret->first_child = nullptr;

		// and it's done!
		T data = std::move(ret->data);
		delete ret;
		return data;
	}

	/**
	 * Returns the smallest item on the heap.
	 * O(1)
	 */
	const T &top() const {
		return this->root_node->get_data();
	}

	/**
	 * Returns the smallest node on the heap.
	 *
	 * O(1)
	 */
	const element_t &top_node() const {
		return this->root_node;
	}

	/**
	 * You must call this after the node data decreased.
	 * This cuts the subtree and links the subtree again.
	 * If the node value _increased_ and you call this,
	 * the heap is corrupted.
	 * Also known as the decrease_key operation.
	 *
	 * O(1)
	 */
	void decrease(const element_t &node) {
		if (node != this->root_node) [[likely]] {
			// cut out the node and its subtree
			node->loosen();
			this->root_node = node->link_with(this->root_node);
		}
		// decreasing the root node won't change it, so we do nothing.
	}

	/**
	 * After a change, call this to reorganize the given node.
	 * Support increase and decrease of values.
	 *
	 * Use `decrease` instead when you know the value decreased.
	 *
	 * O(1) (but slower than decrease), and O(pop) when node is the root.
	 */
	void update(element_t &node) {
		if (node != this->root_node) [[likely]] {
			node = this->push(this->remove_node(node));
		}
		else {
			// it's the root node, so we just pop and push it.
			node = this->push(this->pop());
		}
	}

	/**
	 * remove a node from the heap. Return its data.
	 *
	 * If the item is the current root, just pop().
	 * else, cut the node from its parent, pop() that subtree
	 * and merge these trees.
	 *
	 * O(pop_node)
	 */
	T remove_node(const element_t &node) {
		if (node == this->root_node) {
			return this->pop();
		}
		else {
			node->loosen();

			element_t real_root = this->root_node;
			this->root_node = node;
			T data = this->pop();

			element_t new_root = this->root_node;
			this->root_node = real_root;

			if (new_root != nullptr) {
				this->root_insert(new_root);
			}
			return data;
		}
	}

	/**
	 * erase all elements on the heap.
	 */
	void clear() {
		std::vector<element_t> to_delete;
		to_delete.reserve(this->size());

		// collect all node pointers to delete
		for (iterator it = this->begin(); it != this->end(); it++) {
			to_delete.push_back(it.node());
		}

		// delete all nodes
		for (element_t node : to_delete) {
			delete node;
		}

		// reset heap state to empty
		this->root_node = nullptr;
		this->node_count = 0;
#if OPENAGE_PAIRINGHEAP_DEBUG
		// clear the node set for debugging
		this->nodes.clear();
#endif
	}

	/**
	 * @returns the number of nodes stored on the heap.
	 */
	size_t size() const {
		return this->node_count;
	}

	/**
	 * @returns whether there are no nodes stored on the heap.
	 */
	bool empty() const {
		return this->node_count == 0;
	}

#if OPENAGE_PAIRINGHEAP_DEBUG
	/**
	 * verify the consistency of the heap.
	 * - check if each node is only present once.
	 * - check if the nodes are referenced at their correct place only
	 */
	std::unordered_set<element_t> check_consistency() const {
		std::unordered_set<element_t> found_nodes;

		auto func = [&](const element_t &root) {
			if (not root) {
				throw Error{ERR << "test function called with nullptr node"};
			}

			if (found_nodes.find(root) == std::end(found_nodes)) {
				found_nodes.insert(root);
			}
			else {
				throw Error{ERR << "encountered node twice"};
			}

			if (this->node_count != this->nodes.size()) {
				throw Error{ERR << "node count fail"};
			}

			if (root->next_sibling) {
				if (not root->next_sibling->prev_sibling) {
					throw Error{ERR << "node has next sibling, which has no prev sibling"};
				}
				if (root->next_sibling->prev_sibling != root) {
					throw Error{ERR << "node not referenced by next.prev"};
				}
			}

			if (root->prev_sibling) {
				if (not root->prev_sibling->next_sibling) {
					throw Error{ERR << "node has prev sibling, which has no next sibling"};
				}
				if (root->prev_sibling->next_sibling != root) {
					throw Error{ERR << "node not referenced by prev.next"};
				}
			}

			if (root->first_child) {
				if (root->first_child == root->next_sibling) {
					throw Error{ERR << "first_child is next_sibling"};
				}
				if (root->first_child == root->prev_sibling) {
					throw Error{ERR << "first_child is prev_sibling"};
				}
				if (root->first_child == root->parent) {
					throw Error{ERR << "first_child is parent"};
				}
			}

			if (root->parent) {
				if (found_nodes.find(root->parent) == std::end(found_nodes)) {
					throw Error{ERR << "parent node is not known"};
				}
				element_t child = root->parent->first_child;
				element_t lastchild;

				bool foundvianext = false, foundviaprev = false;
				std::unordered_set<element_t> loopprotect;
				while (true) {
					if (not child) {
						break;
					}

					if (loopprotect.find(child) == std::end(loopprotect)) {
						loopprotect.insert(child);
					}
					else {
						throw Error{ERR << "child reencountered when walking forward"};
					}

					if (child == root) {
						foundvianext = true;
					}

					// if both are equal, cmp will still be false.
					if (this->cmp(child->data, root->parent->data)) {
						throw Error{ERR << "tree invariant violated"};
					}

					lastchild = child;
					child = child->next_sibling;
				}

				loopprotect.clear();
				while (true) {
					if (not lastchild) {
						break;
					}

					if (loopprotect.find(lastchild) == std::end(loopprotect)) {
						loopprotect.insert(lastchild);
					}
					else {
						throw Error{ERR << "child reencountered when walking back"};
					}

					if (lastchild == root) {
						foundviaprev = true;
					}

					lastchild = lastchild->prev_sibling;
				}

				if (not foundvianext and not foundviaprev) {
					throw Error{ERR << "node not found as parent's child at all"};
				}
				else if (not foundvianext) {
					throw Error{ERR << "node not found via parent's next childs"};
				}
				else if (not foundviaprev) {
					throw Error{ERR << "node not found via parent's prev childs"};
				}
			}
		};

		if (this->root_node) {
			this->walk_tree(this->root_node, func);

			if (found_nodes.size() != this->size()) {
				for (auto &it : found_nodes) {
					const element_t &elem = it;
					if (this->nodes.find(elem) == std::end(this->nodes)) {
						throw Error{ERR << "node not recorded but found"};
					}
				}

				for (auto &it : this->nodes) {
					const element_t &elem = it;
					if (found_nodes.find(elem) == std::end(found_nodes)) {
						throw Error{ERR << "node recorded but not found"};
					}
				}

				throw Error{ERR << "node count inconsistent"};
			}
		}
		else {
			if (not this->empty()) {
				throw Error{ERR << "root missing but heap not empty"};
			}
		}

		return found_nodes;
	}
#endif

	/**
	 * Apply the given function to all nodes in the tree.
	 *
	 * @tparam reverse If true, the function is applied to the nodes in reverse order.
	 * @param func Function to apply to each node.
	 */
	template <bool reverse = false>
	void iter_all(const std::function<void(const element_t &)> &func) const {
		this->walk_tree<reverse>(this->root_node, func);
	}

	iterator begin() const {
		return iterator(this->root_node);
	}

	iterator end() const {
		return iterator(nullptr);
	}

private:
	/**
	 * Apply the given function to all nodes in the tree.
	 *
	 * @tparam reverse If true, the function is applied to the nodes in reverse order.
	 * @param start Starting node.
	 * @param func Function to apply to each node.
	 */
	template <bool reverse = false>
	void walk_tree(const element_t &start,
	               const std::function<void(const element_t &)> &func) const {
		if constexpr (not reverse) {
			func(start);
		}

		if (start) {
			auto node = start->first_child;
			while (true) {
				if (not node) {
					break;
				}

				this->walk_tree<reverse>(node, func);
				node = node->next_sibling;
			}
			if constexpr (reverse) {
				func(start);
			}
		}
	}


	/**
	 * adds the given node to the heap.
	 * use this if the node was not in the heap before.
	 * O(1)
	 */
	void push_node(const element_t &node) {
		this->root_insert(node);
#if OPENAGE_PAIRINGHEAP_DEBUG
		auto [iter, result] = this->nodes.insert(node);
		if (not result) {
			throw Error{ERR << "node already known"};
		}
#endif

		this->node_count += 1;
	}

	/**
	 * insert a node into the heap.
	 */
	void root_insert(const element_t &node) {
		if (this->root_node == nullptr) [[unlikely]] {
			this->root_node = node;
		}
		else {
			this->root_node = this->root_node->link_with(node);
		}
	}

	compare cmp;
	size_t node_count;
	element_t root_node;

#if OPENAGE_PAIRINGHEAP_DEBUG
	std::unordered_set<element_t> nodes;
#endif
};

} // namespace openage::datastructure
