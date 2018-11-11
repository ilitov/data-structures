#pragma once
#ifndef DYNAMIC_ARRAY_CLASS_HEADER
#define DYNAMIC_ARRAY_CLASS_HEADER

template <typename T>
class DynamicArrayProxy;

template <typename T>
class DynamicArray {
public:
	template <typename IT>
	class iterator_base {
		friend class DynamicArray<T>;
		iterator_base(DynamicArray<T> *arr, size_t pos);

	public:
		using value_type = IT;
		using reference = IT&;
		using pointer = IT*;

		bool operator==(const iterator_base &r) const;
		bool operator!=(const iterator_base &r) const;

		bool operator<(const iterator_base &r) const;
		bool operator<=(const iterator_base &r) const;

		bool operator>(const iterator_base &r) const;
		bool operator>=(const iterator_base &r) const;

		iterator_base& operator++();
		iterator_base operator++(int);

		iterator_base& operator--();
		iterator_base operator--(int);

		reference operator*() const;
		pointer operator->() const;

	private:
		DynamicArray<T> *arr;
		size_t pos;
	};

	using iterator = iterator_base<T>;
	using const_iterator = iterator_base<const T>;

	iterator begin();
	iterator end();

	const_iterator begin() const;
	const_iterator end() const;

public:
	DynamicArray(size_t capacity = 0);
	DynamicArray(const DynamicArray &r);
	DynamicArray& operator=(const DynamicArray &rhs);
	~DynamicArray();

public:
	void push_back(const T &value);
	void pop_back();

	T& getAt(size_t idx);
	const T& getAt(size_t idx) const;
	void setAt(size_t idx, const T &value);

	DynamicArray operator+(const DynamicArray &rhs) const;
	DynamicArray& operator+=(const DynamicArray &rhs);

	DynamicArrayProxy<T> operator[](size_t idx);
	const DynamicArrayProxy<T> operator[](size_t idx) const;

	void reserve(size_t newCapacity);
	void shrink_to_fit();
	void swap(DynamicArray &r);

	bool empty() const;
	size_t getSize() const;
	size_t getCapacity() const;

private:
	void reallocate(size_t newSize);
	void resize(size_t newSize);
	void free();
	void copy(const DynamicArray &r);
	size_t calcSizeIncrease(size_t newSize) const;

private:
	T *data;
	size_t size;
	size_t capacity;
};


/* --- DYNAMIC ARRAY --- */
template <typename T>
inline DynamicArray<T>::DynamicArray(size_t capacity)
	: data(nullptr)
	, size(0)
	, capacity(0) {
	reallocate(capacity);
}

template <typename T>
inline DynamicArray<T>::DynamicArray(const DynamicArray<T> &r)
	: DynamicArray<T>(r.capacity) {
	try {
		copy(r);
		size = r.size;
	}
	catch (...) {
		free();
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
	free();
}

template <typename T>
inline void DynamicArray<T>::push_back(const T &el) {
	if (size >= capacity) {
		resize(capacity + 1);
	}

	data[size] = el;
	++size;
}

template <typename T>
inline void DynamicArray<T>::pop_back() {
	if (size) {
		--size;
		return;
	}

	throw std::logic_error("Array is empty!");
}

template <typename T>
inline T& DynamicArray<T>::getAt(size_t idx) {
	return const_cast<T&>(static_cast<const DynamicArray<T>&>(*this).getAt(idx));
}

template <typename T>
inline const T& DynamicArray<T>::getAt(size_t idx) const {
	if (idx >= size) {
		std::out_of_range("Invalid index!");
	}

	return data[idx];
}

template <typename T>
inline void DynamicArray<T>::setAt(size_t idx, const T &value) {
	if (idx >= size) {
		throw std::out_of_range("Invalid index!");
	}

	data[idx] = value;
}

template <typename T>
inline DynamicArray<T> DynamicArray<T>::operator+(const DynamicArray<T> &rhs) const {
	DynamicArray<T> tmp(*this);
	tmp += rhs;
	return tmp;
}

template <typename T>
inline DynamicArray<T>& DynamicArray<T>::operator+=(const DynamicArray<T> &rhs) {
	reserve(capacity + rhs.capacity);

	for (size_t i = 0; i < rhs.size; ++i) {
		push_back(rhs.data[i]);
	}

	return *this;
}

template <typename T>
inline DynamicArrayProxy<T> DynamicArray<T>::operator[](size_t idx) {
	return DynamicArrayProxy<T>(this, idx);
}

template <typename T>
inline const DynamicArrayProxy<T> DynamicArray<T>::operator[](size_t idx) const {
	return const_cast<DynamicArray<T>&>(*this)[idx];
}

template <typename T>
inline void DynamicArray<T>::reserve(size_t newCapacity) {
	resize(newCapacity);
}

template <typename T>
inline void DynamicArray<T>::shrink_to_fit() {
	reallocate(size);
}

template <typename T>
inline bool DynamicArray<T>::empty() const {
	return size == 0;
}

template <typename T>
inline size_t DynamicArray<T>::getSize() const {
	return size;
}

template <typename T>
inline size_t DynamicArray<T>::getCapacity() const {
	return capacity;
}

template <typename T>
inline void DynamicArray<T>::reallocate(size_t newSize) {
	if (capacity == newSize) {
		return;
	}

	size_t elmntsToCopy = size < newSize ? size : newSize;
	T *tmp = nullptr;

	if (newSize > 0) {
		try {
			tmp = new T[newSize];

			for (size_t i = 0; i < elmntsToCopy; ++i) {
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
	size = elmntsToCopy;
	capacity = newSize;
}

template <typename T>
inline void DynamicArray<T>::resize(size_t newSize) {
	reallocate(calcSizeIncrease(newSize));
}

template <typename T>
inline void DynamicArray<T>::free() {
	delete[] data;
	data = nullptr;

	size = 0;
	capacity = 0;
}

template <typename T>
inline void DynamicArray<T>::copy(const DynamicArray<T> &r) {
	for (size_t i = 0; i < r.size; ++i) {
		data[i] = r.data[i];
	}
}

template <typename T>
inline void DynamicArray<T>::swap(DynamicArray<T> &r) {
	T *tmpData = data;
	size_t tmpSize = size;
	size_t tmpCapacity = capacity;

	data = r.data;
	size = r.size;
	capacity = r.capacity;

	r.data = tmpData;
	r.size = tmpSize;
	r.capacity = tmpCapacity;
}

template <typename T>
inline size_t DynamicArray<T>::calcSizeIncrease(size_t newSize) const {
	if (newSize < capacity) {
		return capacity;
	}

	size_t resultSize = capacity ? capacity * 2 : 1;
	return resultSize < newSize ? newSize : resultSize;
}

template <typename T>
inline typename DynamicArray<T>::iterator DynamicArray<T>::begin() {
	return iterator(this, 0);
}

template <typename T>
inline typename DynamicArray<T>::iterator DynamicArray<T>::end() {
	return iterator(this, size);
}

template <typename T>
inline typename DynamicArray<T>::const_iterator DynamicArray<T>::begin() const {
	return const_iterator(this, 0);
}

template <typename T>
inline typename DynamicArray<T>::const_iterator DynamicArray<T>::end() const {
	return const_iterator(this, size);
}

/* --- ITERATOR --- */
template <typename T>
template <typename IT>
inline DynamicArray<T>::iterator_base<IT>::iterator_base(DynamicArray<T> *arr, size_t pos)
	: arr(arr)
	, pos(pos) {

}

template <typename T>
template <typename IT>
inline bool DynamicArray<T>::iterator_base<IT>::operator==(const DynamicArray<T>::iterator_base<IT> &r) const {
	return (arr == r.arr && pos == r.pos);
}

template <typename T>
template <typename IT>
inline bool DynamicArray<T>::iterator_base<IT>::operator!=(const DynamicArray<T>::iterator_base<IT> &r) const {
	return !(*this == r);
}

template <typename T>
template <typename IT>
inline bool DynamicArray<T>::iterator_base<IT>::operator<(const DynamicArray<T>::iterator_base<IT> &r) const {
	return (arr == r.arr && pos < r.pos);
}

template <typename T>
template <typename IT>
inline bool DynamicArray<T>::iterator_base<IT>::operator<=(const DynamicArray<T>::iterator_base<IT> &r) const {
	return (*this == r || *this < r);
}

template <typename T>
template <typename IT>
inline bool DynamicArray<T>::iterator_base<IT>::operator>(const DynamicArray<T>::iterator_base<IT> &r) const {
	return !(*this <= r);
}

template <typename T>
template <typename IT>
inline bool DynamicArray<T>::iterator_base<IT>::operator>=(const DynamicArray<T>::iterator_base<IT> &r) const {
	return !(*this < r);
}

template <typename T>
template <typename IT>
inline typename DynamicArray<T>::iterator_base<IT>& DynamicArray<T>::iterator_base<IT>::operator++() {
	++pos;
	return *this;
}

template <typename T>
template <typename IT>
inline typename DynamicArray<T>::iterator_base<IT> DynamicArray<T>::iterator_base<IT>::operator++(int) {
	iterator_base<IT> res(*this);
	++(*this);
	return res;
}

template <typename T>
template <typename IT>
inline typename DynamicArray<T>::iterator_base<IT>& DynamicArray<T>::iterator_base<IT>::operator--() {
	if (pos > 0) {
		--pos;
	}

	return *this;
}

template <typename T>
template <typename IT>
inline typename DynamicArray<T>::iterator_base<IT> DynamicArray<T>::iterator_base<IT>::operator--(int) {
	iterator_base<IT> res(*this);
	--(*this);
	return res;
}

template <typename T>
template <typename IT>
inline typename DynamicArray<T>::iterator_base<IT>::reference DynamicArray<T>::iterator_base<IT>::operator*() const {
	return arr->getAt(pos);
}

template <typename T>
template <typename IT>
inline typename DynamicArray<T>::iterator_base<IT>::pointer DynamicArray<T>::iterator_base<IT>::operator->() const {
	return &(operator*());
}

/* std::swap */
template <typename T>
inline void swap(DynamicArray<T> &a, DynamicArray<T> &b) {
	a.swap(b);
}

#endif // !DYNAMIC_ARRAY_CLASS_HEADER
