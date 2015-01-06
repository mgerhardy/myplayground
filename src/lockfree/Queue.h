#pragma once

#include <atomic>
#include <algorithm>

/**
 * @brief Lockless queue that allocated an internal @c Node structure which each item that is inserted.
 *
 * This queue orders elements FIFO (first-in-first-out).
 *
 * This queue is best used with
 *
 * http://web.cecs.pdx.edu/~walpole/class/cs510/papers/08.pdf
 *
 * @note Every added @c T is copied into the list.
 */
template<class T>
class Queue {
private:
	Queue& operator=(const Queue&);
	Queue(const Queue&);

	struct Node {
		Node* _next;
		T _value;
		Node(const T& value) :
				_next(nullptr), _value(value) {
		}
		Node(T&& value) :
				_next(nullptr), _value(std::move(value)) {
		}
	};
	std::atomic<Node*> _head;
	std::atomic<Node*> _tail;
public:
	Queue();
	~Queue();

	/**
	 * @brief adds a copy of the item to the end of the queue
	 */
	void add(const T& item);
	void add(T&& item);

	/**
	 * @return @c true if the queue does not contain any element
	 */
	bool empty() const;

	/**
	 * @brief removes object at the beginning with copying the removed item to the reference
	 *
	 * @return If @c true is returned here, the @c item is filled with the removed item. @c false is returned if the queue was empty.
	 */
	bool pop(T& item);
	/**
	 * @brief removes object at the beginning without being interested in the object itself
	 */
	bool pop();

	/**
	 * @brief removes all objects
	 */
	void clear();

	/**
	 * @return the number of elements in this queue.
	 *
	 * @note Beware that this is no constant operation but an O(n) traversal (due to the asynchronous nature of this queue implementation).
	 */
	int size() const;
};

template<class T>
Queue<T>::Queue() :
		_tail(nullptr), _head(nullptr) {
}

template<class T>
Queue<T>::~Queue() {
	clear();
}

template<class T>
inline void Queue<T>::add(const T& item) {
	Node* node = new Node(item);
	Node* old = _tail.exchange(node);
	if (_head.compare_exchange_strong(node, nullptr))
		old->_next = node;
}

template<class T>
inline void Queue<T>::add(T&& item) {
	Node* node = new Node(item);
	Node* old = _tail.exchange(node);
	if (_head.compare_exchange_strong(node, nullptr))
		old->_next = node;
}

template<class T>
inline bool Queue<T>::pop(T& item) {
	Node* node = _tail.exchange(nullptr);
	if (node == nullptr)
		return false;
	_tail.compare_exchange_strong(nullptr, node);
	_head = node->_next;
	item = node->_value;
	delete node;
	return true;
}

template<class T>
inline bool Queue<T>::pop() {
	Node* node = _head.exchange(nullptr);
	if (node == nullptr)
		return false;
	_tail.compare_exchange_strong(nullptr, node);
	_head = node->_next;
	delete node;
	return true;
}

template<class T>
inline void Queue<T>::clear() {
	while (pop()) {
	}
}

template<class T>
inline bool Queue<T>::empty() const {
	return _head == nullptr;
}

template<class T>
inline int Queue<T>::size() const {
	int count = 0;
	for (Node* node = _head; node; node = node->_next, ++count);
	return count;
}
