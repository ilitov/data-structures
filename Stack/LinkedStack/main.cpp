#include <iostream>
#include "Stack.h"

int main() {
	Stack<int> s;

	for (int i = 0; i < 100; ++i) {
		s.push(i);
	}

	while (!s.empty()) {
		std::cout << s.top() << '\n';
		s.pop();
	}

	return 0;
}
