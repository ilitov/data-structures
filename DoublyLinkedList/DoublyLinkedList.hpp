#pragma once
#ifndef DOUBLY_LINKED_LIST_TEMPLATE_CLASS_HEADER
#define DOUBLY_LINKED_LIST_TEMPLATE_CLASS_HEADER

template <typename T>
class DoublyLinkedList {
public:
	using value_type = T;
	using size_type = std::size_t;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;

private:
	struct Node {
		value_type data;
		Node *prev;
		Node *next;

		Node(const_reference data = value_type(), Node *prev = nullptr, Node *next = nullptr);
	};

public:
	template <typename IT>
	class iterator_impl {
		friend class DoublyLinkedList<T>;
		iterator_impl(Node *node);

	public:
		using value_type = IT;
		using reference = IT&;
		using pointer = IT*;

		iterator_impl& operator++();
		iterator_impl operator++(int);

		iterator_impl& operator--();
		iterator_impl operator--(int);

		bool operator==(const iterator_impl &r) const;
		bool operator!=(const iterator_impl &r) const;

		reference operator*() const;
		pointer operator->() const;

	private:
		Node *node;
	};

	using iterator = iterator_impl<T>;
	using const_iterator = iterator_impl<const T>;

	iterator begin();
	iterator end();

	const_iterator cbegin() const;
	const_iterator cend() const;

	const_iterator begin() const;
	const_iterator end() const;

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

	void clear();
	void insert(iterator pos, const_reference value);
	iterator erase(iterator pos);

	bool empty() const;
	size_type size() const;

	void merge(DoublyLinkedList<T> &other);
	void swap(DoublyLinkedList<T> &other);

private:
	void copy(const DoublyLinkedList<T> &r);
	void freeMemory();

private:
	Node *head;
	Node *tail;
	size_type Size;
};

/* --- DOUBLY LINKED LIST --- */
template <typename T>
inline DoublyLinkedList<T>::DoublyLinkedList()
	: head(nullptr)
	, tail(nullptr)
	, Size(0) {
	try {
		head = new Node;
		tail = new Node;

		head->next = tail;
		tail->prev = head;
	}
	catch (...) {
		delete head;
		delete tail;
		throw;
	}
}

template <typename T>
inline DoublyLinkedList<T>::DoublyLinkedList(const DoublyLinkedList<T> &r)
	: DoublyLinkedList() {
	try {
		copy(r);
		Size = r.Size;
	}
	catch (...) {
		freeMemory();
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
	freeMemory();
}

template <typename T>
inline typename DoublyLinkedList<T>::reference DoublyLinkedList<T>::front() {
	return const_cast<DoublyLinkedList<T>::reference>(static_cast<const DoublyLinkedList<T>&>(*this).front());
}

template <typename T>
inline typename DoublyLinkedList<T>::const_reference DoublyLinkedList<T>::front() const {
	if (!empty()) {
		return head->next->data;
	}

	throw std::logic_error("Empty list!");
}

template <typename T>
inline typename DoublyLinkedList<T>::reference DoublyLinkedList<T>::back() {
	return const_cast<DoublyLinkedList<T>::reference>(static_cast<const DoublyLinkedList<T>&>(*this).back());
}

template <typename T>
inline typename DoublyLinkedList<T>::const_reference DoublyLinkedList<T>::back() const {
	if (!empty()) {
		return tail->prev->data;
	}

	throw std::logic_error("Empty list!");
}

template <typename T>
inline void DoublyLinkedList<T>::push_front(const_reference value) {
	insert(begin(), value);
}

template <typename T>
inline void DoublyLinkedList<T>::push_back(const_reference value) {
	insert(end(), value);
}

template <typename T>
inline void DoublyLinkedList<T>::pop_front() {
	erase(begin());
}

template <typename T>
inline void DoublyLinkedList<T>::pop_back() {
	erase(empty() ? end() : tail->prev);
}

template <typename T>
inline void DoublyLinkedList<T>::clear() {
	Node *tmp = head->next;

	while (tmp && tmp != tail) {
		Node *toDelete = tmp;
		tmp = tmp->next;
		delete toDelete;
	}

	head->next = tail;
	tail->prev = head;
}

template <typename T>
inline void DoublyLinkedList<T>::insert(iterator pos, const_reference value) {
	Node *newNode = new Node(value, pos.node->prev, pos.node);

	pos.node->prev->next = newNode;
	pos.node->prev = newNode;

	++Size;
}

template <typename T>
inline typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::erase(iterator pos) {
	iterator resultIterator = pos;
	if (pos != end()) {
		resultIterator = iterator(pos.node->next);
		Node *toDelete = pos.node;

		toDelete->prev->next = toDelete->next;
		toDelete->next->prev = toDelete->prev;

		delete toDelete;
		--Size;
	}

	return resultIterator;
}

template <typename T>
inline bool DoublyLinkedList<T>::empty() const {
	return !size();
}

template <typename T>
inline typename DoublyLinkedList<T>::size_type DoublyLinkedList<T>::size() const {
	return Size;
}

template <typename T>
inline void DoublyLinkedList<T>::merge(DoublyLinkedList<T> &other) {
	if (this != &other) {
		tail->prev->next = other.head->next;
		other.head->next->prev = tail->prev;

		tail->prev = other.tail->prev;
		other.tail->prev->next = tail;

		other.head->next = other.tail;
		other.tail->prev = other.head;

		Size += other.Size;
		other.Size = 0;
	}
}

template <typename T>
inline void DoublyLinkedList<T>::swap(DoublyLinkedList<T> &other) {
	if (this != &other) {
		Node *tmpHead = head;
		Node *tmpTail = tail;

		head = other.head;
		tail = other.tail;

		other.head = tmpHead;
		other.tail = tmpTail;

		size_type tmpCap = Size;
		Size = other.Size;
		other.Size = tmpCap;
	}
}

template <typename T>
inline void DoublyLinkedList<T>::copy(const DoublyLinkedList<T> &r) {
	for (const_iterator it = r.begin(); it != r.end(); ++it) {
		push_back(*it);
	}
}

template <typename T>
inline void DoublyLinkedList<T>::freeMemory() {
	clear();

	delete head;
	delete tail;

	head = tail = nullptr;
	Size = 0;
}

template <typename T>
inline typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::begin() {
	return iterator(head->next);
}

template <typename T>
inline typename DoublyLinkedList<T>::iterator DoublyLinkedList<T>::end() {
	return iterator(tail);
}

template <typename T>
inline typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::cbegin() const {
	return const_iterator(head->next);
}

template <typename T>
inline typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::cend() const {
	return const_iterator(tail);
}

template <typename T>
inline typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::begin() const {
	return const_iterator(head->next);
}

template <typename T>
inline typename DoublyLinkedList<T>::const_iterator DoublyLinkedList<T>::end() const {
	return const_iterator(tail);
}

/* --- ITERATOR --- */
template <typename T>
template <typename IT>
inline DoublyLinkedList<T>::iterator_impl<IT>::iterator_impl(Node *node)
	:node(node) {

}

template <typename T>
template <typename IT>
inline DoublyLinkedList<T>::iterator_impl<IT>& DoublyLinkedList<T>::iterator_impl<IT>::operator++() {
	node = node->next;
	return *this;
}

template <typename T>
template <typename IT>
inline DoublyLinkedList<T>::iterator_impl<IT> DoublyLinkedList<T>::iterator_impl<IT>::operator++(int) {
	iterator_impl tmp(*this);
	++(*this);
	return tmp;
}

template <typename T>
template <typename IT>
inline DoublyLinkedList<T>::iterator_impl<IT>& DoublyLinkedList<T>::iterator_impl<IT>::operator--() {
	node = node->prev;
	return *this;
}

template <typename T>
template <typename IT>
inline DoublyLinkedList<T>::iterator_impl<IT> DoublyLinkedList<T>::iterator_impl<IT>::operator--(int) {
	iterator_impl tmp(*this);
	--(*this);
	return tmp;
}

template <typename T>
template <typename IT>
inline bool DoublyLinkedList<T>::iterator_impl<IT>::operator==(const iterator_impl<IT> &r) const {
	return node == r.node;
}

template <typename T>
template <typename IT>
inline bool DoublyLinkedList<T>::iterator_impl<IT>::operator!=(const iterator_impl<IT> &r) const {
	return !(*this == r);
}

template <typename T>
template <typename IT>
inline typename DoublyLinkedList<T>::iterator_impl<IT>::reference 
DoublyLinkedList<T>::iterator_impl<IT>::operator*() const {
	return node->data;
}

template <typename T>
template <typename IT>
inline typename DoublyLinkedList<T>::iterator_impl<IT>::pointer
DoublyLinkedList<T>::iterator_impl<IT>::operator->() const {
	return &(operator*());
}

/* --- NODE CONSTRUCTOR --- */
template <typename T>
inline DoublyLinkedList<T>::Node::Node(const_reference data, Node *prev, Node *next)
	: data(data)
	, prev(prev)
	, next(next) {

}

template <typename T>
inline void swap(DoublyLinkedList<T> &lhs, DoublyLinkedList<T> &rhs) {
	lhs.swap(rhs);
}

#endif // !DOUBLY_LINKED_LIST_TEMPLATE_CLASS_HEADER
