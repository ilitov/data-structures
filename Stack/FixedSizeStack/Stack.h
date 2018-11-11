#pragma once
#ifndef FIXED_SIZE_STACK_CLASS_HEADER
#define FIXED_SIZE_STACK_CLASS_HEADER

template <typename T>
class Stack {
public:
	Stack(size_t capacity);
	Stack(const Stack &r);
	Stack& operator=(const Stack &r);
	~Stack();

public:
	bool empty() const;
	size_t getSize() const;
	size_t getMaxSize() const;

	T& top();
	const T& top() const;

	void push(const T &value);
	void pop();

	void swap(Stack &other);

private:
	void freeMemory();
	void copy(const Stack &other);

private:
	T *data;
	size_t size;
	size_t capacity;
};

template <typename T>
inline Stack<T>::Stack(size_t capacity)
	: data(nullptr)
	, size(0)
	, capacity(0) {
	if (capacity == 0) {
		throw std::logic_error("Invalid capacity!");
	}

	data = new T[capacity];
	this->capacity = capacity;
}

template <typename T>
inline Stack<T>::Stack(const Stack<T> &r)
	: Stack(r.capacity) {
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
	delete[] data;
	data = nullptr;

	size = 0;
	capacity = 0;
}

template <typename T>
inline void Stack<T>::copy(const Stack<T> &other) {
	for (size_t i = 0; i < other.size; ++i) {
		data[i] = other.data[i];
	}

	size = other.size;
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
inline size_t Stack<T>::getMaxSize() const {
	return capacity;
}

template <typename T>
inline void Stack<T>::swap(Stack<T> &other) {
	if (this != &other) {
		T *tmpData = data;
		data = other.data;
		other.data = tmpData;

		std::swap(size, other.size);
		std::swap(capacity, other.capacity);
	}
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

	return data[size - 1];
}

template <typename T>
inline void Stack<T>::push(const T &value) {
	if (size >= capacity) {
		throw std::logic_error("Full stack!");
	}

	data[size] = value;
	++size;
}

template <typename T>
inline void Stack<T>::pop() {
	if (!empty()) {
		--size;
	}
}

template <typename T>
inline void swap(Stack<T> &lhs, Stack<T> &rhs) {
	lhs.swap(rhs);
}

#endif // !FIXED_SIZE_STACK_CLASS_HEADER
