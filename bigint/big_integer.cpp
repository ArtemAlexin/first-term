#include "big_integer.h"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <functional>
static const uint64_t BLOCK_SIZE = static_cast<uint64_t>(UINT32_MAX) + 1;
static const uint32_t BITS_NUM = 32;
static int32_t getSign(bool val) {
    return val * 2 - 1;
}
static uint32_t and_(uint32_t f, uint32_t s) {
    return f & s;
}
static uint32_t xor_(uint32_t f, uint32_t s) {
    return f ^ s;
}
static uint32_t or_(uint32_t f, uint32_t s) {
    return f | s;
}
static int32_t andSign(int32_t f, int32_t s) {
    return getSign(f > 0 || s > 0);
}
static uint32_t xorSign(int32_t f, int32_t s) {
    return getSign(f > 0 && s > 0);
}
static uint32_t orSign(int32_t f, int32_t s) {
    return getSign((f > 0 || s < 0) && (f < 0 || s > 0));
}

void big_integer::updateSign(bool cond) {
    sign = getSign(cond);
}

big_integer::big_integer() {
    value.push_back(0);
    sign = 1;
}
big_integer::big_integer(uint64_t num) {
    sign = 1;
    value.push_back(static_cast<uint32_t>(num & UINT32_MAX));
    value.push_back(static_cast<uint32_t> (num >> 32));
    shrink_to_fit();
}
big_integer::big_integer(uint32_t num) {
    sign = 1;
    value.push_back(num);
}
big_integer::big_integer(big_integer const &other) = default;

big_integer::big_integer(int num) {

    if (num == INT32_MIN) {
        value.push_back((uint32_t)(INT32_MAX) + 1);
    } else {
        value.push_back(static_cast<uint32_t>(abs(num)));
    }
    updateSign(num >= 0);
}

big_integer::big_integer(std::string const &s) {
    big_integer shift = 1;
    value.push_back(0);
    for (int32_t i = static_cast<int32_t>(s.length() - 1); i >= 0; i--) {
        if (s[i] == '-') {
            break;
        }
        *this += (shift * static_cast<uint32_t>(s[i] - '0'));
        shift *= 10;
    }
    updateSign(s[0] != '-');
    shrink_to_fit();
}

big_integer::~big_integer() {}

void big_integer::shrink_to_fit() {
    while (value.size() > 1 && value.back() == 0)
        value.pop_back();
}

big_integer big_integer::inv() const {
    if (sign == 1) {
        return *this;
    }
    big_integer res(*this);
    for (size_t i = 0; i < res.value.size(); i++) {
        res.value[i] = ~res.value[i];
    }
    return res;
}

big_integer big_integer::get_adding_code() const {
    big_integer res = inv();
    if (res.sign == 1) {
        return res;
    }
    res.value.push_back(UINT32_MAX);
    res -= 1;
    return res;
}

big_integer big_integer::complement_to_unsigned() const {
    big_integer res = inv();
    if (res.sign == 1) {
        return res;
    }
    res -= 1;
    res.shrink_to_fit();
    return res;
}

bool big_integer::is_zero() const {
    return (value.size() == 1 && value[0] == 0);
}

big_integer &big_integer::operator=(big_integer const &num) = default;

big_integer &big_integer::operator+=(big_integer const &rhs) {
    return (*this = *this + rhs);
}

big_integer &big_integer::operator-=(big_integer const &rhs) {
    return (*this = *this - rhs);
}

big_integer &big_integer::operator*=(big_integer const &rhs) {
    return (*this = *this * rhs);
}

big_integer &big_integer::operator/=(big_integer const &rhs) {
    return (*this = *this / rhs);
}

big_integer &big_integer::operator%=(big_integer const &rhs) {
    return (*this = *this % rhs);
}

big_integer &big_integer::operator&=(big_integer const &rhs) {
    return (*this = *this & rhs);
}

big_integer &big_integer::operator|=(big_integer const &rhs) {
    return (*this = *this | rhs);
}

big_integer &big_integer::operator^=(big_integer const &rhs) {
    return (*this = *this ^ rhs);
}

big_integer &big_integer::operator<<=(int rhs) {
    return (*this = *this << rhs);
}

big_integer &big_integer::operator>>=(int rhs) {
    return (*this = *this >> rhs);
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator-() const {
    big_integer res(*this);
    int32_t ns = getSign(res.is_zero());
    res.sign *= ns;
    return res;
}

big_integer big_integer::operator~() const {
    big_integer res(*this);
    return -(res + 1);
}

big_integer &big_integer::operator++() {
    return *this = *this + 1;
}

big_integer big_integer::operator++(int) {
    big_integer res(*this);
    *this = *this + 1;
    return res;
}

big_integer &big_integer::operator--() {
    return *this = *this - 1;
}

big_integer big_integer::operator--(int) {
    big_integer res = *this;
    *this = *this - 1;
    return res;
}
//algorithms for /, +, -, * are taken from neerc adn some other sites and books.

big_integer operator+(big_integer const &f, big_integer const &s) {
    if (f.sign < 0) {
        return s - (-f);
    } else if (s.sign < 0) {
        return f - (-s);
    }
    big_integer res(f);
    uint64_t shift = 0;
    for (size_t i = 0; i < std::max(res.value.size(), s.value.size()) || shift; i++) {
        if (i == res.value.size()) {
            res.value.push_back(0);
        }
        uint64_t add = res.value[i] + shift + (i < s.value.size() ? s.value[i] : 0);
        shift = add >= BLOCK_SIZE;
        if (shift) {
            add -= BLOCK_SIZE;
        }
        res.value[i] = static_cast<uint32_t>(add);
    }
    return res;
}

big_integer operator-(big_integer const &f, big_integer const &s) {
    if (f.sign < 0) {
        return -((-f) + s);
    } else if (s.sign < 0) {
        return f + (-s);
    }
    big_integer g = f, l = s;
    int32_t rs = getSign(g >= l);
    if (rs == -1) {
        std::swap(g, l);
    }
    int64_t shift = 0;
    for (size_t i = 0; i < l.value.size() || shift; i++) {
        int64_t sub = g.value[i] - (shift + (i < l.value.size() ? l.value[i] : 0));
        shift = sub < 0;
        if (shift) {
            sub += BLOCK_SIZE;
        }
        g.value[i] = static_cast<uint32_t>(sub);
    }
    g.shrink_to_fit();
    g.sign = rs;
    return g;
}

std::pair<big_integer, uint32_t> big_integer::div_long_short(uint32_t nu) const {
    if (nu == 0) {
        throw std::runtime_error("Division by zero");
    }
    uint64_t shift = 0;
    big_integer res(*this);
    for (int32_t i = static_cast<int32_t>(value.size()) - 1; i >= 0; i--) {
        uint64_t cur = value[i] + shift * BLOCK_SIZE;
        res.value[i] = static_cast<uint32_t>(cur / nu);
        shift = cur % nu;
    }
    res.shrink_to_fit();
    return {res, shift};
}

big_integer operator*(big_integer const &f, big_integer const &s) {
    big_integer res;
    res.value.resize(f.value.size() + s.value.size());
    uint64_t carry = 0;
    res.sign = f.sign * s.sign;
    for (uint32_t i = 0; i < f.value.size(); i++) {
        for (uint32_t j = 0; j < s.value.size() || carry; j++) {
            __uint128_t mul_res = res.value[i + j] + static_cast<__uint128_t> (f.value[i])
                                                     * (j < s.value.size() ? s.value[j] : 0) + carry;
            res.value[i + j] = static_cast<uint32_t>(mul_res % BLOCK_SIZE);
            carry = static_cast<uint32_t>(mul_res / BLOCK_SIZE);
        }
    }
    res.shrink_to_fit();
    if (res.is_zero()) {
        res.sign = 1;
    }
    return res;
}

//these tree functions are only used in big_integer division
bool big_integer::smaller(big_integer const &dq, uint64_t k, uint64_t m) const {
    //TODO
    uint64_t i = m, j = 0;
    while (i != j) {
        if (value[i + k] == dq.value[i]) {
            i--;
            continue;
        }
        break;
    }
    return value[i + k] < dq.value[i];
}

uint32_t big_integer::trial(uint64_t l, uint64_t r, uint64_t const num) const {
    auto BASE = static_cast<__uint128_t>(BLOCK_SIZE);
    uint64_t t = l + r;
    __uint128_t res = (static_cast<__uint128_t>(value[t]) * BASE + value[t - 1]) * BASE + value[t - 2];
    return uint32_t(std::min(res / static_cast<__uint128_t>(num), static_cast<__uint128_t>(UINT32_MAX)));
}

void big_integer::difference(big_integer const &num, uint64_t l, uint64_t r) {
    int64_t taken = 0, BASE = static_cast<int64_t>(BLOCK_SIZE), d;
    for (uint64_t i = 0; i <= r; i++) {
        d = static_cast<int64_t>(value[i + l]) - num.value[i] - taken + BASE;
        value[i + l] = static_cast<uint32_t>(d % BASE);
        taken = 1 - d / BASE;
    }
}

big_integer operator/(big_integer const &a, big_integer const &b) {
    if (b.is_zero()) {
        throw std::runtime_error("Division by zero");
    } else if (a.value.size() < b.value.size()) {
        big_integer res;
        return res;
    } else if (b.value.size() == 1) {
        return b.sign > 0 ? a.div_long_short(b.value[0]).first : -(a.div_long_short(b.value[0]).first);
    }
    int32_t res_sign = a.sign * b.sign;
    auto n = a.value.size(), m = b.value.size();
    auto f = (static_cast<uint64_t>(1) << 32) / (uint64_t(b.value[m - 1]) + 1);
    big_integer r = a * f;
    big_integer d = b * f;
    r.sign = 1;
    d.sign = 1;
    big_integer q;
    q.value.resize(n - m + 1);
    r.value.push_back(0);
    const uint64_t d2 = (uint64_t(d.value[m - 1]) << 32) + uint64_t(d.value[m - 2]);
    for (auto k = int32_t(n - m); k >= 0; k--) {
        // precondition: t has k + m + 1 digits
        auto qt = r.trial(uint64_t(k), m, d2);
        big_integer qt_mul;
        qt_mul.value[0] = qt;
        big_integer dq = qt_mul * d;
        dq.value.resize(m + 1);
        if (r.smaller(dq, uint64_t(k), m)) {
            qt--;
            dq = d * qt;
        }
        q.value[k] = qt;
        r.difference(dq, uint64_t(k), m);
    }
    q.sign = res_sign;
    q.shrink_to_fit();
    return q;
}


big_integer operator%(big_integer const &f, big_integer const &s) {
    return f - (f / s) * s;
}

big_integer big_integer::logical_op(big_integer const &f,
                                    big_integer const &s,
                                    std::function<uint32_t(uint32_t, uint32_t)> binaryOperation,
                                    std::function<int32_t(int32_t, int32_t)> signResult) const {
    big_integer g = f.get_adding_code(), l = s.get_adding_code();
    uint32_t extraDigit = 0;
    if (g.value.size() < l.value.size()) {
        std::swap(g, l);
    }
    if(l.sign < 0) {
        extraDigit = UINT32_MAX;
    }
    for (size_t i = 0; i < g.value.size(); i++) {
        g.value[i] = binaryOperation(g.value[i], (i >= l.value.size() ? extraDigit : l.value[i]));
    }
    g.sign = signResult(f.sign, s.sign);
    g.shrink_to_fit();
    return g.complement_to_unsigned();
}

big_integer operator&(big_integer const &a, big_integer const &b) {
    return a.logical_op(a, b, and_, andSign);
}

big_integer operator|(big_integer const &a, big_integer const &b) {
    return a.logical_op(a, b, or_, orSign);
}

big_integer operator^(big_integer const &a, big_integer const &b) {
    return a.logical_op(a, b, xor_, xorSign);
}

big_integer operator<<(big_integer num, int32_t shift) {
    uint64_t s = (static_cast<uint64_t>(1) << (shift % 32));
    num *= s;
    for (int i = 0; i < shift/ 32; i++) num.value.push_back(0);
    return num;
}

big_integer operator>>(big_integer a, int b) {
    for (int i = 0; i < b / 32 && !a.value.empty(); i++) a.value.pop_back();
    if (a < 0) a -= (uint64_t) (static_cast<uint64_t>(1) << b % 32) - 1;
    if (b % 32 == 31) a /= 2;
    a /= (static_cast<uint64_t>(1) << std::min(30, b % 32));
    return a;
}

bool operator==(big_integer const &a, big_integer const &b) {
    int l = a.value.size();
    int r = b.value.size();
    if (a.value.size() != b.value.size() || a.sign != b.sign) {
        return 0;
    }
    for (uint32_t i = 0; i < a.value.size(); i++) {
        if (a.value[i] != b.value[i]) {
            return 0;
        }
    }
    return 1;
}

bool operator!=(big_integer const &a, big_integer const &b) {
    return !(a == b);
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign) {
        return a.sign < b.sign;
    } else if (a.value.size() < b.value.size()) {
        return a.sign == 1;
    } else if (a.value.size() > b.value.size()) {
        return a.sign == -1;
    } else if (a.sign == -1) {
        return (-a) > (-b);
    }
    for (int32_t i = static_cast<int32_t>(a.value.size()) - 1; i >= 0; i--) {
        if (a.value[i] < b.value[i]) {
            return true;
        } else if (a.value[i] > b.value[i]) {
            return false;
        }
    }
    return false;
}

bool operator>(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign) {
        return a.sign > b.sign;
    } else if (a.value.size() < b.value.size()) {
        return a.sign == -1;
    } else if (a.value.size() > b.value.size()) {
        return a.sign == 1;
    } else if (a.sign == -1) {
        return (-a) < (-b);
    }
    for (int32_t i = static_cast<int32_t>(a.value.size()) - 1; i >= 0; i--) {
        if (a.value[i] > b.value[i]) {
            return true;
        } else if (a.value[i] < b.value[i]) {
            return false;
        }
    }
    return false;
}

bool operator<=(big_integer const &a, big_integer const &b) { return !(a > b); }

bool operator>=(big_integer const &a, big_integer const &b) { return !(a < b); }

std::string to_string(big_integer const &a) {
    std::string res;
    big_integer cur = a;
    bool is_negative = cur.sign == -1;
    if (cur.is_zero()) {
        return "0";
    }
    while (!cur.is_zero()) {
        auto div_res = cur.div_long_short(10);
        cur = div_res.first;
        res.push_back('0' + char(div_res.second));
    }
    if (is_negative) {
        res.push_back('-');
    }
    std::reverse(res.begin(), res.end());
    return res;
}




