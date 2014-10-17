#ifndef _OPENAGE_DATASTRUCTURE_PAIRINGHEAP_H_
#define _OPENAGE_DATASTRUCTURE_PAIRINGHEAP_H_

#include <functional>

#include "../util/compiler.h"
#include "../util/error.h"

namespace openage {
namespace datastructure {

template <class T, class compare=std::less<T>>
class PairingHeapNode {
public:
	using this_type = PairingHeapNode<T, compare>;

	PairingHeapNode *first_child;
	PairingHeapNode *prev_sibling;
	PairingHeapNode *next_sibling;
	PairingHeapNode *parent;       // for decrease-key and delete

	T data;
	compare cmp;

	PairingHeapNode(const T data)
		:
		first_child(nullptr),
		prev_sibling(nullptr),
		next_sibling(nullptr),
		parent(nullptr),
		data(data) {
	}

	~PairingHeapNode() {}

	void become_child_of(this_type *node) {
		node->add_child(this);
	}

	void add_child(this_type *node) {
		// first child is the most recently attached one
		// it must not have siblings as they will get lost.

		node->prev_sibling = nullptr;
		node->next_sibling = this->first_child;
		if (this->first_child != nullptr) {
			this->first_child->prev_sibling = node;
		}
		this->first_child = node;
		node->parent = this;
	}

	this_type *link_with(this_type *node) {
		this_type *linked_root, *linked_child;

		if (this->cmp(this->data, node->data)) {
			linked_root  = this;
			linked_child = node;
		}
		else {
			linked_root  = node;
			linked_child = this;
		}

		linked_root->add_child(linked_child);
		return linked_root;
	}

	this_type *link_backwards() {
		if (this->next_sibling != nullptr) {
			// recurse to last sibling,
			this_type *root = this->next_sibling->link_backwards();

			// then link ourself to the new root.
			this->next_sibling = nullptr;
			this->prev_sibling = nullptr;
			root->next_sibling = nullptr;
			root->prev_sibling = nullptr;
			return root->link_with(this);
		} else {
			// reached end, return this as current root,
			// the previous siblings will be linked to it.
			return this;
		}
	}

	void loosen() {
		if (this->parent != nullptr) {
			// release us from some other node
			this->prev_sibling->next_sibling = this->next_sibling;
			this->next_sibling->prev_sibling = this->prev_sibling;
			this->prev_sibling = nullptr;
			this->next_sibling = nullptr;
			this->parent = nullptr;
		}
	}
};

template <class T, class compare=std::less<T>, class node_t=PairingHeapNode<T, compare>>
class PairingHeap {
public:
	using this_type = PairingHeap<T, compare, node_t>;

	/**
	 * create a empty heap.
	 */
	PairingHeap()
		:
		node_count(0),
		root_node(nullptr) {
	}

	~PairingHeap() {}

	/**
	 * adds the given item to the heap.
	 * create a 1-node tree and link it with this one.
	 * O(1)
	 */
	void push(T item) {
		node_t *new_node = new node_t{item};

		if (unlikely(this->root_node == nullptr)) {
			this->root_node = new_node;
		} else {
			this->root_node = this->root_node->link_with(new_node);
		}
		this->node_count += 1;
	}

	/**
	 * returns the smallest item on the heap and deletes it.
	 * also known as delete_min.
	 *
	 * Ω(log log n), O(2^(2*sqrt(log log n)))
	 */
	T pop() {
		if (this->root_node == nullptr) {
			throw util::Error{"can't pop an empty heap!"};
		}

		// 0. remove tree root, it's the minimum.
		T ret = this->root_node->data;
		node_t *next_child = this->root_node->first_child;
		delete this->root_node;
		this->root_node = nullptr;

		// 1. link root children pairwise, last node may be alone
		node_t *first_pair = nullptr;
		node_t *previous_pair = nullptr;

		while (next_child != nullptr) {
			node_t *link0 = next_child;
			next_child = next_child->next_sibling;
			node_t *link1 = next_child;

			// pair link0 and link1
			if (link1 != nullptr) {
				// do the link: merges two nodes, smaller one = root.
				node_t *link_root = link0->link_with(link1);
				link_root->parent = nullptr;

				if (previous_pair == nullptr) {
					// this was the first pair
					first_pair = link_root;
					first_pair->prev_sibling = nullptr;
				}
				else {
					// set being the next sibling in the previous pair
					previous_pair->next_sibling = link_root;
					link_root->prev_sibling = previous_pair;
				}

				previous_pair = link_root;
				next_child = next_child->next_sibling;
				link_root->next_sibling = nullptr;
			}
			else {
				// link0 is the last and unpaired root child.
				if (previous_pair == nullptr) {
					// link0 was the only node
					first_pair = link0;
					link0->prev_sibling = nullptr;
				} else {
					previous_pair->next_sibling = link0;
					link0->prev_sibling = previous_pair;
				}
				link0->next_sibling = nullptr;
			}
		}

		// 2. then link remaining trees to the last one, from right to left
		if (first_pair != nullptr) {
			this->root_node = first_pair->link_backwards();
		}

		// and it's done!
		this->node_count -= 1;
		return ret;
	}

	/**
	 * returns the smallest item on the heap.
	 * O(1)
	 */
	T top() {
		return this->root_node->data;
	}

	/**
	 * delete a given node from the heap.
	 *
	 * if the node is the current root, just pop().
	 * else, cut the node from its parent, pop() that subtree
	 * and merge these trees.
	 */
	void erase(const T &item) {
		// depth-first deletion
		if (this->root_node == nullptr) {
			throw util::Error{"can't pop an empty heap!"};
		}
		else if (item == *this->root_node) {
			this->pop();
		} else {
			throw util::Error{"erasing not implemented yet"};
		}
	}

	/**
	 * erase all elements on the heap.
	 */
	void clear() {
		throw util::Error{"clearing not implemented yet"};
	}

	/**
	 * @returns the number of nodes stored on the heap.
	 */
	size_t size() {
		return this->node_count;
	}

	/**
	 * @returns whether there are no nodes stored on the heap.
	 */
	bool empty() {
		return (this->node_count == 0);
	}

protected:
	size_t node_count;
	compare cmp;

	node_t *root_node;
};

} // namespace datastructure
} // namespace openage

#endif // _OPENAGE_DATASTRUCTURE_PAIRINGHEAP_H_
