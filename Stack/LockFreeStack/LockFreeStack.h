#pragma once
#ifndef _LOCK_FREE_THREAD_SAFE_STACK_HEADER_
#define _LOCK_FREE_THREAD_SAFE_STACK_HEADER_

#include <atomic>
#include <memory>	// Using std::shared_ptr<> for exception safety.

template <typename T>
class Stack {
	struct Node {
		std::shared_ptr<T> m_data;
		Node *m_next;

		Node(const T &data, Node *next = nullptr);
	};
	
public:
	Stack();
	Stack(const Stack &r) = delete;
	Stack& operator=(const Stack &rhs) = delete;
	~Stack();

public:
	void push(const T &value);
	std::shared_ptr<T> pop();

	size_t size() const;
	bool empty() const;

private:
	void free_memory(Node *node);
	void try_to_free_nodes(Node *node);
	void add_pending_nodes(Node *nodes);
	void add_pending_nodes(Node *first, Node *last);
	void add_pending_node(Node *node);

private:
	std::atomic<Node*> m_head;
	std::atomic<Node*> m_nodes_to_delete;
	std::atomic<size_t> m_pop_threads;
	std::atomic<size_t> m_size;
};

template <typename T>
inline Stack<T>::Node::Node(const T &data, Node *next)
	: m_data(std::make_shared<T>(data))
	, m_next(next) {

}

template <typename T>
inline Stack<T>::Stack()
	: m_head(nullptr)
	, m_nodes_to_delete(nullptr)
	, m_pop_threads(0)
	, m_size(0) {

}

template <typename T>
inline Stack<T>::~Stack() {
	free_memory(m_head.load());
	m_size = 0;
}

template <typename T>
inline void Stack<T>::push(const T &value) {
	Node *new_node = new Node(value);
	new_node->m_next = m_head.load();

	while (!m_head.compare_exchange_weak(new_node->m_next, new_node)) {
		//loop...
		//std::cout << "loop\n";
	}

	++m_size;
}

template <typename T>
inline std::shared_ptr<T> Stack<T>::pop() {
	m_pop_threads += 1;
	Node *old_head = m_head.load();

	while (old_head && !m_head.compare_exchange_weak(old_head, old_head->m_next)) {
		//loop...
	}

	std::shared_ptr<T> result;

	if (old_head) {
		// Delete the data as soon as possible when it's not needed anymore.
		result.swap(old_head->m_data);

		// Decrease size.
		--m_size;
	}

	try_to_free_nodes(old_head);
	return result;
}

template <typename T>
inline size_t Stack<T>::size() const {
	return m_size;
}

template <typename T>
inline bool Stack<T>::empty() const {
	return m_size == 0;
}

template <typename T>
inline void Stack<T>::free_memory(Node *node) {
	Node *current = node;

	while (current) {
		Node *to_delete = current;
		current = current->m_next;
		delete to_delete;
	}
}

template <typename T>
inline void Stack<T>::try_to_free_nodes(Node *node) {
	// There is only one thread in pop().
	if (m_pop_threads == 1) {
		// Get the current list of nodes, which can be deleted.
		Node *to_delete = m_nodes_to_delete.exchange(nullptr);

		if (--m_pop_threads == 0) {
			// There is still one thread in pop(), so it's safe to free the list.
			free_memory(to_delete);
		}
		else if(to_delete) {
			// If a node was added to nodes_to_delete before calling exchange(nullptr),
			// it's not safe to free the list of nodes, because there might be other threads holding this node.
			// In this case, we leave the deletion for later.
			add_pending_nodes(to_delete);
		}

		// There is only one thread in pop()(the current) that can access 'node',
		// so it's safe to delete the popped node.
		// Note: Deletion is a slow operation and we do it here in order to reduce the chance
		// of more threads entering the pop() method before we test (--m_pop_threads == 0).
		delete node;
	}
	else {
		// There might be more than one threads holding the current node, so we will try to delete it later.
		add_pending_node(node);

		// The current thread leaves pop().
		m_pop_threads -= 1;
	}
}

template <typename T>
inline void Stack<T>::add_pending_nodes(Node *nodes) {
	Node *current = nodes;

	while (current && current->m_next) {
		current = current->m_next;
	}

	add_pending_nodes(nodes, current);
}

template <typename T>
inline void Stack<T>::add_pending_nodes(Node *first, Node *last) {
	last->m_next = m_nodes_to_delete;

	while (!m_nodes_to_delete.compare_exchange_weak(last->m_next, first)) {
		//loop...
	}
}

template <typename T>
inline void Stack<T>::add_pending_node(Node *node) {
	add_pending_nodes(node, node);
}

#endif // !_LOCK_FREE_THREAD_SAFE_STACK_HEADER_
