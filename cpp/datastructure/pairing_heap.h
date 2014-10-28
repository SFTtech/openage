#ifndef OPENAGE_DATASTRUCTURE_PAIRINGHEAP_H_6A826E0ABA3142F7B9C6999C96FCE47B
#define OPENAGE_DATASTRUCTURE_PAIRINGHEAP_H_6A826E0ABA3142F7B9C6999C96FCE47B

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
#include <type_traits>
#include <unordered_map>

#include "../log.h"
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

	PairingHeapNode(const T &data)
		:
		first_child(nullptr),
		prev_sibling(nullptr),
		next_sibling(nullptr),
		parent(nullptr),
		data(data) {
	}

	~PairingHeapNode() {}

	/**
	 * Let this node become a child of the given one.
	 */
	void become_child_of(this_type *node) {
		node->add_child(this);
	}

	/**
	 * Add the given node as a child to this one.
	 */
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

	/**
	 * Links the given subtree root node with this one.
	 * Returns the resulting subtree root node after both
	 * nodes were compared and linked.
	 */
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

	/**
	 * Link all siblings backwards from right to left.
	 * Recursive call, one stage for each all childs of the root node.
	 * This results in the computation of the new subtree root.
	 */
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

	/**
	 * Cut this node from all parent and sibling connections.
	 * This effectively cuts out the subtree.
	 */
	void loosen() {
		if (this->parent != nullptr) {
			// release us from some other node
			if (this->parent->first_child == this) {
				// we are the first child
				// make the next sibling the first child
				this->parent->first_child = this->next_sibling;
			}
			if (this->prev_sibling != nullptr) {
				this->prev_sibling->next_sibling = this->next_sibling;
			}
			if (this->next_sibling != nullptr) {
				this->next_sibling->prev_sibling = this->prev_sibling;
			}
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

	~PairingHeap() {
		this->clear();
	}

	/**
	 * adds the given node to the heap.
	 * O(1)
	 */
	void push_node(node_t &node) {
		if (unlikely(this->root_node == nullptr)) {
			this->root_node = &node;
		} else {
			this->root_node = this->root_node->link_with(&node);
		}

		this->nodes.insert({node.data, &node});
		this->node_count += 1;
	}

	/**
	 * adds the given item to the heap.
	 * O(1)
	 */
	void push(const T &item) {
		node_t *new_node = new node_t{item};
		this->push_node(*new_node);
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
		node_t *current_sibling = this->root_node->first_child;
		this->delete_node(this->root_node);
		this->root_node = nullptr;

		// 1. link root children pairwise, last node may be alone
		node_t *first_pair = nullptr;
		node_t *previous_pair = nullptr;

		while (current_sibling != nullptr) {
			node_t *link0 = current_sibling;
			node_t *link1 = current_sibling->next_sibling;

			// pair link0 and link1
			if (link1 != nullptr) {
				// get the first sibling for next pair, just in advance.
				current_sibling = link1->next_sibling;

				// do the link: merges two nodes, smaller one = root.
				node_t *link_root = link0->link_with(link1);
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

		// and it's done!
		return ret;
	}

	void delete_node(node_t *node) {
		auto nkeys = this->nodes.equal_range(node->data);
		for (auto it = nkeys.first; it != nkeys.second; ++it) {
			if (it->second == node) {
				delete it->second;     // free the node
				this->nodes.erase(it); // remove entry n from hashmap
				this->node_count -= 1;
				return;
			}
		}
		throw util::Error{"specified node not found for deletion!"};
	}

	/**
	 * Returns the smallest item on the heap.
	 * O(1)
	 */
	T top() const {
		return this->root_node->data;
	}

	/**
	 * Get a reference to the container node of the given item.
	 * O(1)
	 */
	node_t &get_node(const T &item) const {
		auto it = this->nodes.find(item);
		if (it != std::end(this->nodes)) {
			return *it->second;
		} else {
			throw util::Error{"item not found on heap!"};
		}
	}

	/**
	 * Update a given item.
	 * This function only makes sense when the contained objects
	 * use an internal member for sorting.
	 *
	 * This cuts the subtree and links the subtree again.
	 * Also known as the decrease_key operation.
	 * O(1)
	 */
	void update(const T &item) const {
		node_t &node = this->get_node(item);

		if (not this->cmp(item, this->root_node->data)) {
			throw util::Error{"only decreases are allowed for an update!"};
		}

		if (&node != this->root_node) {
			node.loosen();
			node.link_with(this->root_node);
		}
	}

	/**
	 * delete a given node from the heap.
	 *
	 * if the node is the current root, just pop().
	 * else, cut the node from its parent, pop() that subtree
	 * and merge these trees.
	 * O(1)
	 */
	void erase(const T &item) {
		// depth-first deletion
		if (this->root_node == nullptr) {
			throw util::Error{"can't pop an empty heap!"};
		}
		else if (item == *this->root_node) {
			this->pop();
		} else {
			this->delete_node(this->get_node(item));
		}
	}

	/**
	 * erase all elements on the heap.
	 */
	void clear() {
		for (auto &it : this->nodes) {
			this->delete_node(it.second);
		}
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
		return (this->node_count == 0);
	}

protected:
	size_t node_count;
	compare cmp;
	node_t *root_node;

	std::unordered_multimap<T, node_t *> nodes;
};

} // namespace datastructure
} // namespace openage

#endif // _OPENAGE_DATASTRUCTURE_PAIRINGHEAP_H_
