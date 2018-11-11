#pragma once
#ifndef DYNAMIC_ARRAY_PROXY_CLASS_HEADER
#define DYNAMIC_ARRAY_PROXY_CLASS_HEADER

#include "DynamicArray.h"

template <typename T>
class DynamicArrayProxy {
public:
	DynamicArrayProxy(DynamicArray<T> *arr, size_t idx);

public:
	DynamicArrayProxy& operator=(const T &value);
	operator T() const;

private:
	DynamicArray<T> *arr;
	size_t idx;
};

template <typename T>
inline DynamicArrayProxy<T>::DynamicArrayProxy(DynamicArray<T> *arr, size_t idx)
	: arr(arr)
	, idx(idx) {

}

template <typename T>
inline DynamicArrayProxy<T>& DynamicArrayProxy<T>::operator=(const T &value) {
	arr->setAt(idx, value);
	return *this;
}

template <typename T>
inline DynamicArrayProxy<T>::operator T() const {
	return arr->getAt(idx);
}

#endif // !DYNAMIC_ARRAY_PROXY_CLASS_HEADER
