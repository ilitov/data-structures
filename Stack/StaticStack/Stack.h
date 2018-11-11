#pragma once
#ifndef STATIC_STACK_CLASS_HEADER
#define STATIC_STACK_CLASS_HEADER

template <typename T, size_t MAX_SIZE>
class Stack {
public:
	Stack();

public:
	bool empty() const;
	size_t getSize() const;
	size_t getMaxSize() const;

	T& top();
	const T& top() const;

	void push(const T &value);
	void pop();

private:
	T data[MAX_SIZE];
	size_t size;
};

template <typename T, size_t MAX_SIZE>
inline Stack<T, MAX_SIZE>::Stack()
	: size(0) {

}

template <typename T, size_t MAX_SIZE>
inline bool Stack<T, MAX_SIZE>::empty() const {
	return size == 0;
}

template <typename T, size_t MAX_SIZE>
inline size_t Stack<T, MAX_SIZE>::getSize() const {
	return size;
}

template <typename T, size_t MAX_SIZE>
inline size_t Stack<T, MAX_SIZE>::getMaxSize() const {
	return MAX_SIZE;
}

template <typename T, size_t MAX_SIZE>
inline T& Stack<T, MAX_SIZE>::top() {
	return const_cast<T&>(static_cast<const Stack<T, MAX_SIZE>&>(*this).top());
}

template <typename T, size_t MAX_SIZE>
inline const T& Stack<T, MAX_SIZE>::top() const {
	if (empty()) {
		throw std::logic_error("Empty stack!");
	}

	return data[size - 1];
}

template <typename T, size_t MAX_SIZE>
inline void Stack<T, MAX_SIZE>::push(const T &value) {
	if (getSize() >= MAX_SIZE) {
		throw std::logic_error("Full stack!");
	}

	data[size] = value;
	++size;
}

template <typename T, size_t MAX_SIZE>
inline void Stack<T, MAX_SIZE>::pop() {
	if (!empty()) {
		--size;
	}
}

#endif // !STATIC_STACK_CLASS_HEADER
