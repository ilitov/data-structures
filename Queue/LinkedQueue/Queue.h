#pragma once
#ifndef LINKED_QUEUE_CLASS_HEADER
#define LINKED_QUEUE_CLASS_HEADER

#include <stdexcept>

template <typename T>
class Queue {
	struct Node {
		T data;
		Node *next;

		Node(const T &data = T(), Node *next = nullptr);
	};

public:
	Queue();
	Queue(const Queue &r);
	Queue& operator=(const Queue &rhs);
	~Queue();

public:
	bool empty() const;
	size_t getSize() const;

	T& front();
	const T& front() const;

	T& back();
	const T& back() const;

	void push(const T &value);
	void pop();

	void swap(Queue &other);

private:
	void copy(const Queue &r);
	void freeMemory();

private:
	Node *head;
	Node *tail;
	size_t size;
};

template <typename T>
inline Queue<T>::Node::Node(const T &data, Node *next)
	: data(data)
	, next(next) {

}

template <typename T>
inline Queue<T>::Queue()
	: head(nullptr)
	, tail(nullptr)
	, size(0) {

}

template <typename T>
inline Queue<T>::Queue(const Queue<T> &r)
	: Queue() {
	try {
		copy(r);
	}
	catch (...) {
		freeMemory();
		throw;
	}
}

template <typename T>
inline Queue<T>& Queue<T>::operator=(const Queue<T> &rhs) {
	if (this != &rhs) {
		Queue<T> tmp(rhs);
		swap(rhs);
	}

	return *this;
}

template <typename T>
inline Queue<T>::~Queue() {
	freeMemory();
}

template <typename T>
inline bool Queue<T>::empty() const {
	return !getSize();
}

template <typename T>
inline size_t Queue<T>::getSize() const {
	return size;
}

template <typename T>
inline T& Queue<T>::front() {
	return const_cast<T&>(static_cast<const Queue<T>&>(*this).front());
}

template <typename T>
inline const T& Queue<T>::front() const {
	if (empty()) {
		throw std::logic_error("Empty queue!");
	}

	return head->data;
}

template <typename T>
inline T& Queue<T>::back() {
	return const_cast<T&>(static_cast<const Queue<T>&>(*this).back());
}

template <typename T>
inline const T& Queue<T>::back() const {
	if (empty()) {
		throw std::logic_error("Empty queue!");
	}

	return tail->data;
}

template <typename T>
inline void Queue<T>::push(const T &value) {
	Node *newNode = new Node(value);

	if (!head) {
		head = tail = newNode;
	}
	else {
		tail = tail->next = newNode;
	}

	++size;
}

template <typename T>
inline void Queue<T>::pop() {
	if (!empty()) {
		Node *toDelete = head;

		head = head->next;
		if (!head) {
			tail = nullptr;
		}

		delete toDelete;
		--size;
	}
}

template <typename T>
inline void Queue<T>::swap(Queue<T> &other) {
	std::swap(head, other.head);
	std::swap(tail, other.tail);
	std::swap(size, other.size);
}

template <typename T>
inline void Queue<T>::copy(const Queue<T> &r) {
	if (!r.empty()) {
		for (Node *read = r.head; read; read = read->next) {
			push(read->data);
		}
	}
}

template <typename T>
inline void Queue<T>::freeMemory() {
	Node *tmp = head;

	while (tmp) {
		Node *toDelete = tmp;
		tmp = tmp->next;
		delete toDelete;
	}

	head = tail = nullptr;
	size = 0;
}

/* std::swap */
template <typename T>
inline void swap(Queue<T> &lhs, Queue<T> &rhs) {
	lhs.swap(rhs);
}

#endif // !LINKED_QUEUE_CLASS_HEADER
