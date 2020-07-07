#pragma once
#ifndef _LOCK_FREE_THREAD_SAFE_QUEUE_HEADER_
#define _LOCK_FREE_THREAD_SAFE_QUEUE_HEADER_

#define _ENABLE_ATOMIC_ALIGNMENT_FIX

#include <atomic>
#include <memory>

template <typename T>
class Queue {
	struct Node;

	// The sum of external count and internal count equals the number of references to the given node.

	// A Node wrapper that keeps a pointer to each node and its external count.
	struct CountedNodePtr {
		int m_externalCount;	// Every time the pointer is read, this count is increased by 1.
		Node *m_ptr;

		CountedNodePtr(int count = 0, Node *ptr = nullptr);
	};

	// We keep the number of external counters and an internal count in each Node. 
	struct NodeCounter {
		signed int m_internalCount : 30;	// Every time the reader releases the pointer(CountedNodePtr), this count is deacreased by 1.
		unsigned int m_externalCounters : 2;	// There are at most 2 external counters. We need no more than 2 bits.
	};

	struct Node {
		std::atomic<T*> m_data;
		std::atomic<NodeCounter> m_count;
		std::atomic<CountedNodePtr> m_next;

		Node();
		void releaseRef();
	};

public:
	Queue();
	Queue(const Queue &r) = delete;
	Queue& operator=(const Queue &rhs) = delete;
	~Queue();

public:
	size_t size() const;
	bool empty() const;

	void push(const T &value);
	std::unique_ptr<T> pop();

private:
	void push_non_lock_free(const T &value);
	void freeMemory();
	void setNewTail(CountedNodePtr &old_tail, CountedNodePtr &new_tail);
	static void increaseExternalCount(std::atomic<CountedNodePtr> &counter, CountedNodePtr &old_counter);
	static void freeExternalCounter(CountedNodePtr &counter);

private:
	std::atomic<size_t> m_size;
	std::atomic<CountedNodePtr> m_head;
	std::atomic<CountedNodePtr> m_tail;
};

template <typename T>
inline Queue<T>::CountedNodePtr::CountedNodePtr(int count, Node *ptr)
	: m_externalCount(count)
	, m_ptr(ptr) {

}

template <typename T>
inline Queue<T>::Node::Node()
	: m_data(nullptr)
	, m_next(CountedNodePtr()) {

	// Initially, every node is referenced from the tail and from the m_next pointer of the previous node.
	NodeCounter new_count{ 0, 2 };
	m_count.store(new_count);
}

template <typename T>
inline void Queue<T>::Node::releaseRef() {
	// Release a reference to the given node atomically.

	NodeCounter old_counter = m_count.load(std::memory_order_relaxed);
	NodeCounter new_counter;

	do {
		new_counter = old_counter;
		new_counter.m_internalCount -= 1;
	} while (!m_count.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire, std::memory_order_relaxed));

	// There are no more references to this node, so we can safely delete it.
	if (new_counter.m_internalCount == 0 && new_counter.m_externalCounters == 0) {
		delete this;
	}
}

template <typename T>
inline Queue<T>::Queue()
	: m_size(0)
	, m_head(CountedNodePtr(1, new Node))
	, m_tail(m_head.load()) {
	// We keep a dummy node at the end of the queue.
}

template <typename T>
inline Queue<T>::~Queue() {
	freeMemory();

	m_size.store(0);
	m_tail.store(m_head.load());
}

template <typename T>
inline size_t Queue<T>::size() const {
	return m_size.load();
}

template <typename T>
inline bool Queue<T>::empty() const {
	return m_size.load() == 0;
}

template <typename T>
inline void Queue<T>::push(const T &value) {
	// Prepare the new data.
	std::unique_ptr<T> new_data(new T(value));

	// Prepare the new dummy node.
	CountedNodePtr new_next;

	new_next.m_externalCount = 1;
	new_next.m_ptr = new Node;

	CountedNodePtr old_tail = m_tail.load();

	while (true) {
		// We do reference m_tail from one more place and we indicate that it's not safe to delete it.
		increaseExternalCount(m_tail, old_tail);

		T *old_data = nullptr;

		// Try to update the data of the old dummy node.
		if (old_tail.m_ptr->m_data.compare_exchange_strong(old_data, new_data.get())) {
			
			// The default constructed dummy value.
			CountedNodePtr old_next;

			// If another thread changed m_next.
			if (!old_tail.m_ptr->m_next.compare_exchange_strong(old_next, new_next)) {
				
				// We no longer need the old value.
				delete new_next.m_ptr;

				// Update new_next with the value that the other thread used.
				new_next = old_next;
			}

			// Update the tail.
			setNewTail(old_tail, new_next);

			// Do not free the data.
			new_data.release();

			// Update the size.
			m_size += 1;

			break;
		}
		else {	// This is the branch of the helper thread, which helps the main push() thread, instead of busy-waiting.

			// The default constructed dummy value.
			CountedNodePtr old_next;

			// We try to change the m_next pointer in order to help the other thread.
			if (old_tail.m_ptr->m_next.compare_exchange_strong(old_next, new_next)) {
				// Use new_next for the tail.
				old_next = new_next;

				// Prepare new_next for another push().
				new_next.m_ptr = new Node;
			}

			// Try to update the tail.
			setNewTail(old_tail, old_next);
		}
	}
}

template <typename T>
inline std::unique_ptr<T> Queue<T>::pop() {
	CountedNodePtr old_head = m_head.load(std::memory_order_relaxed);

	while (true) {
		// We do reference m_head from old_head, so we increase the external count.
		// It's safe to dereference it as it won't be deleated by another thread.
		increaseExternalCount(m_head, old_head);

		Node * const ptr = old_head.m_ptr;

		// If the queue is empty.
		if (ptr == m_tail.load().m_ptr) {
			// Release the current reference to the node.
			ptr->releaseRef();

			// Return a null pointer.
			return std::unique_ptr<T>();
		}

		CountedNodePtr next = ptr->m_next.load();

		// Try to update m_head to the next node.
		if (m_head.compare_exchange_strong(old_head, next)) {
			T * const result = ptr->m_data.exchange(nullptr);

			// There is no previous node, so we decrase the number of external counters by 1.
			// Also, we try to free the node associated with old_head.
			freeExternalCounter(old_head);

			// Update the size.
			m_size -= 1;

			// Return the result.
			return std::unique_ptr<T>(result);
		}

		// Release the current reference to the node and try again.
		ptr->releaseRef();
	}

	// Should never reach this line.
	return std::unique_ptr<T>();
}

template <typename T>
inline void Queue<T>::freeMemory() {
	while (pop() != nullptr) {
		//pop elements until the queue is empty.
	}

	CountedNodePtr node = m_head.exchange(CountedNodePtr());
	delete node.m_ptr;
}

template <typename T>
inline void Queue<T>::push_non_lock_free(const T &value) {
	// Prepare the new data.
	std::unique_ptr<T> new_data(new T(value));

	// Prepare the new dummy node.
	CountedNodePtr new_next;

	new_next.m_externalCount = 1;
	new_next.m_ptr = new Node;

	CountedNodePtr old_tail = m_tail.load();

	while (true) {
		// We do reference m_tail from one more place and we indicate that it's not safe to delete it.
		increaseExternalCount(m_tail, old_tail);

		T *old_data = nullptr;

		// Try to update the data of the old dummy node.
		if (old_tail.m_ptr->m_data.compare_exchange_strong(old_data, new_data.get())) {

			// We managed to update the data and now it's safe to modify old_tail.
			old_tail.m_ptr->m_next = new_next;

			// Set m_tail to the new dummy node and update old_tail with its old value.
			old_tail = m_tail.exchange(new_next);

			// We do not reference old_tail from two places anymore.
			// Also, it is safe to delete it if there are no more references.
			freeExternalCounter(old_tail);

			// Do not free the data.
			new_data.release();

			break;
		}

		// Release the current reference to the node.
		old_tail.m_ptr->releaseRef();
	}
}

template <typename T>
inline void Queue<T>::setNewTail(CountedNodePtr &old_tail, CountedNodePtr &new_tail) {
	Node * const current_tail_ptr = old_tail.m_ptr;

	// Update old_tail.
	// We take care not to change the value if another thread has already changed it.
	while (!m_tail.compare_exchange_weak(old_tail, new_tail) && current_tail_ptr == old_tail.m_ptr)	{
		//loop...
	}

	// We successfully set the tail, so we free the external counter.
	if (current_tail_ptr == old_tail.m_ptr) {
		freeExternalCounter(old_tail);
		return;
	}

	// Another thread has changed the tail, so we just release the current reference to the node.
	current_tail_ptr->releaseRef();
}

template <typename T>
inline void Queue<T>::increaseExternalCount(std::atomic<CountedNodePtr> &counter, CountedNodePtr &old_counter) {
	CountedNodePtr new_counter;

	do {
		new_counter = old_counter;
		new_counter.m_externalCount += 1;
	} while (!counter.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire, std::memory_order_relaxed));

	old_counter.m_externalCount = new_counter.m_externalCount;
}

template <typename T>
inline void Queue<T>::freeExternalCounter(CountedNodePtr &counter) {
	Node * const ptr = counter.m_ptr;
	const int count_increase = counter.m_externalCount - 2; // One from the list and one from the current thread.

	NodeCounter old_counter = ptr->m_count.load(std::memory_order_relaxed);
	NodeCounter new_counter;

	// Update both counts atomically in order to prevent the case of two threads deleteing the node.
	do {
		new_counter = old_counter;
		new_counter.m_externalCounters -= 1;
		new_counter.m_internalCount += count_increase;
	} while (!ptr->m_count.compare_exchange_strong(old_counter, new_counter, std::memory_order_acquire, std::memory_order_relaxed));

	// There are no more references to the node in 'counter', so it's safe to delete it.
	if (new_counter.m_internalCount == 0 && new_counter.m_externalCounters == 0) {
		delete ptr;
		counter.m_ptr = nullptr;
	}
}

#endif // !_LOCK_FREE_THREAD_SAFE_QUEUE_HEADER_
