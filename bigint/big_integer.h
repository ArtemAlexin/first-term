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

    friend big_integer operator/(big_integer const &, big_integer const &);

    friend big_integer operator%(big_integer const &, big_integer const &);

    friend big_integer operator&(big_integer const &, big_integer const &);

    friend big_integer operator|(big_integer const &, big_integer const &);

    friend big_integer operator^(big_integer const &, big_integer const &);

    friend big_integer operator<<(big_integer, int32_t);

    friend big_integer operator>>(big_integer const &, int32_t );

    friend std::string to_string(big_integer const &);

   private:
    void shrink_to_fit();
    void updateSign(bool);
    big_integer inv() const;
    big_integer get_adding_code() const;

    big_integer complement_to_unsigned() const;

    std::pair<big_integer, uint32_t> div_long_short(uint32_t x) const;

    big_integer logical_op(big_integer const &, big_integer const &,
                           std::function<uint32_t(uint32_t , uint32_t)>,
                           std::function<int32_t (int32_t , int32_t )>) const;

    bool smaller(big_integer const &, uint64_t, uint64_t) const;

    uint32_t trial(uint64_t, uint64_t, uint64_t const) const;

    void difference(big_integer const &, uint64_t, uint64_t);

    bool is_zero() const;

    //sign = 1 num >=0, sign = -1 num < 0
    int32_t sign;
    std::vector<uint32_t> value;
};

big_integer operator+(big_integer const &, big_integer const &);

big_integer operator-(big_integer const &, big_integer const &);

big_integer operator*(big_integer const &, big_integer const &);

big_integer operator/(big_integer const &, big_integer const &);

big_integer operator%(big_integer const &, big_integer const &);

big_integer operator&(big_integer const &, big_integer const &);

big_integer operator|(big_integer const &, big_integer const &);

big_integer operator^(big_integer const &, big_integer const &);

bool operator==(big_integer const &, big_integer const &);

bool operator!=(big_integer const &, big_integer const &);

bool operator<(big_integer const &, big_integer const &);

bool operator>(big_integer const &, big_integer const &);

bool operator<=(big_integer const &, big_integer const &);

bool operator>=(big_integer const &, big_integer const &);

std::string to_string(big_integer const &);
