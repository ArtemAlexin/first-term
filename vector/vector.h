#pragma once

#include <cstdint>
#include <algorithm>
#include <cassert>

template<typename T>
struct vector {
    typedef T *iterator;
    typedef T const *const_iterator;

    vector() noexcept;                                         // O(1) nothrow
    vector(vector const &);                                    // O(N) strong
    vector &operator=(vector const &w);                    // O(N) strong

    ~vector() noexcept;                                        // O(N) nothrow

    T &operator[](size_t i) noexcept;                          // O(1) nothrow
    T const &operator[](size_t i) const noexcept;              // O(1) nothrow

    T *data() noexcept;                                        // O(1) nothrow
    T const *data() const noexcept;                            // O(1) nothrow
    size_t size() const noexcept;                              // O(1) nothrow

    T &front() noexcept;                                       // O(1) nothrow
    T const &front() const noexcept;                           // O(1) nothrow

    T &back() noexcept;                                        // O(1) nothrow
    T const &back() const noexcept;                            // O(1) nothrow
    void push_back(T const &);                                 // O(1)* strong
    void pop_back() noexcept;                                  // O(1) nothrow

    bool empty() const noexcept;                               // O(1) nothrow

    size_t capacity() const noexcept;                          // O(1) nothrow
    void reserve(size_t);                                      // O(N) strong
    void shrink_to_fit();                                      // O(N) strong

    void clear() noexcept;                                     // O(N) nothrow

    void swap(vector &) noexcept;                              // O(1) nothrow

    iterator begin() noexcept;                                 // O(1) nothrow
    iterator end() noexcept;                                   // O(1) nothrow

    const_iterator begin() const noexcept;                     // O(1) nothrow
    const_iterator end() const noexcept;                       // O(1) nothrow

    iterator insert(const_iterator, T const &);                // O(N) weak

    iterator erase(const_iterator);                            // O(N) weak

    iterator erase(const_iterator, const_iterator);            // O(N) weak

private:
    T *data_;
    size_t size_;
    size_t capacity_;

    void extend();

    void reBuild(size_t cap);
};

template<typename T>
void deleteAllElements(T *data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        data[i].~T();
    }
}

template<typename T>
T* newArr(T *src, size_t size, size_t cap) {
    size_t i = 0;
    T *arr = static_cast<T *>(operator new(sizeof(T) * cap));
    try {
        for (; i < size; ++i) {
            new(arr + i) T(src[i]);
        }
    } catch (...) {
        deleteAllElements(arr, i);
        operator delete(arr);
        throw;
    }
    return arr;
}

template<typename T>
vector<T>::vector() noexcept : data_(nullptr), size_(0), capacity_(0) {}

template<typename T>
vector<T>::vector(vector<T> const &other) : vector() {
    if (!other.empty()) {
        data_ = newArr(other.data_, other.size_, other.size_);
        capacity_ = size_ = other.size_;
    }
}

template<typename T>
vector<T> &vector<T>::operator=(vector<T> const &w) {
    vector<T> tmp(w);
    swap(tmp);
    return *this;
}

template<typename T>
vector<T>::~vector() noexcept {
    deleteAllElements(data_, size_);
    operator delete(data_);
}

template<typename T>
T &vector<T>::operator[](size_t id) noexcept {
    return data_[id];
}

template<typename T>
T const &vector<T>::operator[](size_t id) const noexcept {
    return data_[id];
}

template<typename T>
T const *vector<T>::data() const noexcept {
    return data_;
}

template<typename T>
T *vector<T>::data() noexcept {
    return data_;
}

template<typename T>
T &vector<T>::front() noexcept {
    return data_[0];
}

template<typename T>
size_t vector<T>::size() const noexcept {
    return size_;
}

template<typename T>
size_t vector<T>::capacity() const noexcept {
    return capacity_;
}

template<typename T>
T const &vector<T>::front() const noexcept {
    return data_[0];
}


template<typename T>
T &vector<T>::back() noexcept {
    return data_[size_ - 1];
}

template<typename T>
T const &vector<T>::back() const noexcept {
    return data_[size_ - 1];
}

template<typename T>
bool vector<T>::empty() const noexcept {
    return size_ == 0;
}

template<typename T>
void vector<T>::reBuild(size_t cap) {
    if (capacity_ == cap)
        return;
    assert(cap >= size_);
    T *tmp = newArr(data_, size_, cap);
    capacity_ = cap;
    deleteAllElements(data_, size_);
    operator delete(data_);
    data_ = tmp;
}

template<typename T>
void vector<T>::shrink_to_fit() {
    if (!empty()) {
        if (size_ < capacity_) {
            reBuild(size_);
        }
        return;
    }
    operator delete(data_);
    data_ = nullptr;
}

template<typename T>
void vector<T>::clear() noexcept {
    deleteAllElements(data_, size_);
    size_ = 0;
}
template<typename T>
void vector<T>::push_back(const T &v) {
    if (size_ < capacity_) {
        new(end()) T(v);
    } else {
        T tmp = v;
        extend();
        new(end()) T(tmp);
    }
    size_++;
}

template<typename T>
void vector<T>::pop_back() noexcept {
    data_[--size_].~T();
}

template<typename T>
void vector<T>::reserve(size_t cap) {
    if (capacity_ < cap) {
        reBuild(cap);
    }
}

template<typename T>
void vector<T>::swap(vector &w) noexcept {
    using std::swap;
    swap(size_, w.size_);
    swap(data_, w.data_);
    swap(capacity_, w.capacity_);
}
template<typename T>
typename vector<T>::iterator vector<T>::end() noexcept {
    return data_ + size_;
}

template<typename T>
typename vector<T>::const_iterator vector<T>::end() const noexcept {
    return data_ + size_;
}

template<typename T>
typename vector<T>::iterator vector<T>::begin() noexcept {
    return data_;
}

template<typename T>
typename vector<T>::const_iterator vector<T>::begin() const noexcept {
    return data_;
}

template<typename T>
void vector<T>::extend() {
    assert(size_ == capacity_);
    if (capacity_ == 0) {
        reBuild(1);
    } else {
        reBuild(2 * capacity_);
    }
}

template<typename T>
typename vector<T>::iterator vector<T>::insert(vector::const_iterator idx, const T &v) {
    ptrdiff_t del = idx - begin();
    push_back(v);
    for (iterator it = begin() + del; it != end(); it++) {
        std::swap(back(), *it);
    }
    return begin() + del;
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator f, vector::const_iterator e) {
    if (e < f) {
        return data_;
    }
    ptrdiff_t cnt = e - f;
    for (auto i = e - data_; i < size_; ++i) {
        std::swap(data_[i - cnt], data_[i]);
    }
    while (cnt--) {
        pop_back();
    }
    return begin() + (f - begin());
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(vector::const_iterator idx) {
    return erase(idx, idx + 1);
}

