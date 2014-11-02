#ifndef OPENAGE_DATASTRUCTURE_DOUBLY_LINKED_LIST_H_
#define OPENAGE_DATASTRUCTURE_DOUBLY_LINKED_LIST_H_

#include "../util/compiler.h"
#include "../util/error.h"

namespace openage {
namespace datastructure {

/**
 * Storage node for the doubly linked list.
 * Contains pointers to next and previous elements.
 */
template <class T>
class DoublyLinkedListNode {
public:
	DoublyLinkedListNode(const T &data)
		:
		data{data},
		previous{nullptr},
		next{nullptr} {
	}

	T data;
	DoublyLinkedListNode *previous;
	DoublyLinkedListNode *next;
};


/**
 * Doubly linked list implementation.
 *
 * It is a circular implementation:
 *    last->next      == first
 *    first->previous == last
 *
 * Stores the start and end node pointers, allows traversing over all present nodes and
 * can push/pop items to the front/back.
 */
template <class T, class node_t=DoublyLinkedListNode<T>>
class DoublyLinkedList {
public:
	DoublyLinkedList()
		:
		first{nullptr},
		last{nullptr},
		node_count{0} {
	}

	~DoublyLinkedList() {
		this->clear();
	}

	/**
	 * Insert the first item into the list.
	 * O(1)
	 */
	node_t *insert_first_item(const T &item) {
		if (this->size() != 0) {
			throw util::Error{
				"can't insert item as the first one "
				"if there's already something in the list."
			};
		} else {
			node_t *new_node   = new node_t{item};
			new_node->next     = new_node;
			new_node->previous = new_node;

			this->first = new_node;
			this->last  = new_node;
			this->node_count += 1;
			return new_node;
		}
	}

	/**
	 * Insert a data item after a given node.
	 * O(1)
	 */
	node_t *insert_after(node_t *node, const T &item) {
		node_t *new_node   = new node_t{item};
		new_node->next     = node->next;
		new_node->previous = node;

		node->next->previous = new_node;
		node->next           = new_node;
		this->node_count += 1;
		return new_node;
	}

	/**
	 * Insert a data item before a given node.
	 * O(1)
	 */
	node_t *insert_before(node_t *node, const T &item) {
		node_t *new_node   = new node_t{item};
		new_node->next     = node;
		new_node->previous = node->previous;

		node->previous->next = new_node;
		node->previous       = new_node;
		this->node_count += 1;
		return new_node;
	}

	/**
	 * Fully delete a node and fix the neighbor pointers.
	 * O(1)
	 */
	void erase(node_t *node) {
		node->previous->next = node->next;
		node->next->previous = node->previous;
		delete node;
		this->node_count -= 1;
	}

	/**
	 * Search for an item, then delete it.
	 * O(n)
	 */
	void erase(const T &item) {
		node_t *current = this->first;
		if (this->size > 0) {
			do {
				if (current->data == item) {
					this->erase_node(current);
					return;
				}
				current = current->next;
			} while (current != this->end);
		}
		throw util::Error("element not found!");
	}

	/**
	 * Seek to the given item position, then delete it.
	 * O(n)
	 */
	void erase(size_t pos) {
		if (pos < this->size()) {
			node_t *current = this->first;
			for (size_t i = 0; i < pos; i++) {
				current = current->next;
			}
			this->erase_node(current);
			return;
		}
		throw util::Error{"element %zu not in linked list!", pos};
	}

	/**
	 * Append a data item to the end of the list.
	 * O(1);
	 */
	node_t *push_back(const T &item) {
		if (unlikely(this->size() == 0)) {
			return this->insert_first_item(item);
		} else {
			node_t *new_last_node = this->insert_after(this->last, item);
			this->last = new_last_node;
			return new_last_node;
		}
	}

	/**
	 * Fetch and delete the last data item of the list.
	 * O(1)
	 */
	T pop_back() {
		if (this->node_count == 0) {
			throw util::Error{"can't pop from an empty list!"};
		}
		T data = this->last->data;
		node_t *new_last_node = this->last->previous;

		this->erase(this->last);
		this->last = new_last_node;
		return data;
	}

	/**
	 * Append a data item to the beginning of the list.
	 * O(1)
	 */
	node_t *push_front(T item) {
		if (unlikely(this->size() == 0)) {
			return this->insert_first_item(item);
		} else {
			node_t *new_first_node = this->insert_before(this->first, item);
			this->first = new_first_node;
			return new_first_node;
		}
	}

	/**
	 * Fetch and delete the first data item of the list.
	 * O(1)
	 */
	T pop_front() {
		if (this->node_count == 0) {
			throw util::Error{"can't pop from an empty list!"};
		}
		T data = this->first->data;
		node_t *new_first_node = this->first->next;

		this->erase(this->first);
		this->first = new_first_node;
		return data;
	}

	/**
	 * Return the first node in the list.
	 */
	node_t *get_first() {
		return this->first;
	}

	/**
	 * Return the last node in the list.
	 */
	node_t *get_last() {
		return this->last;
	}

	/**
	 * Return the current list element count.
	 */
	size_t size() {
		return this->node_count;
	}

	/**
	 * Check whether no elements are present in the list.
	 */
	bool empty() {
		return (this->node_count == 0);
	}

	/**
	 * Delete all list nodes.
	 * O(n)
	 */
	void clear() {
		node_t *delete_now = this->first;
		while (this->node_count > 0) {
			node_t *deleted_next = this->first->next;
			this->erase(delete_now);
			delete_now = deleted_next;
		}
	}

protected:
	node_t *first;     //!< The first node in the list
	node_t *last;      //!< The last node in the list

	size_t node_count; //!< The number of nodes currently stored in the list.
};

} // namespace datastructure
} // namespace openage

#endif
