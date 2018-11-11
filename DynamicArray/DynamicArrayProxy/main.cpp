#include <iostream>
#include "DynamicArray.h"
#include "DynamicArrayProxy.h"

int main() {
	DynamicArray<int> arr;
	const DynamicArray<int> arr2;

	std::cout << arr[0] << '\n';
	arr[0] = 20;

	std::cout << arr2[0] << '\n';
	//arr2[0] = 20;

	return 0;
}
