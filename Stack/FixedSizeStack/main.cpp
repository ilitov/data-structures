#include <iostream>
#include "Stack.h"

int main() {
	Stack<int> a(10);
	Stack<int> b(1);

	b = a;

	return 0;
}
