#include <iostream>
#include "Stack.h"

int main() {
	Stack<int, 10> stack;
	for (int i = 0; i < 10; ++i) {
		stack.push(i);
	}

	std::cout << stack.top() << '\n';

	for (int i = 0; i < 10; ++i) {
		std::cout << stack.top() << '\n';
		stack.pop();
	}

	return 0;
}
