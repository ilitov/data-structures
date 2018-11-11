#pragma once
#ifndef DYNAMIC_ARRAY_CLASS_HEADER
#define DYNAMIC_ARRAY_CLASS_HEADER

#include <stdexcept>

template <typename T>
class DynamicArray {
public:
	using value_type = T;
	using size_type = size_t;
	using reference = T&;
	using const_reference = const T&;
	using pointer = T*;
	using const_pointer = const T*;

public:
	class iterator;
	class const_iterator;

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;

	const_iterator cbegin() const;
	const_iterator cend() const;

public:
	DynamicArray(size_type capacity = 0);
	DynamicArray(const DynamicArray &r);
	DynamicArray& operator=(const DynamicArray &rhs);
	~DynamicArray();

public:
	reference at(size_type pos);
	const_reference at(size_type pos) const;

	reference front();
	const_reference front() const;

	reference back();
	const_reference back() const;

	reference operator[](size_type pos);
	const_reference operator[](size_type pos) const;

	void pop_back();
	void push_back(const_reference value);

	void reserve(size_type newCapacity);
	void shrink_to_fit();

	bool empty() const;
	size_type size() const;
	size_type capacity() const;

	iterator erase(iterator pos);
	iterator insert(iterator pos, const_reference value);

	void clear();
	void swap(DynamicArray &other);

private:
	void reallocate(size_type newCapacity);
	void resize(size_type newCapacity);
	void copyFrom(const DynamicArray<T> &other);
	size_type calcSizeIncrease(size_type newCapacity) const;

private:
	value_type *data;
	size_type m_size;
	size_type m_capacity;
};

template <typename T>
class DynamicArray<T>::iterator {
	friend class DynamicArray<T>;
	iterator(DynamicArray<T> *arr, size_type pos);

public:
	bool operator==(const iterator &r) const;
	bool operator!=(const iterator &r) const;

	bool operator<(const iterator &r) const;
	bool operator<=(const iterator &r) const;

	bool operator>(const iterator &r) const;
	bool operator>=(const iterator &r) const;

	iterator& operator++();
	iterator operator++(int);

	iterator& operator--();
	iterator operator--(int);

	reference operator*();
	pointer operator->();

private:
	DynamicArray<T> *arr;
	size_type pos;
};

template <typename T>
class DynamicArray<T>::const_iterator {
	friend class DynamicArray<T>;
	const_iterator(const DynamicArray<T> *arr, size_type pos);

public:
	bool operator==(const const_iterator &r) const;
	bool operator!=(const const_iterator &r) const;

	bool operator<(const const_iterator &r) const;
	bool operator<=(const const_iterator &r) const;

	bool operator>(const const_iterator &r) const;
	bool operator>=(const const_iterator &r) const;

	const_iterator& operator++();
	const_iterator operator++(int);

	const_iterator& operator--();
	const_iterator operator--(int);

	const_reference operator*() const;
	const_pointer operator->() const;

private:
	const DynamicArray<T> *arr;
	size_type pos;
};

/* --- DYNAMIC ARRAY --- */
template <typename T>
inline DynamicArray<T>::DynamicArray(size_type capacity)
	: data(nullptr)
	, m_size(0)
	, m_capacity(0) {
	reallocate(capacity);
}

template <typename T>
inline DynamicArray<T>::DynamicArray(const DynamicArray<T> &r)
	: DynamicArray<T>(r.m_capacity) {
	try {
		copyFrom(r);
	}
	catch (...) {
		clear();
		throw;
	}
}

template <typename T>
inline DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T> &rhs) {
	if (this != &rhs) {
		DynamicArray<T> tmp(rhs);
		swap(tmp);
	}

	return *this;
}

template <typename T>
inline DynamicArray<T>::~DynamicArray() {
	clear();
}

template <typename T>
inline typename DynamicArray<T>::reference DynamicArray<T>::at(size_type pos) {
	return const_cast<reference>(static_cast<const DynamicArray<T>&>(*this).at(pos));
}

template <typename T>
inline typename DynamicArray<T>::const_reference DynamicArray<T>::at(size_type pos) const {
	if (pos >= m_size) {
		throw std::out_of_range("Invalid position!");
	}

	return data[pos];
}

template <typename T>
inline typename DynamicArray<T>::reference DynamicArray<T>::front() {
	return const_cast<reference>(static_cast<const DynamicArray<T>&>(*this).front());
}

template <typename T>
inline typename DynamicArray<T>::const_reference DynamicArray<T>::front() const {
	if (empty()) {
		throw std::logic_error("Empty array!");
	}

	return data[0];
}

template <typename T>
inline typename DynamicArray<T>::reference DynamicArray<T>::back() {
	return const_cast<reference>(static_cast<const DynamicArray<T>&>(*this).back());
}

template <typename T>
inline typename DynamicArray<T>::const_reference DynamicArray<T>::back() const {
	if (empty()) {
		throw std::logic_error("Empty array!");
	}

	return data[m_size - 1];
}

template <typename T>
inline typename DynamicArray<T>::reference DynamicArray<T>::operator[](size_type pos) {
	return const_cast<reference>(static_cast<const DynamicArray<T>&>(*this)[pos]);
}

template <typename T>
inline typename DynamicArray<T>::const_reference DynamicArray<T>::operator[](size_type pos) const {
	return data[pos];
}

template <typename T>
inline void DynamicArray<T>::pop_back() {
	if (!empty()) {
		--m_size;
	}
}

template <typename T>
inline void DynamicArray<T>::push_back(const_reference value) {
	if (m_size >= m_capacity) {
		reserve(m_size + 1);
	}

	data[m_size] = value;
	++m_size;
}

template <typename T>
inline void DynamicArray<T>::reserve(size_type newCapacity) {
	resize(newCapacity);
}

template <typename T>
inline void DynamicArray<T>::shrink_to_fit() {
	reallocate(m_size);
}

template <typename T>
inline bool DynamicArray<T>::empty() const {
	return !size();
}

template <typename T>
inline typename DynamicArray<T>::size_type DynamicArray<T>::size() const {
	return m_size;
}

template <typename T>
inline typename DynamicArray<T>::size_type DynamicArray<T>::capacity() const {
	return m_capacity;
}

template <typename T>
inline typename DynamicArray<T>::iterator DynamicArray<T>::erase(iterator pos) {
	iterator resultIterator = end();
	if (pos != end()) {
		size_type i = pos.pos;
		resultIterator = iterator(this, i);

		for (; i < m_size - 1; ++i) {
			data[i] = data[i + 1];
		}

		pop_back();
	}

	return resultIterator;
}

template <typename T>
inline typename DynamicArray<T>::iterator
DynamicArray<T>::insert(iterator pos, const_reference value) {
	iterator resultIterator = end();
	if (pos != end()) {
		reserve(m_size + 1);
		size_type insertPos = pos.pos;

		for (size_type i = m_size; i > insertPos; --i) {
			data[i] = data[i - 1];
		}

		data[insertPos] = value;
		++m_size;
	}
	else {
		push_back(value);
		resultIterator = iterator(this, m_size - 1);
	}

	return resultIterator;
}

template <typename T>
inline void DynamicArray<T>::clear() {
	delete[] data;
	data = nullptr;

	m_size = 0;
	m_capacity = 0;
}

template <typename T>
inline void DynamicArray<T>::swap(DynamicArray<T> &other) {
	value_type *tmpData = data;
	data = other.data;
	other.data = tmpData;

	size_type tmpSize = m_size;
	m_size = other.m_size;
	other.m_size = tmpSize;

	size_type tmpCapacity = m_capacity;
	m_capacity = other.m_capacity;
	other.m_capacity = tmpCapacity;
}

template <typename T>
inline void DynamicArray<T>::reallocate(size_type newCapacity) {
	if (newCapacity == m_capacity) {
		return;
	}

	value_type *tmp = nullptr;
	size_type elmntsToCopy = newCapacity < m_size ? newCapacity : m_size;

	if (newCapacity) {
		try {
			tmp = new value_type[newCapacity];

			for (size_type i = 0; i < elmntsToCopy; ++i) {
				tmp[i] = data[i];
			}
		}
		catch (...) {
			delete[] tmp;
			throw;
		}
	}

	delete[] data;
	data = tmp;
	m_size = elmntsToCopy;
	m_capacity = newCapacity;
}

template <typename T>
inline void DynamicArray<T>::resize(size_type newCapacity) {
	return reallocate(calcSizeIncrease(newCapacity));
}

template <typename T>
inline void DynamicArray<T>::copyFrom(const DynamicArray<T> &other) {
	for (const_iterator it = other.begin(); it != other.end(); ++it) {
		push_back(*it);
	}
}

template <typename T>
inline typename DynamicArray<T>::size_type DynamicArray<T>::calcSizeIncrease(size_type newCapacity) const {
	if (newCapacity < m_capacity) {
		return m_capacity;
	}

	size_type resultCapacity = m_capacity ? m_capacity * 2 : 1;
	return resultCapacity < newCapacity ? newCapacity : resultCapacity;
}


template <typename T>
inline typename DynamicArray<T>::iterator DynamicArray<T>::begin() {
	return iterator(this, 0);
}

template <typename T>
inline typename DynamicArray<T>::iterator DynamicArray<T>::end() {
	return iterator(this, m_size);
}

template <typename T>
inline typename DynamicArray<T>::const_iterator DynamicArray<T>::begin() const {
	return const_iterator(this, 0);
}

template <typename T>
inline typename DynamicArray<T>::const_iterator DynamicArray<T>::end() const {
	return const_iterator(this, m_size);
}

template <typename T>
inline typename DynamicArray<T>::const_iterator DynamicArray<T>::cbegin() const {
	return begin();
}

template <typename T>
inline typename DynamicArray<T>::const_iterator DynamicArray<T>::cend() const {
	return end();
}

/* --- ITERATOR --- */
template <typename T>
inline DynamicArray<T>::iterator::iterator(DynamicArray<T> *arr, size_t pos)
	: arr(arr)
	, pos(pos) {

}

template <typename T>
inline bool DynamicArray<T>::iterator::operator==(const iterator &r) const {
	return (arr == r.arr && pos == r.pos);
}

template <typename T>
inline bool DynamicArray<T>::iterator::operator!=(const iterator &r) const {
	return !(*this == r);
}

template <typename T>
inline bool DynamicArray<T>::iterator::operator<(const iterator &r) const {
	return (arr == r.arr && pos < r.pos);
}

template <typename T>
inline bool DynamicArray<T>::iterator::operator<=(const iterator &r) const {
	return (*this == r || *this < r);
}

template <typename T>
inline bool DynamicArray<T>::iterator::operator>(const iterator &r) const {
	return !(*this <= r);
}

template <typename T>
inline bool DynamicArray<T>::iterator::operator>=(const iterator &r) const {
	return !(*this < r);
}

template <typename T>
inline typename DynamicArray<T>::iterator& DynamicArray<T>::iterator::operator++() {
	++pos;
	return *this;
}

template <typename T>
inline typename DynamicArray<T>::iterator DynamicArray<T>::iterator::operator++(int) {
	iterator res(*this);
	++(*this);
	return res;
}

template <typename T>
inline typename DynamicArray<T>::iterator& DynamicArray<T>::iterator::operator--() {
	if (pos) {
		--pos;
	}

	return *this;
}

template <typename T>
inline typename DynamicArray<T>::iterator DynamicArray<T>::iterator::operator--(int) {
	iterator res(*this);
	--(*this);
	return res;
}

template <typename T>
inline typename DynamicArray<T>::reference DynamicArray<T>::iterator::operator*() {
	return arr->at(pos);
}

template <typename T>
inline typename DynamicArray<T>::pointer DynamicArray<T>::iterator::operator->() {
	return &(operator*());
}

/* --- CONST_ITERATOR METHODS --- */
template <typename T>
inline DynamicArray<T>::const_iterator::const_iterator(const DynamicArray<T> *arr, size_t pos)
	: arr(arr)
	, pos(pos) {

}

template <typename T>
inline bool DynamicArray<T>::const_iterator::operator==(const const_iterator &r) const {
	return (arr == r.arr && pos == r.pos);
}

template <typename T>
inline bool DynamicArray<T>::const_iterator::operator!=(const const_iterator &r) const {
	return !(*this == r);
}

template <typename T>
inline bool DynamicArray<T>::const_iterator::operator<(const const_iterator &r) const {
	return (arr == r.arr && pos < r.pos);
}

template <typename T>
inline bool DynamicArray<T>::const_iterator::operator<=(const const_iterator &r) const {
	return (*this == r || *this < r);
}

template <typename T>
inline bool DynamicArray<T>::const_iterator::operator>(const const_iterator &r) const {
	return !(*this <= r);
}

template <typename T>
inline bool DynamicArray<T>::const_iterator::operator>=(const const_iterator &r) const {
	return !(*this < r);
}

template <typename T>
inline typename DynamicArray<T>::const_iterator& DynamicArray<T>::const_iterator::operator++() {
	++pos;
	return *this;
}

template <typename T>
inline typename DynamicArray<T>::const_iterator DynamicArray<T>::const_iterator::operator++(int) {
	const_iterator res(*this);
	++(*this);
	return res;
}

template <typename T>
inline typename DynamicArray<T>::const_iterator& DynamicArray<T>::const_iterator::operator--() {
	if (pos) {
		--pos;
	}

	return *this;
}

template <typename T>
inline typename DynamicArray<T>::const_iterator DynamicArray<T>::const_iterator::operator--(int) {
	const_iterator res(*this);
	--(*this);
	return res;
}

template <typename T>
inline typename DynamicArray<T>::const_reference DynamicArray<T>::const_iterator::operator*() const {
	return arr->at(pos);
}

template <typename T>
inline typename DynamicArray<T>::const_pointer DynamicArray<T>::const_iterator::operator->() const {
	return &(operator*());
}

/* std::swap */
template <typename T>
inline void swap(DynamicArray<T> &lhs, DynamicArray<T> &rhs) {
	lhs.swap(rhs);
}

#endif // !DYNAMIC_ARRAY_CLASS_HEADER
