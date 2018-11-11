#pragma once
#ifndef LINKED_STACK_CLASS_HEADER
#define LINKED_STACK_CLASS_HEADER

template <typename T>
class Stack {
private:
	struct Node {
		T data;
		Node *next;

		Node(const T &data = T(), Node *next = nullptr);
	};

public:
	Stack();
	Stack(const Stack &r);
	Stack& operator=(const Stack &rhs);
	~Stack();

public:
	bool empty() const;
	size_t getSize() const;

	T& top();
	const T& top() const;

	void push(const T &value);
	void pop();

	void swap(Stack &other);

private:
	void freeMemory();
	void copy(const Stack<T> &other);

private:
	Node *pTop;
	size_t size;
};

template <typename T>
inline Stack<T>::Node::Node(const T &data, Node *next)
	: data(data)
	, next(next) {

}

template <typename T>
inline Stack<T>::Stack()
	: pTop(nullptr)
	, size(0) {

}

template <typename T>
inline Stack<T>::Stack(const Stack<T> &r)
	: Stack() {
	try {
		copy(r);
	}
	catch (...) {
		freeMemory();
		throw;
	}
}

template <typename T>
inline Stack<T>& Stack<T>::operator=(const Stack<T> &rhs) {
	if (this != &rhs) {
		Stack<T> tmp(rhs);
		swap(tmp);
	}

	return *this;
}

template <typename T>
inline Stack<T>::~Stack() {
	freeMemory();
}

template <typename T>
inline void Stack<T>::freeMemory() {
	Node *tmp = pTop;

	while (tmp) {
		Node *toDelete = tmp;
		tmp = tmp->next;
		delete toDelete;
	}

	pTop = nullptr;
	size = 0;
}

template <typename T>
inline void Stack<T>::copy(const Stack<T> &other) {
	if (other.empty()) {
		return;
	}

	try {
		pTop = new Node(other.top->data);

		Node *write = pTop;
		Node *read = other.pTop->next;
	
		while (read) {
			write = write->next = new Node(read->data);
			read = read->next;
		}

		size = other.size;
	}
	catch (...) {
		freeMemory();
		throw;
	}
}

template <typename T>
inline bool Stack<T>::empty() const {
	return size == 0;
}

template <typename T>
inline size_t Stack<T>::getSize() const {
	return size;
}

template <typename T>
inline void Stack<T>::swap(Stack<T> &other) {
	Node *tmpTop = pTop;
	pTop = other.pTop;
	other.pTop = tmpTop;

	size_t tmpSize = size;
	size = other.size;
	other.size = tmpSize;
}

template <typename T>
inline T& Stack<T>::top() {
	return const_cast<T&>(static_cast<const Stack<T>&>(*this).top());
}

template <typename T>
inline const T& Stack<T>::top() const {
	if (empty()) {
		throw std::logic_error("Empty stack!");
	}

	return pTop->data;
}

template <typename T>
inline void Stack<T>::push(const T &value) {
	pTop = new Node(value, pTop);
	++size;
}

template <typename T>
inline void Stack<T>::pop() {
	if (!empty()) {
		Node *tmp = pTop;
		pTop = pTop->next;
		delete tmp;

		--size;
	}
}

template <typename T>
inline void swap(Stack<T> &lhs, Stack<T> &rhs) {
	lhs.swap(rhs);
}

#endif // !LINKED_STACK_CLASS_HEADER
