#include <cstring>
#include "my_buffer.h"
#include <new>
static const uint32_t UINT_SIZE = sizeof(uint32_t);

my_buffer::big_buffer::big_buffer(size_t cap) :
        data(new std::vector<uint32_t>(cap), std::default_delete<std::vector<uint32_t>>()) {
        std::fill(data->data(), data->data() + cap, 0);
}

my_buffer::big_buffer::big_buffer() : data() {}

my_buffer::big_buffer &my_buffer::big_buffer::operator=(const my_buffer::big_buffer &buf) = default;

my_buffer::big_buffer::~big_buffer() = default;

my_buffer::my_buffer() : size_b(0), data_b(small_b) {
    std::fill(small_b, small_b + MAX_SIZE, 0);
}

size_t my_buffer::get_capacity() const {
    return is_big() ? big_b.data->size() : MAX_SIZE;
}

my_buffer::~my_buffer() {
    if (is_big()) {
        big_b.~big_buffer();
    }
}

my_buffer::my_buffer(my_buffer const &buf) : my_buffer() {
    if (buf.is_big()) {
        big_b = buf.big_b;
        data_b = big_b.data->data();
    } else {
        std::copy(buf.small_b, buf.small_b + MAX_SIZE, data_b);
        data_b = small_b;
    }
    size_b = buf.size_b;
}

void my_buffer::create_unique() {
    if (!is_big() || big_b.data.unique()) {
        return;
    }
    big_buffer tmp(big_b.data->capacity());
    std::copy(big_b.data->data(), big_b.data->data() + big_b.data->capacity(), tmp.data->data());
    big_b = tmp;
    data_b = big_b.data->data();

}

my_buffer &my_buffer::operator=(my_buffer const &buf) {
    if (buf.is_big()) {
        if (!is_big()) {
            auto* ptr = new (small_b) big_buffer;
            big_b = *ptr;
        }
        big_b = buf.big_b;
        data_b = big_b.data->data();
    } else {
        if (is_big()) {
            big_b.~big_buffer();
        }
        data_b = small_b;
        std::copy(buf.small_b, buf.small_b + MAX_SIZE, small_b);
        }
    size_b = buf.size_b;
    return *this;
}

void my_buffer::resize(size_t sz) {
    create_unique();
    size_b = sz;
    if (size_b <= get_capacity()) {
        return;
    }
    big_buffer nv(sz);
    std::copy(data_b, data_b + get_capacity(), nv.data->data());
    if (!is_big()) {
        auto* ptr = new (small_b) big_buffer;
        big_b = *ptr;
    }
    big_b = nv;
    data_b = big_b.data->data();
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
    size_b--;
}

void my_buffer::push_back(uint32_t x) {
    create_unique();
    size_t cap = get_capacity();
    if (size_b + 1 > cap) {
        big_buffer tmp(2 * cap);
        std::copy(data_b, data_b + cap, tmp.data->data());
        if (!is_big()) {
            auto* ptr = new (small_b) big_buffer;
            big_b = *ptr;
        }
        big_b = tmp;
        data_b = big_b.data->data();
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
