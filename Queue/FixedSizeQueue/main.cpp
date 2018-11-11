#include <iostream>
#include "Queue.h"

int main() {
	Queue<int> q(10);

	try {
		for (int i = 0; i < 10; ++i) {
			q.push(i);
		}

		for (int i = 0; i < 10; ++i) {
			std::cout << q.front() << '\n';
			q.pop();
		}
	}
	catch (std::logic_error &x) {
		std::cout << x.what() << '\n';
	}

	return 0;
}
