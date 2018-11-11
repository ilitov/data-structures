#pragma once
#ifndef STACK_CLASS_HEADER
#define STACK_CLASS_HEADER

#include <stdexcept>

template <typename T, typename Container>
class Stack {
public:
	Stack() = default;
	Stack(const Stack &r) = default;
	Stack& operator=(const Stack &rhs);
	~Stack() = default;

public:
	T& top();
	const T& top() const;

	void push(const T &value);
	void pop();

	bool empty() const;
	size_t size() const;
	size_t max_size() const;

	void swap(Stack &other);

private:
	Container data;
};

template <typename T, typename Container>
inline Stack<T, Container>& Stack<T, Container>::operator=(const Stack<T, Container> &rhs) {
	if (this != &rhs) {
		Stack<T, Container> tmp(rhs);
		swap(tmp);
	}

	return *this;
}

template <typename T, typename Container>
inline T& Stack<T, Container>::top() {
	return const_cast<T&>(static_cast<const Stack<T, Container>&>(*this).top());
}

template <typename T, typename Container>
inline const T& Stack<T, Container>::top() const {
	if (empty()) {
		throw std::logic_error("Empty stack!");
	}

	return data.back();
}

template <typename T, typename Container>
inline void Stack<T, Container>::push(const T &value) {
	data.push_back(value);
}

template <typename T, typename Container>
inline void Stack<T, Container>::pop() {
	if (!data.empty()) {
		data.pop_back();
	}
}

template <typename T, typename Container>
inline bool Stack<T, Container>::empty() const {
	return !size();
}

template <typename T, typename Container>
inline size_t Stack<T, Container>::size() const {
	return data.size();
}

template <typename T, typename Container>
inline size_t Stack<T, Container>::max_size() const {
	return data.max_size();
}

template <typename T, typename Container>
inline void Stack<T, Container>::swap(Stack<T, Container> &other) {
	std::swap(data, other.data);
}

template <typename T, typename Container>
inline void swap(Stack<T, Container> &lhs, Stack<T, Container> &rhs) {
	lhs.swap(rhs);
}

#endif // !STACK_CLASS_HEADER
