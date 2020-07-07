#include <iostream>
#include <vector>
#include <thread>
#include <cassert>

#include "LockFreeQueue.h"

int main() {
	const int num_threads = 100;
	std::vector<std::thread> threads(num_threads);
	
	Queue<int> q;

	for (int i = 0; i < num_threads; ++i) {
		q.push(i);

		std::unique_ptr<int> res = q.pop();

		assert(*res.get() == i);
		assert(q.empty());
	}

	for (int i = 0; i < num_threads; ++i) {
		q.push(i);
	}

	for (int i = 0; i < num_threads; ++i) {
		std::unique_ptr<int> res = q.pop();

		assert(*res.get() == i);
		assert(q.size() == num_threads - 1 - i);
	}
	
	for (int i = 0; i < num_threads; ++i) {
		threads[i] = std::thread(&Queue<int>::push, std::ref(q), i);
	}

	for (int i = 0; i < num_threads; ++i) {
		threads[i].join();
	}

	assert(q.size() == num_threads);

	return 0;
}
