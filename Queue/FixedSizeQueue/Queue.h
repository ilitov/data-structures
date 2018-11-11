#pragma once
#ifndef FIXEDm_size_QUEUE_HEADER
#define FIXEDm_size_QUEUE_HEADER

#include <stdexcept>

template <typename T>
class Queue {
public:
	Queue(size_t capacity);
	Queue(const Queue &r);
	Queue& operator=(const Queue &rhs);
	~Queue();

public:
	bool empty() const;
	size_t size() const;
	size_t capacity() const;

	T& front();
	const T& front() const;

	T& back();
	const T& back() const;

	void push(const T &value);
	void pop();

	void swap(Queue<T> &other);

private:
	void copy(const Queue<T> &other);
	void freeMemory();

private:
	T *data;
	size_t m_front;
	size_t m_back;
	size_t m_size;
	size_t m_capacity;
};

template <typename T>
inline Queue<T>::Queue(size_t capacity)
	: m_front(0)
	, m_back(0)
	, m_size(0)
	, m_capacity(0) {
	if (capacity == 0) {
		throw std::logic_error("Invalid capacity!");
	}

	data = new T[capacity];
	m_capacity = capacity;
}

template <typename T>
inline Queue<T>::Queue(const Queue<T> &r)
	: Queue<T>(r.m_capacity) {
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
		swap(tmp);
	}

	return *this;
}

template <typename T>
inline Queue<T>::~Queue() {
	freeMemory();
}

template <typename T>
inline void Queue<T>::swap(Queue<T> &other) {
	std::swap(data, other.data);
	std::swap(m_front, other.m_front);
	std::swap(m_back, other.m_back);
	std::swap(m_size, other.m_size);
	std::swap(m_capacity, other.m_capacity);
}

template <typename T>
inline void Queue<T>::copy(const Queue<T> &other) {
	for (size_t i = 0; i < other.m_capacity) {
		data[i] = other.data[i];
	}

	m_front = other.m_front;
	m_back = other.m_back;
	m_size = other.m_size;
}

template <typename T>
inline void Queue<T>::freeMemory() {
	delete[] data;
	data = nullptr;

	m_front = 0;
	m_back = 0;
	m_size = 0;
	m_capacity = 0;
}

template <typename T>
inline bool Queue<T>::empty() const {
	return !size();
}

template <typename T>
inline size_t Queue<T>::size() const {
	return m_size;
}

template <typename T>
inline size_t Queue<T>::capacity() const {
	return m_capacity;
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

	return data[m_front];
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

	return data[m_back ? m_back - 1 : m_capacity - 1];
}

template <typename T>
inline void Queue<T>::push(const T &value) {
	if (m_size >= m_capacity) {
		throw std::logic_error("Full queue!");
	}

	data[m_back] = value;
	m_back = (m_back + 1) % m_capacity;
	++m_size;
}

template <typename T>
inline void Queue<T>::pop() {
	if (empty()) {
		throw std::logic_error("Empty queue!");
	}

	m_front = (m_front + 1) % m_capacity;
	--m_size;
}

template <typename T>
inline void swap(Queue<T> &lhs, Queue<T> &rhs) {
	lhs.swap(rhs);
}

#endif // !FIXEDm_size_QUEUE_HEADER
