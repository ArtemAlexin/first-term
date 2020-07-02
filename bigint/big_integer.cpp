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

static bool less(uint32_t f, uint32_t g) {
    return f < g;
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
    return getSign(f >= 0 || s >= 0);
}

static uint32_t orSign(int32_t f, int32_t s) {
    return getSign(f >= 0 && s >= 0);
}

static uint32_t xorSign(int32_t f, int32_t s) {
    return getSign((f >= 0 || s <= 0) && (f <= 0 || s >= 0));
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
    value.push_back(static_cast<uint32_t>(num & 0x00000000ffffffff));
    value.push_back(static_cast<uint32_t> (num >> 32));
    toNormalForm();
}

big_integer::big_integer(uint32_t num) {
    sign = 1;
    value.push_back(num);
}

big_integer::big_integer(int32_t sign, size_t size) {
    value.resize(size);
    this->sign = sign;
}

big_integer::big_integer(int32_t sign, size_t size, size_t extraReserveSize) {
    value.reserve(size + extraReserveSize);
    value.resize(size);
    this->sign = sign;
}
big_integer::big_integer(big_integer const &other) = default;

big_integer::big_integer(int num) {
    if (num == INT32_MIN) {
        value.push_back(static_cast<uint32_t>(INT32_MAX) + 1);
    } else {
        value.push_back(static_cast<uint32_t>(abs(num)));
    }
    updateSign(num >= 0);
}

big_integer::big_integer(std::string const &s) {
    big_integer shift = 1;
    value.push_back(0);
    this->sign = 1;
    for (auto i = static_cast<int32_t>(s.length() - 1); i >= 0; i--) {
        if (s[i] == '-') {
            break;
        }
        *this += (shift * static_cast<uint32_t>(s[i] - '0'));
        shift *= 10;
    }
    updateSign(s[0] != '-');
    toNormalForm();
}

size_t big_integer::arrSize() const{
    return value.size();
}
uint32_t & big_integer::operator[](size_t id) {
    return value[id];
}
uint32_t big_integer::operator[](size_t id) const{
    return value[id];
}
bool big_integer::isZero() const {
    return (value.size() == 1 && value[0] == 0);
}

big_integer::~big_integer() {}

void big_integer::toNormalForm() {
    while (value.size() > 1 && value.back() == 0)
        value.pop_back();
}

big_integer big_integer::inv() const {
    if (sign == 1) {
        return *this;
    }
    big_integer res(*this);
    for (unsigned int &i : res.value) {
        i = ~i;
    }
    return res;
}

big_integer big_integer::toUnsignedCompl() const {
    big_integer res = inv();
    if (res.sign == 1) {
        return res;
    }
    res -= 1;
    res.toNormalForm();
    return res;
}

big_integer big_integer::addingCode() const {
    big_integer res = inv();
    if (res.sign == 1) {
        return res;
    }
    res.value.push_back(UINT32_MAX);
    res -= 1;
    return res;
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
    int32_t ns = getSign(res.isZero());
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
//algorithms for /, +, -, * are taken from neerc and some other sites and books.

big_integer operator+(big_integer const &f, big_integer const &s) {
    if (f.sign < 0) {
        return s - (-f);
    }
    if (s.sign < 0) {
        return f - (-s);
    }
    big_integer res(f);
    bool shift = false;
    for (size_t i = 0; i < std::max(res.arrSize(), s.arrSize()) || shift; i++) {
        if (i == res.arrSize()) {
            res.value.push_back(0);
        }
        uint64_t add = res[i] + static_cast<uint64_t>(shift) + (i < s.arrSize() ? s[i] : 0);
        shift = add >= BLOCK_SIZE;
        if (shift) {
            add -= BLOCK_SIZE;
        }
        res[i] = static_cast<uint32_t>(add);
    }
    return res;
}

big_integer operator-(big_integer const &f, big_integer const &s) {
    if (f.sign < 0) {
        return -((-f) + s);
    }
    if (s.sign < 0) {
        return f + (-s);
    }
    big_integer g(f), l(s);
    int32_t rs = getSign(g >= l);
    if (rs == -1) {
        std::swap(g, l);
    }
    int64_t shift = 0;
    for (size_t i = 0; i < l.arrSize() || shift; i++) {
        int64_t sub = g[i] - (shift + (i < l.arrSize() ? l[i] : 0));
        shift = sub < 0;
        if (shift) {
            sub += BLOCK_SIZE;
        }
        g[i] = static_cast<uint32_t>(sub);
    }
    g.toNormalForm();
    g.sign = rs;
    return g;
}

big_integer operator*(big_integer const &f, big_integer const &s) {
    if (f.isZero() || s.isZero()) {
        return big_integer();
    }
    big_integer res(f.sign * s.sign, f.arrSize() + s.arrSize());
    uint64_t carry = 0;
    for (uint32_t i = 0; i < f.arrSize(); i++) {
        for (uint32_t j = 0; j < s.arrSize() || carry; j++) {
            __uint128_t mul_res =
                    res[i + j] + static_cast<uint64_t>(f[i]) * (j < s.arrSize() ? s[j] : 0) +
                    carry;
            res[i + j] = static_cast<uint32_t>(mul_res % BLOCK_SIZE);
            carry = static_cast<uint32_t>(mul_res / BLOCK_SIZE);
        }
    }
    res.toNormalForm();
    return res;
}

//division
uint32_t big_integer::divLongShort(uint32_t x) {
    if (x == 0) {
        throw std::runtime_error("division by zero");
    }
    if (isZero()) {
        return 0;
    }
    uint32_t shift = 0;
    for (size_t i = value.size(); i > 0; --i) {
        uint64_t tmp = (static_cast<uint64_t>(shift) << 32u) + value[i - 1];
        value[i - 1] = tmp / x;
        shift = tmp % x;
    }
    toNormalForm();
    return shift;
}

bool big_integer::smaller(big_integer const &dq, uint64_t k, uint64_t m) const {
    uint64_t i = m, j = 0;
    while (i != j) {
        if (value[i + k] != dq[i]) {
            j = i;
        } else {
            i--;
        }
    }
    return value[i + k] < dq[i];
}

uint32_t big_integer::trial(uint64_t const k, uint64_t const m, big_integer const &d) const {
    auto BASE = static_cast<__uint128_t>(BLOCK_SIZE);
    __uint128_t r3 = (static_cast<__uint128_t>(value[k + m]) * BASE + value[k + m - 1]) * BASE + value[k + m - 2];
    uint64_t d2 = (static_cast<uint64_t>(d[m - 1]) << 32u) + d[m - 2];
    return static_cast<uint32_t>(std::min(r3 / d2, BASE - 1));
}

void big_integer::difference(big_integer const &dq, uint64_t k, uint64_t m) {
    int64_t borrow = 0, diff;
    auto BASE = static_cast<int64_t>(BLOCK_SIZE);
    for (uint64_t i = 0; i < m + 1; i++) {
        diff = int64_t(value[i + k]) - int64_t(dq[i]) - borrow + BASE;
        value[i + k] = uint32_t(diff % BASE);
        borrow = 1 - diff / BASE;
    }
}

big_integer operator/(big_integer a, big_integer const &b) {
    if (b.isZero()) {
        throw std::runtime_error("Division by zero");
    } else if (a.arrSize() < b.arrSize()) {
        big_integer res;
        return res;
    } else if (b.arrSize() == 1) {
        a.divLongShort(b[0]);
        if (b.sign == -1) {
            a = -a;
        }
        return a;
    }
    size_t n = a.arrSize(), m = b.arrSize();
    uint64_t f = (BLOCK_SIZE + 1) / (static_cast<uint64_t>(b[m - 1]) + 1);
    big_integer q, r = a * f, d = b * f;;
    q.value.resize(n - m + 1);
    q.sign = a.sign * b.sign, r.sign = 1, d.sign = 1;
    r.value.push_back(0);
    for (ptrdiff_t k = n - m; k >= 0; --k) {
        uint32_t qt = r.trial(static_cast<uint64_t>(k), m, d);
        big_integer qt_mul = big_integer(static_cast<uint64_t>(qt));
        big_integer dq = qt_mul * d;
        dq.value.resize(m + 1);
        if (r.smaller(dq, static_cast<uint64_t>(k), m)) {
            qt--;
            dq = big_integer(d) * qt;
        }
        q[k] = qt;
        r.difference(dq, static_cast<uint64_t>(k), m);
    }
    q.toNormalForm();
    return q;
}
//end of division

big_integer operator%(big_integer const &f, big_integer const &s) {
    return f - (f / s) * s;
}

big_integer big_integer::bitOperation(big_integer const &f,
                                    big_integer const &s,
                                    const std::function<uint32_t(uint32_t, uint32_t)> &binaryOperation,
                                    const std::function<int32_t(int32_t, int32_t)> &signResult) {
    big_integer g = f.addingCode(), l = s.addingCode();
    uint32_t extraDigit = 0;
    if (g.arrSize() < l.arrSize()) {
        std::swap(g, l);
    }
    if (l.sign < 0) {
        extraDigit = UINT32_MAX;
    }
    for (size_t i = 0; i < g.arrSize(); i++) {
        g[i] = binaryOperation(g[i], (i >= l.arrSize() ? extraDigit : l[i]));
    }
    g.sign = signResult(f.sign, s.sign);
    g.toNormalForm();
    return g.toUnsignedCompl();
}

big_integer operator&(big_integer const &f, big_integer const &s) {
    return big_integer::bitOperation(f, s, and_, andSign);
}

big_integer operator|(big_integer const &f, big_integer const &s) {
    return big_integer::bitOperation(f, s, or_, orSign);
}

big_integer operator^(big_integer const &f, big_integer const &s) {
    return big_integer::bitOperation(f, s, xor_, xorSign);
}

std::string to_string(big_integer const &f) {
    big_integer num(f);
    std::string str;
    if (num.isZero()) {
        return "0";
    }
    while (!num.isZero()) {
        uint32_t c = num.divLongShort(10);
        str.push_back('0' + char(c));
    }
    if (num.sign == -1) {
        str.push_back('-');
    }
    std::reverse(str.begin(), str.end());
    return str;
}


big_integer operator<<(const big_integer &f, int32_t s) {
    if (s == 0) {
        return big_integer(f);
    }
    uint32_t shift = static_cast<uint32_t>(s) % BITS_NUM, r = 0, digits = static_cast<uint32_t>(s) / BITS_NUM;
    big_integer res(f.sign, digits, f.arrSize());
    for (auto i : f.value) {
        res.value.push_back(i);
    }
    for (size_t i = digits; i < res.arrSize(); i++) {
        uint32_t tmp = (res[i] << shift);
        tmp += r;
        r = (res[i] >> (BITS_NUM - shift));
        res[i] = tmp;
    }
    res.value.push_back(r);
    res.toNormalForm();
    return res;
}

big_integer operator>>(const big_integer &f, int s) {
    big_integer cur(f);
    if (s == 0) {
        return cur;
    }
    uint32_t shift = static_cast<uint32_t>(s) % BITS_NUM, digits = static_cast<uint32_t>(s) / BITS_NUM;
    if (cur.sign == -1) {
        cur = cur.addingCode();
        cur.value.reserve(cur.arrSize() + digits);
        for (int32_t i = 0; i < int32_t(digits) - 1; i++) {
            cur.value.push_back(UINT32_MAX);
        }
    }
    if (digits >= cur.arrSize()) {
        return big_integer();
    }
    big_integer res(cur.sign, 0, cur.arrSize());
    for (size_t i = digits; i < cur.arrSize(); i++) {
        res.value.push_back(cur[i]);
    }
    for (size_t i = 0; i < res.arrSize() - 1; i++) {
        res[i] >>= shift;
        uint32_t t = res[i + 1] << (BITS_NUM - shift);
        res[i] += t;
    }
    res.value.back() >>= shift;
    res.toNormalForm();
    if (res.sign < 0) {
        res.value.pop_back();
    }
    res = res.toUnsignedCompl();
    return res;
}

bool operator==(big_integer const &f, big_integer const &s) {
    return f.sign == s.sign && f.value == s.value;
}

bool operator!=(big_integer const &f, big_integer const &s) {
    return !(f == s);
}

bool big_integer::less(const big_integer &s, const std::function<bool(uint32_t, uint32_t)> &fu) const {
    if (value.size() != s.arrSize()) {
        return value.size() < s.arrSize();
    }
    for (int32_t i = static_cast<int32_t>(value.size()) - 1; i >= 0; i--) {
        if (value[i] == s[i]) {
            continue;
        }
        return fu(value[i], s[i]);
    }
    return false;
}

bool operator<(big_integer const &a, big_integer const &b) {
    if (a.sign != b.sign) {
        return a.sign < b.sign;
    }
    if (a.sign == 1) {
        return a.less(b, less);
    }
    return b.less(a, less);
}

bool operator>(big_integer const &a, big_integer const &b) {
    return b < a;
}

bool operator<=(big_integer const &a, big_integer const &b) {
    return !(a > b);
}

bool operator>=(big_integer const &a, big_integer const &b) {
    return !(a < b);
}



