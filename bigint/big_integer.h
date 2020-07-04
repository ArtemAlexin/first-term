#pragma once

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <vector>
#include <string>
#include<functional>

struct big_integer {
    big_integer();

    big_integer(big_integer const &);

    big_integer(uint64_t);

    big_integer(uint32_t);

    big_integer(int32_t);

    explicit big_integer(std::string const &);

    ~big_integer();

    big_integer &operator=(big_integer const &);

    big_integer &operator+=(big_integer const &);

    big_integer &operator-=(big_integer const &);

    big_integer &operator*=(big_integer const &);

    big_integer &operator/=(big_integer const &);

    big_integer &operator%=(big_integer const &);

    big_integer &operator&=(big_integer const &);

    big_integer &operator|=(big_integer const &);

    big_integer &operator^=(big_integer const &);

    big_integer &operator<<=(int32_t);

    big_integer &operator>>=(int32_t);

    big_integer operator+() const;

    big_integer operator-() const;

    big_integer operator~() const;

    big_integer &operator++();

    big_integer operator++(int32_t);

    big_integer &operator--();

    big_integer operator--(int32_t);

    friend bool operator==(big_integer const &, big_integer const &);

    friend bool operator!=(big_integer const &, big_integer const &);

    friend bool operator<(big_integer const &, big_integer const &);

    friend bool operator>(big_integer const &, big_integer const &);

    friend bool operator<=(big_integer const &, big_integer const &);

    friend bool operator>=(big_integer const &, big_integer const &);

    friend big_integer operator+(big_integer const &, big_integer const &);

    friend big_integer operator-(big_integer const &, big_integer const &);

    friend big_integer operator*(big_integer const &, big_integer const &);

    friend big_integer operator/(big_integer, big_integer const &);

    friend big_integer operator%(big_integer const &, big_integer const &);

    friend big_integer operator&(big_integer const &, big_integer const &);

    friend big_integer operator|(big_integer const &, big_integer const &);

    friend big_integer operator^(big_integer const &, big_integer const &);

    friend big_integer operator<<(const big_integer &, int32_t);

    friend big_integer operator>>(const big_integer &, int32_t);

    friend std::string to_string(big_integer const &);

private:
    size_t arr_size() const;

    uint32_t operator[](size_t) const;
    uint32_t& operator[](size_t);

    bool less(const big_integer &, const std::function<bool(uint32_t, uint32_t)> &) const;

    big_integer(int32_t, size_t);

    big_integer(int32_t, size_t, size_t);

    void to_normal_form();

    void update_sign(bool);

    big_integer inv() const;

    big_integer to_unsigned_comp() const;

    big_integer addingCode() const;

    uint32_t div_long_short(uint32_t x);

    static big_integer bit_operation(big_integer const &, big_integer const &,
                                    const std::function<uint32_t(uint32_t, uint32_t)> &,
                                    const std::function<int32_t(int32_t, int32_t)> &);


    bool isZero() const;

    uint32_t trial(uint64_t, uint64_t, big_integer const &) const;

    bool smaller(big_integer const &, uint64_t, uint64_t) const;

    void difference(big_integer const &, uint64_t, uint64_t);

    //sign = 1 num >=0, sign = -1 num < 0
    int32_t sign;
    std::vector<uint32_t> value;
};
