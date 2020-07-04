#ifndef MY_BUFFER_H
#define MY_BUFFER_H

#include <cstdint>
#include <cstdlib>
#include <memory>

struct my_buffer {
private:
    struct big_buffer {
        explicit big_buffer(size_t);
        big_buffer();

        big_buffer& operator=(big_buffer const &);
        ~big_buffer();

        size_t capacity;
        std::shared_ptr<uint32_t> data;
    };
    static const size_t MAX_SIZE = sizeof(big_buffer) / sizeof(uint32_t);

    union {
        big_buffer _big;
        uint32_t _small[MAX_SIZE];
    };

    size_t _size;
    bool is_big;
    uint32_t *_data;

    size_t get_size();

    void create_unique();

public:
    my_buffer();
    uint32_t& back();
    uint32_t back() const;
    my_buffer(my_buffer const &);

    ~my_buffer();

    my_buffer &operator=(my_buffer const &other);

    void resize(size_t sz);
    
    void pop_back();

    void push_back(uint32_t x);
    
    uint32_t &operator[](size_t index) const;

    size_t size() const;

    my_buffer & change(size_t index, uint32_t val);
};

#endif
