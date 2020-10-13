#include <cstring>
#include "my_buffer.h"
#include <new>
#include <iostream>

my_buffer::my_buffer() : size_b(0), data_b(small_b) {}
void my_buffer::clear() {
    if(is_big()) {
        big_b.~shared_ptr();
    }
}

my_buffer::~my_buffer() {
   clear();
}

my_buffer::my_buffer(my_buffer const &buf) : my_buffer() {
    if (buf.is_big()) {
        alloc_sm(buf);
    } else {
        std::copy(buf.small_b, buf.small_b + buf.size_b, data_b);
        data_b = small_b;
    }
    size_b = buf.size_b;
}

void my_buffer::create_unique() {
    if (!is_big() || big_b.unique()) {
        return;
    }
    auto tmp = new std::vector<uint32_t>(data_b, data_b + size_b);
    big_b.reset(tmp);
    data_b = big_b->data();
}

my_buffer &my_buffer::operator=(my_buffer const &buf) {
    if(buf.is_big()) {
        alloc_sm(buf);
    } else {
        clear();
        std::copy(buf.small_b, buf.small_b + buf.size_b, small_b);
        data_b = small_b;
    }
    size_b = buf.size_b;
    return *this;
}

void my_buffer::resize(size_t sz) {
    create_unique();
    size_t last_size = size_b;
    if(is_big()) {
        big_b->resize(sz);
        data_b = big_b->data();
        size_b = sz;
        return;
    }
    if(sz > MAX_SIZE) {
        auto tmp = new std::vector<uint32_t>(data_b, data_b + last_size);
        tmp->resize(sz);
        alloc_pl(tmp);
        size_b = sz;
        return;
    }
    if(sz > last_size) {
        std::fill(data_b + last_size, data_b + sz, 0);
    }
    size_b = sz;
}

uint32_t &my_buffer::operator[] (size_t index) {
    create_unique();
    return data_b[index];
}
uint32_t my_buffer::operator[] (size_t index) const {
    return data_b[index];
}

void my_buffer::pop_back() {
    create_unique();
    if(is_big()) {
        big_b->pop_back();
    }
    size_b--;
}

void my_buffer::push_back(uint32_t x) {
    create_unique();
    if(is_big()) {
        big_b->push_back(x);
        data_b = big_b->data();
        size_b++;
        return;
    }
    if(size_b == MAX_SIZE) {
        auto tmp = new std::vector<uint32_t>(data_b, data_b + MAX_SIZE);
        tmp->push_back(x);
        alloc_pl(tmp);
        size_b++;
        return;
    }
    size_b++;
    data_b[size_b - 1] = x;
}

size_t my_buffer::size() const {
    return size_b;
}

uint32_t my_buffer::back() const {
    return data_b[size_b - 1];
}
uint32_t& my_buffer::back() {
    create_unique();
    return data_b[size_b - 1];
}

bool my_buffer::is_big() const {
    return data_b != small_b;
}
void my_buffer::alloc_pl(std::vector<uint32_t> *tmp) {
    new (&big_b) std::shared_ptr<std::vector<uint32_t>>(tmp);
    data_b = big_b->data();
}
void my_buffer::alloc_sm(my_buffer const &buf) {
    if(!is_big()) {
        new (&big_b) std::shared_ptr<std::vector<uint32_t>>(buf.big_b);
    } else {
        big_b = buf.big_b;
    }
    data_b = big_b->data();
}
