#include <iostream>
#include <vector>
#include <stack>

#include <thread>
#include <mutex>

#include "LockFreeStack.h"

std::mutex mtx;

void joinThreads(std::vector<std::thread> &threads) {
	for (std::thread &t : threads) {
		t.join();
	}
}

template <typename T>
void pushToSTDStack(std::stack<T> &s, const T &value) {
	std::lock_guard<std::mutex> lck(mtx);
	for (int i = 0; i < 100; ++i) {
		s.push(value);
	}
}

template <typename T>
void pushToStack(Stack<T> &s, const T &value) {
	for (int i = 0; i < 100; ++i) {
		s.push(value);
	}
}

template <typename T>
void popFromStack(Stack<T> &s) {
	for (int i = 0; i < 100; ++i) {
		const std::shared_ptr<T> &value = s.pop();
		// do smth...
	}
}

int main() {
	const int num_threads = 100;

	Stack<int> stack;
	std::stack<int> s;
	
	std::vector<std::thread> threads(num_threads);

	for (int i = 0; i < num_threads; ++i) {
		//threads[i] = std::thread(&pushToSTDStack<int>, std::ref(s), i);
		threads[i] = std::thread(&pushToStack<int>, std::ref(stack), i);
	}

	joinThreads(threads);

	for (int i = 0; i < num_threads; ++i) {
		threads[i] = std::thread(&popFromStack<int>, std::ref(stack));
	}

	joinThreads(threads);

	return 0;
}
