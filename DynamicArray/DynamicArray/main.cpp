#include <iostream>
#include "DynamicArray.h"

int main() {
	DynamicArray<int> a;
	for (int i = 0; i < 10; ++i) {
		a.insert(a.begin(), i);
	}

	std::cout << a.at(5) << "\n\n";

	for (DynamicArray<int>::iterator it = a.begin(); it != a.end(); ++it) {
		std::cout << *it << '\n';
	}

	return 0;
}
