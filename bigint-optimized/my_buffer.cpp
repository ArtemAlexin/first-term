#include <cstring>
#include "my_buffer.h"

static const uint32_t UINT_SIZE = sizeof(uint32_t);

my_buffer::big_buffer::big_buffer(size_t cap) :
        capacity(cap),
        data(new uint32_t[cap], std::default_delete<uint32_t[]>()) {
    memset(data.get(), 0, cap * UINT_SIZE);
}

my_buffer::big_buffer::big_buffer() : capacity(0), data() {}

my_buffer::big_buffer &my_buffer::big_buffer::operator=(const my_buffer::big_buffer &buf) = default;

my_buffer::big_buffer::~big_buffer() = default;

my_buffer::my_buffer() : _big(), _size(0), is_big(false), _data(_small) {}

size_t my_buffer::get_size() {
    return is_big ? _big.capacity : MAX_SIZE;
}

my_buffer::~my_buffer() {
    if (is_big) {
        _big.~big_buffer();
    }
}

my_buffer::my_buffer(my_buffer const &buf) : my_buffer() {
    if (buf.is_big) {
        _big = buf._big;
        _data = _big.data.get();
        is_big = true;
    } else {
        memcpy(_data, buf._small, MAX_SIZE * UINT_SIZE);
        _data = _small;
        is_big = false;
    }
    _size = buf._size;
}

void my_buffer::create_unique() {
    if (!is_big || _big.data.unique()) {
        return;
    }
    big_buffer tmp(_big.capacity);
    memcpy(tmp.data.get(), _big.data.get(), _big.capacity * UINT_SIZE);
    _big = tmp;
    _data = _big.data.get();

}

my_buffer &my_buffer::operator=(my_buffer const &buf) {
    if (buf.is_big) {
        if (!is_big) {
            memset(_small, 0, UINT_SIZE * MAX_SIZE);
        }
        _big = buf._big;
        _data = _big.data.get();
    } else {
        if (is_big) {
            _big.data = nullptr;
        }
        _data = _small;
        memcpy(_small, buf._small, MAX_SIZE * UINT_SIZE);
    }
    is_big = buf.is_big;
    _size = buf._size;
    return *this;
}

void my_buffer::resize(size_t sz) {
    create_unique();
    _size = sz;
    if (_size <= get_size()) {
        return;
    }
    big_buffer new_vect(sz);
    memcpy(new_vect.data.get(), _data, UINT_SIZE * get_size());
    if (!is_big) {
        is_big = true;
        memset(_small, 0, UINT_SIZE * MAX_SIZE);
    }
    _big = new_vect;
    _data = _big.data.get();
}


uint32_t &my_buffer::operator[](size_t index) const {
    return _data[index];
}

void my_buffer::pop_back() {
    create_unique();
    _size--;
}

void my_buffer::push_back(uint32_t x) {
    create_unique();
    size_t cap = get_size();
    if (_size + 1 > cap) {
        big_buffer tmp(2 * cap);
        memcpy(tmp.data.get(), _data, UINT_SIZE * cap);
        if (!is_big) {
            is_big = true;
            memset(_small, 0, UINT_SIZE * MAX_SIZE);
        }
        _big = tmp;
        _data = _big.data.get();
    }
    _size++;
    _data[_size - 1] = x;
}

size_t my_buffer::size() const {
    return _size;
}

my_buffer &my_buffer::change(size_t id, uint32_t x) {
    create_unique();
    _data[id] = x;
    return *this;
}

uint32_t &my_buffer::back() {
    return _data[_size - 1];
}

uint32_t my_buffer::back() const {
    return _data[_size - 1];
}