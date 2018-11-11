#pragma once
#ifndef DOUBLY_LINKED_LIST_CLASS_HEADER
#define DOUBLY_LINKED_LIST_CLASS_HEADER

#include <stdexcept>

template <typename T>
class DoublyLinkedList {
public:
	using value_type = T;
	using size_type = size_t;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;

private:
	struct Node;

public:
	class iterator;
	class const_iterator;

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;

	const_iterator cbegin() const;
	const_iterator cend() const;

public:
	DoublyLinkedList();
	DoublyLinkedList(const DoublyLinkedList &r);
	DoublyLinkedList& operator=(const DoublyLinkedList &rhs);
	~DoublyLinkedList();

public:
	reference front();
	const_reference front() const;

	reference back();
	const_reference back() const;

	void push_front(const_reference value);
	void push_back(const_reference value);

	void pop_front();
	void pop_back();

	iterator erase(iterator pos);
	iterator insert(iterator pos, const_reference value);

	void clear();
	void merge(DoublyLinkedList &other);

	bool empty() const;
	size_type size() const;

	void swap(DoublyLinkedList &other);

private:
	void copyFrom(const DoublyLinkedList &other);

private:
	Node *head;
	Node *tail;
	size_type m_size;
};

template <typename T>
struct DoublyLinkedList<T>::Node {
	value_type data;
	Node *prev;
	Node *next;

	Node(const_reference data = value_type(), Node *prev = nullptr, Node *next = nullptr);
};

template <typename T>
class DoublyLinkedList<T>::iterator {
	friend class DoublyLinkedList<T>;
	iterator(Node *node);

public:
	iterator& operator++();
	iterator operator++(int);

	iterator& operator--();
	iterator operator--(int);

	bool operator==(const iterator &r) const;
	bool operator!=(const iterator &r) const;

	reference operator*();
	pointer operator->();

private:
	Node *node;
};

template <typename T>
class DoublyLinkedList<T>::const_iterator {
	friend class DoublyLinkedList<T>;
	const_iterator(const Node *node);

public:
	const_iterator& operator++();
	const_iterator operator++(int);

	const_iterator& operator--();
	const_iterator operator--(int);

	bool operator==(const const_iterator &r) const;
	bool operator!=(const const_iterator &r) const;

	const_reference operator*() const;
	const_pointer operator->() const;

private:
	const Node *node;
};

/* --- DOUBLY LINKED LIST --- */
template <typename T>
inline DoublyLinkedList<T>::DoublyLinkedList()
	: head(nullptr)
	, tail(nullptr)
	, m_size(0) {

}

template <typename T>
inline DoublyLinkedList<T>::DoublyLinkedList(const DoublyLinkedList<T> &r)
	: DoublyLinkedList<T>() {
	try {
		copyFrom(r);
	}
	catch(...) {
		clear();
		throw;
	}
}

template <typename T>
inline DoublyLinkedList<T>& DoublyLinkedList<T>::operator=(const DoublyLinkedList<T> &rhs) {
	if (this != &rhs) {
		DoublyLinkedList<T> tmp(rhs);
		swap(tmp);
	}

	return *this;
}

template <typename T>
inline DoublyLinkedList<T>::~DoublyLinkedList() {
	clear();
}

template <typename T>
inline typename DoublyLinkedList<T>::reference DoublyLinkedList<T>::front() {
	return const_cast<reference>(static_cast<const DoublyLinkedList<T>&>(*this).front());
}

template <typename T>
inline typename DoublyLinkedList<T>::const_reference DoublyLinkedList<T>::front() const {
	if (empty()) {
		throw std::logic_error("Empty list!");
	}

	return head->data;
}

template <typename T>
inline typename DoublyLinkedList<T>::reference DoublyLinkedList<T>::back() {
	return const_cast<reference>(static_cast<const DoublyLinkedList<T>&>(*this).back());
}

template <typename T>
inline typename DoublyLinkedList<T>::const_reference DoublyLinkedList<T>::back() const {
	if (empty()) {
		throw std::logic_error("Empty list!");
	}

	return tail->data;
}

template <typename T>
inline void DoublyLinkedList<T>::push_front(const_reference value) {
	Node *newNode = new Node(value, nullptr, head);

	if (head) {
		head->prev = newNode;
		head = newNode;
	}
	else {
		head = tail = newNode;
	}

	++m_size;
}

template <typename T>
inline void DoublyLinkedList<T>::push_back(const_reference value) {
	Node *newNode = new Node(value, tail, nullptr);

	if (tail) {
		tail->next = newNode;
		tail = newNode;
	}
	else {
		head = tail = newNode;
	}

	++m_size;
}

template <typename T>
inline void DoublyLinkedList<T>::pop_front() {
	if (!empty()) {
		Node *toDelete = head;

		head = head->next;
		if (!head) {
			tail = nullptr;
		}
		else {
			head->prev = nullptr;
		}

		delete toDelete;
		--m_size;
	}
}

template <typename T>
inline void DoublyLinkedList<T>::pop_back() {
	if (!empty()) {
		Node *toDelete = tail;

		tail = tail->prev;
		if (!tail) {
			head = nullptr;
		}
		else {
			tail->next = nullptr;
		}

		delete toDelete;
		--m_size;
	}
}

template <typename T>
inline typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::erase(iterator pos) {
	iterator resultIterator = end();
	if(pos != end()){
		Node *toDelete = pos.node;
		resultIterator = iterator(toDelete->next);

		if (head == toDelete) {
			pop_front();
		}
		else if (tail == toDelete) {
			pop_back();
		}
		else {
			toDelete->prev->next = toDelete->next;
			toDelete->next->prev = toDelete->prev;
			
			delete toDelete;
			--m_size;
		}
	}

	return resultIterator;
}

template <typename T>
inline typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::insert(iterator pos, const_reference value) {
	iterator resultIterator = end();
	if (pos != end()) {
		if (head == pos.node) {
			push_front(value);
			resultIterator = iterator(head);
		}
		else {
			Node *newNode = new Node(value, pos.node->prev, pos.node);
			resultIterator = iterator(newNode);

			pos.node->prev->next = newNode;
			pos.node->prev = newNode;

			++m_size;
		}
	}
	else {
		push_back(value);
		resultIterator = iterator(tail);
	}

	return resultIterator;
}

template <typename T>
inline void DoublyLinkedList<T>::clear() {
	Node *tmp = head;

	while (tmp) {
		Node *toDelete = tmp;
		tmp = tmp->next;
		delete toDelete;
	}

	head = tail = nullptr;
	m_size = 0;
}

template <typename T>
inline void DoublyLinkedList<T>::merge(DoublyLinkedList<T> &other) {
	if (this != &other && !other.empty()) {
		tail->next = other.head;
		other.head->prev = tail;

		tail = other.tail;
		m_size += other.m_size;

		other.head = other.tail = nullptr;
		other.m_size = 0;
	}
}

template <typename T>
inline bool DoublyLinkedList<T>::empty() const {
	return !size();
}

template <typename T>
inline typename DoublyLinkedList<T>::size_type DoublyLinkedList<T>::size() const {
	return m_size;
}

template <typename T>
inline void DoublyLinkedList<T>::swap(DoublyLinkedList<T> &other) {
	std::swap(head, other.head);
	std::swap(tail, other.tail);
	std::swap(m_size, other.m_size);
}

template <typename T>
inline void DoublyLinkedList<T>::copyFrom(const DoublyLinkedList<T> &other) {
	for (const_iterator it = other.begin(); it != other.end(); ++it) {
		push_back(*it);
	}
}

template <typename T>
inline typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::begin() {
	return iterator(head);
}

template <typename T>
inline typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::end() {
	return iterator(nullptr);
}

template <typename T>
inline typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::begin() const {
	return const_iterator(head);
}

template <typename T>
inline typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::end() const {
	return const_iterator(nullptr);
}

template <typename T>
inline typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::cbegin() const {
	return begin();
}

template <typename T>
inline typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::cend() const {
	return end();
}

/* --- NODE CTOR --- */
template <typename T>
inline DoublyLinkedList<T>::Node::Node(const_reference data, Node *prev, Node *next)
	: data(data)
	, prev(prev)
	, next(next) {

}

/* --- ITERATOR --- */
template <typename T>
inline DoublyLinkedList<T>::iterator::iterator(Node *node)
	: node(node) {

}

template <typename T>
inline typename DoublyLinkedList<T>::iterator& DoublyLinkedList<T>::iterator::operator++() {
	node = node->next;
	return *this;
}

template <typename T>
inline typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::iterator::operator++(int) {
	iterator tmp(*this);
	++(*this);
	return tmp;
}

template <typename T>
inline typename DoublyLinkedList<T>::iterator& DoublyLinkedList<T>::iterator::operator--() {
	node = node->prev;
	return *this;
}

template <typename T>
inline typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::iterator::operator--(int) {
	iterator tmp(*this);
	--(*this);
	return tmp;
}

template <typename T>
inline bool DoublyLinkedList<T>::iterator::operator==(const iterator &r) const {
	return node == r.node;
}

template <typename T>
inline bool DoublyLinkedList<T>::iterator::operator!=(const iterator &r) const {
	return !(*this == r);
}

template <typename T>
inline typename DoublyLinkedList<T>::reference DoublyLinkedList<T>::iterator::operator*() {
	return node->data;
}

template <typename T>
inline typename DoublyLinkedList<T>::pointer DoublyLinkedList<T>::iterator::operator->() {
	return &(operator*());
}

/* --- CONST_ITERATOR --- */
template <typename T>
inline DoublyLinkedList<T>::const_iterator::const_iterator(const Node *node)
	: node(node) {

}

template <typename T>
inline typename DoublyLinkedList<T>::const_iterator& DoublyLinkedList<T>::const_iterator::operator++() {
	node = node->next;
	return *this;
}

template <typename T>
inline typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::const_iterator::operator++(int) {
	const_iterator tmp(*this);
	++(*this);
	return tmp;
}

template <typename T>
inline typename DoublyLinkedList<T>::const_iterator& DoublyLinkedList<T>::const_iterator::operator--() {
	node = node->prev;
	return *this;
}

template <typename T>
inline typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::const_iterator::operator--(int) {
	const_iterator tmp(*this);
	--(*this);
	return tmp;
}

template <typename T>
inline bool DoublyLinkedList<T>::const_iterator::operator==(const const_iterator &r) const {
	return node == r.node;
}

template <typename T>
inline bool DoublyLinkedList<T>::const_iterator::operator!=(const const_iterator &r) const {
	return !(*this == r);
}

template <typename T>
inline typename DoublyLinkedList<T>::const_reference DoublyLinkedList<T>::const_iterator::operator*() const {
	return node->data;
}

template <typename T>
inline typename DoublyLinkedList<T>::const_pointer DoublyLinkedList<T>::const_iterator::operator->() const {
	return &(operator*());
}

/* std::swap */
template <typename T>
inline void swap(DoublyLinkedList<T> &lhs, DoublyLinkedList<T> &rhs) {
	lhs.swap(rhs);
}

#endif // !DOUBLY_LINKED_LIST_CLASS_HEADER
