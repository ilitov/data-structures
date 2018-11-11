#pragma once
#ifndef STATIC_QUEUE_HEADER
#define STATIC_QUEUE_HEADER

#include <stdexcept>

template <typename T, size_t N>
class Queue {
public:
	Queue();

public:
	bool empty() const;
	size_t size() const;

	T& front();
	const T& front() const;

	T& back();
	const T& back() const;

	void push(const T &value);
	void pop();

private:
	T data[N];
	size_t m_front;
	size_t m_back;
	size_t m_size;
};

template <typename T, size_t N>
inline Queue<T, N>::Queue()
	: m_front(0)
	, m_back(0)
	, m_size(0) {

}

template <typename T, size_t N>
inline bool Queue<T, N>::empty() const {
	return !size();
}

template <typename T, size_t N>
inline size_t Queue<T, N>::size() const {
	return m_size;
}

template <typename T, size_t N>
inline T& Queue<T, N>::front() {
	return const_cast<T&>(static_cast<const Queue<T, N>&>(*this).front());
}

template <typename T, size_t N>
inline const T& Queue<T, N>::front() const {
	if (empty()) {
		throw std::logic_error("Empty queue!");
	}

	return data[m_front];
}

template <typename T, size_t N>
inline T& Queue<T, N>::back() {
	return const_cast<T&>(static_cast<const Queue<T, N>&>(*this).back());
}

template <typename T, size_t N>
inline const T& Queue<T, N>::back() const {
	if (empty()) {
		throw std::logic_error("Empty queue!");
	}

	return data[m_back ? m_back - 1 : N - 1];
}

template <typename T, size_t N>
inline void Queue<T, N>::push(const T &value) {
	if (m_size >= N) {
		throw std::logic_error("Full queue!");
	}

	data[m_back] = value;
	m_back = (m_back + 1) % N;
	++m_size;
}

template <typename T, size_t N>
inline void Queue<T, N>::pop() {
	if (empty()) {
		throw std::logic_error("Empty queue!");
	}

	m_front = (m_front + 1) % N;
	--m_size;
}

#endif // !STATIC_QUEUE_HEADER
