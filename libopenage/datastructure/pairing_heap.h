// Copyright 2014-2016 the openage authors. See copying.md for legal info.

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

#include <memory>
#include <functional>
#include <type_traits>
#include <unordered_set>

#include "../util/compiler.h"
#include "../error/error.h"

namespace openage::datastructure {


template<typename T, typename compare=std::less<T>>
class PairingHeapNode final : public std::enable_shared_from_this<PairingHeapNode<T, compare>> {
public:
	using this_type = PairingHeapNode<T, compare>;

	std::shared_ptr<this_type> first_child;
	std::shared_ptr<this_type> prev_sibling;
	std::shared_ptr<this_type> next_sibling;
	std::shared_ptr<this_type> parent;       // for decrease-key and delete

	T data;
	compare cmp;

	PairingHeapNode(const T &data)
		:
		data(data) {}

	PairingHeapNode(T &&data)
		:
		data(std::move(data)) {}

	~PairingHeapNode() = default;


	/**
	 * Let this node become a child of the given one.
	 */
	void become_child_of(const std::shared_ptr<this_type> &node) {
		node->add_child(this->shared_from_this());
	}

	/**
	 * Add the given node as a child to this one.
	 */
	void add_child(const std::shared_ptr<this_type> &node) {
		// first child is the most recently attached one
		// it must not have siblings as they will get lost.

		node->prev_sibling = nullptr;
		node->next_sibling = this->first_child;

		if (this->first_child != nullptr) {
			this->first_child->prev_sibling = node;
		}

		this->first_child = node;
		node->parent = this->shared_from_this();
	}

	/**
	 * Links the given subtree root node with this one.
	 * Returns the resulting subtree root node after both
	 * nodes were compared and linked.
	 */
	std::shared_ptr<this_type> link_with(const std::shared_ptr<this_type> &node) {
		std::shared_ptr<this_type> linked_root;
		std::shared_ptr<this_type> linked_child;

		if (this->cmp(this->data, node->data)) {
			linked_root  = this->shared_from_this();
			linked_child = node;
		}
		else {
			linked_root  = node;
			linked_child = this->shared_from_this();
		}

		linked_root->add_child(linked_child);
		return linked_root;
	}

	/**
	 * Link all siblings backwards from right to left.
	 * Recursive call, one stage for each all childs of the root node.
	 * This results in the computation of the new subtree root.
	 */
	std::shared_ptr<this_type> link_backwards() {
		if (this->next_sibling == nullptr) {
			// reached end, return this as current root,
			// the previous siblings will be linked to it.
			return this->shared_from_this();
		}

		// recurse to last sibling,
		std::shared_ptr<this_type> node = this->next_sibling->link_backwards();

		// then link ourself to the new root.
		this->next_sibling = nullptr;
		this->prev_sibling = nullptr;
		node->next_sibling = nullptr;
		node->prev_sibling = nullptr;
		return node->link_with(this->shared_from_this());
	}

	/**
	 * Cut this node from all parent and sibling connections.
	 * This effectively cuts out the subtree.
	 */
	void loosen() {
		if (this->parent != nullptr) {
			// release us from some other node
			if (this->parent->first_child.get() == this) {
				// we are the first child
				// make the next sibling the first child
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
	}
};


/**
 * (Quite) efficient heap implementation.
 */
template<typename T,
         typename compare=std::less<T>,
         typename heapnode_t=PairingHeapNode<T, compare>>
class PairingHeap final {
public:
	using node_t = heapnode_t;
	using element_t = std::shared_ptr<node_t>;
	using this_type = PairingHeap<T, compare, node_t>;

	/**
	 * create a empty heap.
	 */
	PairingHeap()
		:
		node_count(0),
		root_node(nullptr) {
	}

	~PairingHeap() = default;

	/**
	 * adds the given item to the heap.
	 * O(1)
	 */
	element_t push(const T &item) {
		element_t new_node = std::make_shared<node_t>(item);
		this->push_node(new_node);
		return new_node;
	}

	/**
	 * moves the given item to the heap.
	 * O(1)
	 */
	element_t push(T &&item) {
		element_t new_node = std::make_shared<node_t>(std::move(item));
		this->push_node(new_node);
		return new_node;
	}

	/**
	 * returns and removes the smallest item on the heap.
	 */
	T pop() {
		return std::move(this->pop_node()->data);
	}

	/**
	 * returns the smallest item on the heap and deletes it.
	 * also known as delete_min.
	 *                       _________
	 * Ω(log log n), O(2^(2*√log log n'))
	 */
	element_t pop_node() {
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

		while (unlikely(current_sibling != nullptr)) {
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
				} else {
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

		// and it's done!
		return ret;
	}

	/**
	 * Delete a node from the heap.
	 *
	 * If the item is the current root, just pop().
	 * else, cut the node from its parent, pop() that subtree
	 * and merge these trees.
	 * O(1)
	 */
	void erase_node(const element_t &node) {
		if (node == this->root_node) {
			this->pop();
		}
		else {
			node->loosen();
			element_t child = node->first_child;

			// merge all children of the node to pop
			while (child != nullptr) {
				element_t next = child->next_sibling;
				child->parent = nullptr;
				child->loosen();
				this->root_node = this->root_node->link_with(child);
				child = next;
			}

			this->node_count -= 1;
		}
	}

	/**
	 * Returns the smallest item on the heap.
	 * O(1)
	 */
	const T &top() const {
		return this->root_node->data;
	}

	/**
	 * Returns the smallest node on the heap.
	 * O(1)
	 */
	const element_t &top_node() const {
		return this->root_node;
	}

	/**
	 * Update a given node.
	 * This cuts the subtree and links the subtree again.
	 * Also known as the decrease_key operation.
	 * O(1)
	 */
	void update(const element_t &node) {
		// decreasing the root node won't change it.
		if (likely(node != this->root_node)) {
			node->loosen();
			this->root_node = node->link_with(this->root_node);
			// TODO: if increasing value (=contradicting 'decrease_key'),
			// relink node.childs with rootnode. Might be a nice feature
			// someday.
		}
		else {
			// it's the root node, so we just pop and push it.
			this->push_node(this->pop_node());
		}
	}

	/**
	 * erase all elements on the heap.
	 */
	void clear() {
		this->root_node = nullptr;
		this->node_count = 0;
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

protected:
	/**
	 * adds the given node to the heap.
	 * O(1)
	 */
	void push_node(const element_t &node) {
		if (unlikely(this->root_node == nullptr)) {
			this->root_node = node;
		} else {
			this->root_node = this->root_node->link_with(node);
		}

		this->node_count += 1;
	}

protected:
	compare cmp;
	size_t node_count;
	element_t root_node;
};

} // openage::datastructure
