#ifndef MY_BUFFER_H
#define MY_BUFFER_H

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <algorithm>
#include<vector>
struct my_buffer {
private:
    struct big_buffer {
        explicit big_buffer(size_t);
        big_buffer();

        big_buffer& operator=(big_buffer const &);
        ~big_buffer();

        std::shared_ptr<std::vector<uint32_t> > data;
    };
    static constexpr size_t MAX_SIZE = sizeof(big_buffer) / sizeof(uint32_t);

    union {
        big_buffer big_b;
        uint32_t small_b[MAX_SIZE];
    };

    size_t size_b;
    uint32_t *data_b;
    bool is_big() const;
    size_t get_capacity() const;
    void create_unique();

public:
    my_buffer();
    uint32_t back() const;
    uint32_t& back();
    my_buffer(my_buffer const &);

    ~my_buffer();

    my_buffer &operator=(my_buffer const &other);

    void resize(size_t sz);
    
    void pop_back();

    void push_back(uint32_t x);
    
    uint32_t &operator[](size_t index);
    uint32_t operator[] (size_t index) const;

    size_t size() const;
};

#endif
