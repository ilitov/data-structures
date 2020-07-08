#pragma once
#ifndef _THREAD_SAFE_QUEUE_HEADER_
#define _THREAD_SAFE_QUEUE_HEADER_

#include <mutex>				// Mutex locks for the head and the tail.
#include <memory>				// Smart pointers.
#include <limits>				// The maximum value of the capacity.
#include <atomic>				// Number of elements in the queue.
#include <stdexcept>			// Exceptions.
#include <condition_variable>	// Wait for pop() and push().

template <typename T>
class ThreadSafeQueue {
	struct Node {
		std::unique_ptr<T> m_data;
		Node *m_next;

		Node(std::unique_ptr<T> &&data = std::unique_ptr<T>(), Node *next = nullptr);
	};

public:
	ThreadSafeQueue(size_t maxQueueCapacity = std::numeric_limits<size_t>::max());
	ThreadSafeQueue(const ThreadSafeQueue &r) = delete;
	ThreadSafeQueue& operator=(const ThreadSafeQueue &rhs) = delete;
	ThreadSafeQueue(ThreadSafeQueue &&r) = delete;
	ThreadSafeQueue& operator=(ThreadSafeQueue &&rhs) = delete;
	~ThreadSafeQueue();

public:
	void push(const T &value);
	std::unique_ptr<T> try_pop();
	std::unique_ptr<T> wait_pop();

	size_t size() const;
	bool empty() const;

private:
	Node* popHead();
	Node* tryPopHead();
	Node *waitPopHead();

	std::unique_lock<std::mutex> waitData();
	std::unique_ptr<T> popResult(Node *old_head);
	
	const Node* getTail() const;
	static void freeMemory(Node *node);

private:
	Node *m_head;
	Node *m_tail;
	std::atomic<size_t> m_size;

	const size_t m_maxCapacity;

	mutable std::mutex m_headMtx;
	mutable std::mutex m_tailMtx;

	std::condition_variable m_wait_pop_cv;
	std::condition_variable m_wait_push_cv;
};

template <typename T>
inline ThreadSafeQueue<T>::Node::Node(std::unique_ptr<T> &&data, Node *next)
	: m_data(std::move(data))
	, m_next(next) {

}

template <typename T>
inline ThreadSafeQueue<T>::ThreadSafeQueue(size_t maxQueueCapacity)
	: m_head(new Node)
	, m_tail(m_head)
	, m_size(0)
	, m_maxCapacity(maxQueueCapacity) {
	// We use a dummy node in order to access only m_head(in pop())
	// or m_tail(in push()) and never both of them. Without the dummy node
	// there would be a case in which m_head == m_tail.

	if (maxQueueCapacity == 0) {
		throw std::logic_error("Invalid maxCapacity");
	}
}

template <typename T>
inline ThreadSafeQueue<T>::~ThreadSafeQueue() {
	// Delete the list of nodes.
	freeMemory(m_head);

	// Clear everything.
	m_head = nullptr;
	m_tail = nullptr;
	m_size.store(0);
}

template <typename T>
inline void ThreadSafeQueue<T>::push(const T &value) {
	// Create a new dummy node.
	std::unique_ptr<Node> new_node(new Node);

	// Prepare the data for the new node(the current dummy node).
	std::unique_ptr<T> new_data(new T(value));

	{
		// Lock the mutex for the tail and update the tail.
		std::unique_lock<std::mutex> tailLock(m_tailMtx);

		// Wait until there is enough space in the queue.
		m_wait_push_cv.wait(tailLock, [this]() { return m_size < m_maxCapacity; });

		// Update the data of the current dummy node.
		m_tail->m_data = std::move(new_data);

		// Set the next pointer to the new dummy node.
		m_tail->m_next = new_node.release();

		// Update the tail.
		m_tail = m_tail->m_next;

		// Update the number of elements in the queue.
		m_size += 1;
	}

	// Notify a waiting pop() thread that there is a new item in the queue.
	m_wait_pop_cv.notify_one();
}

template <typename T>
inline std::unique_ptr<T> ThreadSafeQueue<T>::try_pop() {
	Node *old_head = tryPopHead();
	
	// If old_head == nullptr, then the queue is empty.
	if (!old_head) {
		return std::unique_ptr<T>();
	}
	
	return popResult(old_head);
}

template <typename T>
inline std::unique_ptr<T> ThreadSafeQueue<T>::wait_pop() {
	// This node cannot be nullptr, because the thread waits until there is a pushed node.
	Node *old_head = waitPopHead();
	return popResult(old_head);
}

template <typename T>
inline size_t ThreadSafeQueue<T>::size() const {
	return m_size.load();
}

template <typename T>
inline bool ThreadSafeQueue<T>::empty() const {
	std::lock_guard<std::mutex> headLock(m_headMtx);
	return m_head == getTail();
}

template <typename T>
inline typename ThreadSafeQueue<T>::Node* ThreadSafeQueue<T>::popHead() {
	// Get a pointer to the current head.
	Node *old_head = m_head;

	// Update the head to the next node in the queue.
	m_head = m_head->m_next;

	return old_head;
}

template <typename T>
inline typename ThreadSafeQueue<T>::Node* ThreadSafeQueue<T>::tryPopHead() {
	std::lock_guard<std::mutex> headLock(m_headMtx);

	// If the queue is empty.
	if (m_head == getTail()) {
		return nullptr;
	}

	// Pop the head while holding the lock.
	return popHead();
}

template <typename T>
inline typename ThreadSafeQueue<T>::Node* ThreadSafeQueue<T>::waitPopHead() {
	std::unique_lock<std::mutex> headLock(waitData());
	return popHead();
}

template <typename T>
inline std::unique_lock<std::mutex> ThreadSafeQueue<T>::waitData() {
	std::unique_lock<std::mutex> headLock(m_headMtx);

	// The head mutex is already locked, so we do not call empty(),
	// which will try to lock the same mutex again.
	m_wait_pop_cv.wait(headLock, [this]() { return m_head != getTail(); });

	// Move ctor is called here.
	return headLock;
}

template <typename T>
inline std::unique_ptr<T> ThreadSafeQueue<T>::popResult(Node *old_head) {
	// Steal the data from the old head.
	std::unique_ptr<T> result(std::move(old_head->m_data));

	// Free the old head.
	delete old_head;

	// Decrease the size of the queue.
	m_size -= 1;

	// Notify a waiting push() thread that there might be enough space for a new item.
	m_wait_push_cv.notify_one();

	return result;
}

template <typename T>
inline const typename ThreadSafeQueue<T>::Node* ThreadSafeQueue<T>::getTail() const {
	std::lock_guard<std::mutex> lck(m_tailMtx);
	return m_tail;
}

template <typename T>
inline void ThreadSafeQueue<T>::freeMemory(Node *node) {
	while (node) {
		Node *to_delete = node;
		node = node->m_next;
		delete to_delete;
	}
}

#endif // !_THREAD_SAFE_QUEUE_HEADER_
