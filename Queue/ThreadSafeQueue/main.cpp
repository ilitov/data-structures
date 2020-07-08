#include <iostream>
#include <cassert>
#include <vector>
#include <thread>

#include "ThreadSafeQueue.h"

void testQueue(int num_threads, int max_size) {
	ThreadSafeQueue<int> queue(max_size);

	std::vector<std::thread> threads(num_threads);

	int push_threads = max_size < num_threads ? max_size + 1 : num_threads;
	int i = 0;

	for (; i < push_threads; ++i) {
		threads[i] = std::thread(&ThreadSafeQueue<int>::push, std::ref(queue), i);
	}

	for (; i < num_threads; ++i) {
		threads[i] = std::thread(&ThreadSafeQueue<int>::try_pop, std::ref(queue));
	}

	for (int i = 0; i < num_threads; ++i) {
		threads[i].join();
	}

	assert(queue.size() == push_threads == num_threads ? num_threads : num_threads - push_threads - 1 - 1);

	// Pop if there is anything left.
	for (int i = 0; i < num_threads; ++i) {
		threads[i] = std::thread(&ThreadSafeQueue<int>::try_pop, std::ref(queue));
	}

	for (int i = 0; i < num_threads; ++i) {
		threads[i].join();
	}

	assert(queue.size() == 0);

	ThreadSafeQueue<int> queue2;

	for (int i = 0; i < num_threads / 2; ++i) {
		threads[i] = std::thread(&ThreadSafeQueue<int>::wait_pop, std::ref(queue2));
	}

	for (int i = num_threads / 2; i < num_threads; ++i) {
		threads[i] = std::thread(&ThreadSafeQueue<int>::push, std::ref(queue2), i);
	}

	for (int i = 0; i < num_threads; ++i) {
		threads[i].join();
	}

	assert(queue2.empty());
}

int main() {
	testQueue(10, 5);
	testQueue(10, 10);
	testQueue(10, 100);

	testQueue(100, 20);
	testQueue(100, 100);
	testQueue(100, 1000);

	return 0;
}
