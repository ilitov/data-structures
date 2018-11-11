#include <iostream>
#include "Stack.h"

#include <list>
#include <vector>

template <typename T, typename Container>
void test() {
	Stack<T, Container> a;
	
	for (int i = 0; i < 10; ++i) {
		a.push(i);
	}

	for (int i = 0; i < 10; ++i) {
		std::cout << a.top() << '\n';
		a.pop();
	}

	std::putchar('\n');
}

int main() {
	std::cout << "std::list\n";
	test<int, std::list<int>>();

	std::cout << "std::vector\n";
	test<int, std::vector<int>>();

	return 0;
}
