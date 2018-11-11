#include <iostream>
#include "Queue.h"

int main() {
	Queue<int> q;

	for (int i = 0; i < 10; ++i) {
		q.push(i);
	}

	try {
		for (int i = 0; i < 11; ++i) {
			std::cout << "Size: " << q.getSize() << '\n';
			std::cout << q.front() << '\n';
			q.pop();
		}
	}
	catch (std::logic_error &x) {
		std::cout << x.what() << '\n';
	}

	return 0;
}
