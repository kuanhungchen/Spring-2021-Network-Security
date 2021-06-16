#include <iostream>

using namespace std;

#define N 5000

const char *str_p = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFF";
const char *str_a = "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFC";
const char *str_b = "1C97BEFC54BD7A8B65ACF89F81D4D4ADC565FA45";
const char *str_gx = "4A96B5688EF573284664698968C38BB913CBFC82";
const char *str_gy = "23A628553168947D59DCC912042351377AC5FB32";
const char *str_n = "0100000000000000000001F4C8F927AED3CA752257";

int HEX2DEC(const char c) {
  int d;
  if (c >= '0' && c <= '9') d = (int)(c - 48);
  else d = (int)(c - 65 + 10);
  return d;
}

char DEC2HEX(const int d) {
  char c;
  if ((int)d < 10) c = (char)(d + 48);
  else c = (char)(d + 65 - 10);
  return c;
}

int max(const int a, const int b) {
  if (a > b) return a;
  return b;
}

int min(const int a, const int b) {
  if (a < b) return a;
  return b;
}

char add(const char a, const char b, const char c, char &carry) {
  /* single char addition */
  int r = HEX2DEC(a) + HEX2DEC(b) + HEX2DEC(c);
  char sum;
  if (r >= 16) {
    sum = DEC2HEX(r - 16);
    carry = '1';
  } else {
    sum = DEC2HEX(r);
    carry = '0';
  }
  return sum;
}

char sub(const char a, const char b, const char c, char &borrow) {
  /* single char subtraction */
  int r = HEX2DEC(a) - HEX2DEC(b) - HEX2DEC(c);
  char sum;
  if (r < 0) {
    sum = DEC2HEX(r + 16);
    borrow = '1';
  } else {
    sum = DEC2HEX(r);
    borrow = '0';
  }
  return sum;
}

char mul(const char a, const char b, const char c, char &sum) {
  /* single char multiplication */
  int r = HEX2DEC(a) * HEX2DEC(b) + HEX2DEC(c);
  char carry;
  if (r >= 16) {
    sum = DEC2HEX(r % 16);
    carry = DEC2HEX(r / 16);
  } else {
    sum = DEC2HEX(r);
    carry = '0';
  }
  return carry;
}

void add_array(int len, char *a, char *b, char *s) {
  /* add two arrays, a must be larger than b in absolute value */
  char carry = '0';
  for (int i = 0; i <= len; ++i)
    s[i] = add(a[i], b[i], carry, carry);
}

void sub_array(int len, char *a, char *b, char *s) {
  /* sub two arrays, a must be larger than b in absolute value */
  char borrow = '0';
  for (int i = 0; i < len; ++i)
    s[i] = sub(a[i], b[i], borrow, borrow);
}

void remove_spaces(string &s) {
  /* remove spaces in a string */
  int cnt = 0;
  for (int i = 0; s[i]; ++i) {
    if (s[i] != ' ')
      s[cnt++] = s[i];
  }
  s[cnt] = '\0';
}

class BigNumber {
  private:
    bool _sign;  // true means negative
    int _len;  // length of digits, sign exclusive
    char *_array;  // digits of big number, sign exclusive
  public:
    // constructor
    BigNumber();
    BigNumber(const string&);
    BigNumber(const bool, const int, const char*);

    // destructor
    ~BigNumber();

    // operators
    bool operator>(const BigNumber&) const;
    bool operator<(const BigNumber&) const;
    bool operator==(const BigNumber&) const;
    BigNumber operator+(const BigNumber&) const;
    BigNumber operator-(const BigNumber&) const;
    BigNumber operator*(const BigNumber&) const;
    BigNumber operator/(const BigNumber&) const;
    BigNumber operator%(const BigNumber&) const;
    BigNumber& operator+=(const BigNumber&);
    BigNumber& operator-=(const BigNumber&);
    BigNumber& operator*=(const BigNumber&);
    BigNumber& operator/=(const BigNumber&);
    BigNumber& operator%=(const BigNumber&);
    BigNumber& operator=(const BigNumber&);

    // helper functions
    void print(const bool) const;
    bool is_negative() const {return _sign;}
    bool is_zero() const {
      BigNumber zero("0");
      return *this == zero;
    }
    int get_len() const {return _len;}
    char get_bit(const int i) const {return _array[i];}

    void update_len(const int);
    void rotate_left(const int);
    void rotate_right(const int);
    void set_as(const string&);
    void set_sign(const bool x) {_sign = x;}
};

BigNumber::BigNumber() {
  /* constructor: initialize */
  _sign = false;
  _len = 0;
  _array = new char[N];
  for (int i = 0; i < N; ++i)
    _array[i] = '0';
}

BigNumber::BigNumber(const string& array) {
  /* constructor: given a string */
  int new_len = 0;
  _sign = array[0] == '-';
  for (int i = 0; i < array.size(); ++i) {
    if (array[i] != '\0') ++new_len;
    else break;
  }
  _len = _sign ? new_len - 1 : new_len;
  _array = new char[N];
  for (int i = N - 1; i >= _len; --i)
    _array[i] = '0';
  if (_sign) {
    for (int i = _len - 1; i >= 0; --i)
      _array[i] = array[_len - i];
  } else {
    for (int i = _len - 1; i >= 0; --i)
      _array[i] = array[_len - i - 1];
  }
}

BigNumber::BigNumber(const bool sign, const int len, const char *array) {
  /* constructor: given sign, length, and char array */
  _sign = sign;
  _len = len;
  _array = new char[N];
  for (int i = N - 1; i >= _len; --i)
    _array[i] = '0';
  for (int i = _len - 1; i >= 0; --i)
    _array[i] = array[i];
}

BigNumber::~BigNumber() {
  /* destructor */
  delete[] _array;
}

bool BigNumber::operator>(const BigNumber &target) const {
  // absolute value comparison
  bool res;
  if (_len > target._len) {
    res = true;
  } else if (_len < target._len) {
    res = false;
  } else {
    int i;
    for (i = _len; i > 0 && _array[i - 1] == target._array[i - 1]; --i);
    res = !(i == 0 || _array[i - 1] < target._array[i - 1]);
  }
  return res;
}

bool BigNumber::operator<(const BigNumber &target) const {
  // absolute value comparison
  bool res;
  if (_len > target._len) {
    res = false;
  } else if (_len < target._len) {
    res = true;
  } else {
    int i;
    for (i = _len; i > 0 && _array[i - 1] == target._array[i - 1]; --i);
    res = !(i == 0 || _array[i - 1] > target._array[i - 1]);
  }
  return res;
}

bool BigNumber::operator==(const BigNumber &target) const {
  // absolute value comparison
  bool res;
  if (_len != target._len) {
    res = false;
  } else {
    int i;
    for (i = _len - 1; i >= 0 && _array[i] == target._array[i]; --i);
    res = (i == -1);
  }
  return res;
}

BigNumber BigNumber::operator+(const BigNumber &target) const {
  bool new_sign = false;
  int new_len;
  char sum_array[N];
  for (int i = 0; i < N; ++i) sum_array[i] = '0';
  if (!(_sign ^ target._sign)) {
    if (*this > target || *this == target) {
      add_array(_len, _array, target._array, sum_array);
    } else {
      add_array(target._len, target._array, _array, sum_array);
    }
    new_sign = _sign;
  } else {
    if (*this > target || *this == target) {
      sub_array(_len, _array, target._array, sum_array);
      new_sign = _sign;
    } else {
      sub_array(target._len, target._array, _array, sum_array);
      new_sign = target._sign;
    }
  }
  for (new_len = N; sum_array[new_len - 1] == '0' && new_len >= 1; --new_len);
  if (new_len == 0) new_len = 1;
  BigNumber sum(new_sign, new_len, sum_array);
  return sum;
}

BigNumber BigNumber::operator-(const BigNumber &target) const {
  bool new_sign = false;
  int new_len;
  char diff_array[N];
  for (int i = 0; i < N; ++i) diff_array[i] = '0';
  if (_sign ^ target._sign) {
    if (*this > target || *this == target) {
      add_array(_len, _array, target._array, diff_array);
    } else {
      add_array(target._len, target._array, _array, diff_array);
    }
    new_sign = _sign;
  } else {
    if (*this > target || *this == target) {
      sub_array(_len, _array, target._array, diff_array);
      new_sign = _sign;
    } else {
      sub_array(target._len, target._array, _array, diff_array);
      new_sign = !target._sign;
    }
  }
  for (new_len = N; diff_array[new_len - 1] == '0' && new_len >= 1; --new_len);
  if (new_len == 0) new_len = 1;
  BigNumber diff(new_sign, new_len, diff_array);
  return diff;
}

BigNumber BigNumber::operator*(const BigNumber &target) const {
  bool new_sign = this->_sign ^ target._sign;  // new sign depends on sign of two inputs
  int new_len;
  char product_array[N], carry;
  for (int i = 0; i < N; ++i) product_array[i] = '0';
  for (int i = 0; i < target._len; ++i) {
    if (target._array[i] == '0') continue;  // ignore when the bit is zero
    for (int j = 0; j < this->_len; ++j) {  // start from low-digit to high-digit
      carry = mul(target._array[i], this->_array[j], product_array[i + j], product_array[i + j]);
      // handle the carry bit
      int x = 1;
      while (i + j + x < N && carry != '0') {
        product_array[i + j + x] = add(product_array[i + j + x], carry, '0', carry);
        ++x;
      }
    }
  }
  for (new_len = N; product_array[new_len - 1] == '0' && new_len >= 1; --new_len);
  if (new_len == 0) {
    new_len = 1;
    new_sign = false;
  }
  BigNumber product(new_sign, new_len, product_array);
  return product;
}

BigNumber BigNumber::operator/(const BigNumber &target) const {
  BigNumber quotient;
  BigNumber this_copy, target_copy;
  this_copy = *this;
  target_copy = target;
  int cur_len = this_copy._len - target_copy._len;
  target_copy.rotate_left(cur_len);  // make divisor same length with dividend
  target_copy._len = min(N, target_copy._len + cur_len);
  for (int cur_val = 0; cur_len >= 0; --cur_len, cur_val = 0) {
    while (this_copy > target_copy || this_copy == target_copy) {
      ++cur_val;
      this_copy -= target_copy;
    }
    // put the current position's value into quotient
    quotient._array[cur_len] = DEC2HEX(cur_val);
    // each iteration, divisor is 16 times less than previous iteration
    target_copy.rotate_right(1);
    target_copy._len = max(0, target_copy._len - 1);
  }
  int i;
  for (i = N - 1; i >= 0 && quotient._array[i] == '0'; --i);
  if (i == -1) {
    quotient._len = 1;
    quotient._array[0] = '0';
  } else {
    quotient._len = i + 1;
  }
  return quotient;
}

BigNumber BigNumber::operator%(const BigNumber &target) const {
  BigNumber remainder, target_copy;
  remainder = *this;
  target_copy = target;
  if (remainder.is_negative()) {
    for (; remainder.is_negative(); remainder+=target_copy);
    return remainder;
  }
  int cur_len = remainder._len - target_copy._len;
  target_copy.rotate_left(cur_len);  // make divisor same length with dividend
  target_copy._len = min(N, target_copy._len + cur_len);
  for (int cur_val = 0; cur_len >= 0; --cur_len, cur_val = 0) {
    while (remainder > target_copy || remainder == target_copy) {
      // if divisor is less than dividend, add 1 to the current position in the quotient
      ++cur_val;
      remainder -= target_copy;
    }
    // each iteration, divisor is 16 times less than previous iteration
    target_copy.rotate_right(1);
    target_copy._len = max(0, target_copy._len - 1);
  }
  int i;
  for (i = N - 1; i >= 0 && remainder._array[i] == '0'; --i);
  if (i == -1) {
    remainder._len = 1;
    remainder._array[0] = '0';
  } else {
    remainder._len = i + 1;
  }
  return remainder;
}

BigNumber& BigNumber::operator+=(const BigNumber &target) {
  bool new_sign = false;
  int new_len;
  char sum_array[N];
  for (int i = 0; i < N; ++i) sum_array[i] = '0';
  if (!(_sign ^ target._sign)) {
    if (*this > target || *this == target)
      add_array(_len, _array, target._array, sum_array);
    else
      add_array(target._len, target._array, _array, sum_array);
    new_sign = _sign;
  } else {
    if (*this > target || *this == target) {
      sub_array(_len, _array, target._array, sum_array);
      new_sign = _sign;
    } else {
      sub_array(target._len, target._array, _array, sum_array);
      new_sign = target._sign;
    }
  }
  for (new_len = N; sum_array[new_len - 1] == '0' && new_len >= 1; --new_len);
  if (new_len == 0) {
    new_len = 1;
    new_sign = false;
  }
  _sign = new_sign;
  _len = new_len;
  for (int i = N - 1; i >= 0; --i)
    _array[i] = sum_array[i];
  return *this;
}

BigNumber& BigNumber::operator-=(const BigNumber &target) {
  bool new_sign;
  int new_len;
  char diff_array[N], borrow = '0';
  for (int i = 0; i < N; ++i) diff_array[i] = '0';
  if (*this == target) {
    // a == b
    new_sign = false;
    new_len = 1;
    diff_array[0] = '0';
  } else {
    if (*this > target) {
      // a > b
      new_sign = false;
      new_len = _len;
      for (int i = 0; i < new_len; ++i)  // start from low-digit to high-digit
        diff_array[i] = sub(_array[i], target._array[i], borrow, borrow);
    } else {
      // a < b
      new_sign = true;
      new_len = target._len;
      for (int i = 0; i < new_len; ++i)  // start from low-digit to high-digit
        diff_array[i] = sub(target._array[i], _array[i], borrow, borrow);
    }
    for (; diff_array[new_len - 1] == '0'; --new_len);  // find new length
  }
  // assign to itself instead of create a new instance
  _sign = new_sign;
  for (int i = _len - 1; i >= 0; --i)
    _array[i] = diff_array[i];
  _len = new_len;
  return *this;
}

BigNumber& BigNumber::operator*=(const BigNumber &target) {
  bool new_sign = this->_sign ^ target._sign;  // new sign depends on sign of two inputs
  int new_len;
  char product_array[N], carry;
  for (int i = 0; i < N; ++i) product_array[i] = '0';
  for (int i = 0; i < target._len; ++i) {
    if (target._array[i] == '0') continue;  // ignore when the bit is zero
    for (int j = 0; j < this->_len; ++j) {  // start from low-digit to high-digit
      carry = mul(target._array[i], this->_array[j], product_array[i + j], product_array[i + j]);
      // handle the carry bit
      int x = 1;
      while (i + j + x < N && carry != '0') {
        product_array[i + j + x] = add(product_array[i + j + x], carry, '0', carry);
        ++x;
      }
    }
  }
  for (new_len = N - 1; product_array[new_len - 1] == '0' && new_len >= 1; --new_len);
  if (new_len == 0) {
    new_len = 1;
    new_sign = false;
  }
  _sign = new_sign;
  _len = new_len;
  for (int i = N - 1; i >= 0; --i)
    _array[i] = product_array[i];
  return *this;
}

BigNumber& BigNumber::operator/=(const BigNumber &target) {
  bool new_sign = _sign ^ target._sign;
  int new_len;
  char quotient_array[N];
  for (int i = N - 1; i >= 0; --i)  quotient_array[i] = '0';
  BigNumber target_copy;
  target_copy = target;
  int cur_len = _len - target_copy._len;
  target_copy.rotate_left(cur_len);  // make divisor same length with dividend
  target_copy._len = min(N, target_copy._len + cur_len);
  for (int cur_val = 0; cur_len >= 0; --cur_len, cur_val = 0) {
    while (*this > target_copy || *this == target_copy) {
      // if divisor is less than dividend, add 1 to the current position in the quotient
      ++cur_val;
      *this -= target_copy;
    }
    // put the current position's value into quotient
    quotient_array[cur_len] = DEC2HEX(cur_val);
    // each iteration, divisor is 16 times less than previous iteration
    target_copy.rotate_right(1);
    target_copy._len = max(0, target_copy._len - 1);
  }
  for (new_len = N - 1; quotient_array[new_len - 1] == '0' && new_len >= 1; --new_len);
  if (new_len == 0) {
    new_len = 1;
    new_sign = false;
  }
  _sign = new_sign;
  _len = new_len;
  for (int i = N - 1; i >= 0; --i)
    _array[i] = quotient_array[i];
  return *this;
}

BigNumber& BigNumber::operator%=(const BigNumber &target) {
  if (is_negative()) {
    for (; is_negative(); *this += target);
    return *this;
  }
  bool new_sign = _sign ^ target._sign;
  int new_len;
  BigNumber target_copy;
  target_copy = target;
  int cur_len = _len - target_copy._len;
  target_copy.rotate_left(cur_len);  // make divisor same length with dividend
  target_copy._len = min(N, target_copy._len + cur_len);
  for (; cur_len >= 0; --cur_len) {
    while (*this > target_copy || *this == target_copy) {
      *this -= target_copy;
    }
    target_copy.rotate_right(1);  // each iteration, divisor is 16 times less than previous iteration
    target_copy._len = max(0, target_copy._len - 1);
  }
  // find new length
  for (new_len = N - 1; _array[new_len - 1] == '0' && new_len >= 1; --new_len);
  if (new_len == 0) {
    new_len = 1;
    new_sign = false;
  }
  _sign = new_sign;
  _len = new_len;
  return *this;
}

BigNumber& BigNumber::operator=(const BigNumber &target) {
  _sign = target._sign;
  _len = target._len;
  for (int i = N - 1; i >= _len; --i)
    _array[i] = '0';
  for (int i = _len - 1; i >= 0; --i)
    _array[i] = target._array[i];
  return *this;
}

inline void BigNumber::print(const bool padding) const {
  if (_sign)  // if it is negative
    cout << "-";
  int cnt = 0;
  if (padding)
    for (; (_len + cnt) % 8 != 0; cout << "0", cnt += 1);
  for (int i = _len - 1; i >= 0; --i) {
    cout << _array[i];
    if (++cnt == 8 && i != 0) {
      cout << " ";
      cnt = 0;
    }
  }
}

inline void BigNumber::update_len(const int l) {
  _len += l;
  if (_len <= 0) {
    _len = 1;
    _sign = false;
  }
}

inline void BigNumber::rotate_left(const int x){
  // shift all bits to left by x times
  for (int i = min(N, _len + x) - 1; i >= x; --i)
    _array[i] = _array[i - x];
  for (int i = x - 1; i >= 0; --i)
    _array[i] = '0';
}

inline void BigNumber::rotate_right(const int x) {
  // shift all bits to right by x times
  for (int i = 0; i < _len - x; ++i)
    _array[i] = _array[i + x];
  for (int i = _len - x; i < _len; ++i)
    _array[i] = '0';
}

inline void BigNumber::set_as(const string &array) {
  _sign = array[0] == '-';
  int new_len = 0;
  for (int i = 0; i < array.size(); ++i) {
    if (array[i] != '\0') ++new_len;
    else break;
  }
  _len = _sign ? new_len - 1 : new_len;
  for (int i = N - 1; i >= _len; --i)
    _array[i] = '0';
  if (_sign) {
    for (int i = _len - 1; i >= 0; --i)
      _array[i] = array[_len - i];
  } else {
    for (int i = _len - 1; i >= 0; --i)
      _array[i] = array[_len - i - 1];
  }
}

void modular_exp(BigNumber &y, const BigNumber &a, const BigNumber &m,
                 const BigNumber &n) {
  /* Compute y = (a ^ m) % n */
  BigNumber a_copy, m_copy;
  a_copy = a;
  m_copy = m;
  while (!(m_copy.is_zero())) {
    for (int i = 0; i < HEX2DEC(m_copy.get_bit(0)); ++i) {
      y *= a_copy;
      y %= n;
    }
    for (int i = 0; i < 4; ++i) {
      a_copy *= a_copy;
      a_copy %= n;
    }
    m_copy.rotate_right(1);
    m_copy.update_len(-1);
  }
}

BigNumber square_root(const BigNumber &a, const BigNumber &p) {
  /* Compute x such that x^2 = a % p */
  BigNumber d("1"), sqrt("1");
  BigNumber one("1"), two("2"), three("3"), four("4"), five("5"), eight("8");
  BigNumber a_copy, p_copy, p_copy_copy;
  a_copy = a;
  p_copy = p;
  p_copy_copy = p;
  a_copy %= p_copy;
  if ((p_copy % four) == three) {
    modular_exp(sqrt, a_copy, (p_copy + one) / four, p_copy_copy);
  } else if ((p_copy % eight) == five) {
    modular_exp(d, a_copy, (p_copy - one) / four, p_copy);
    if ((d % p_copy) == one) {
      sqrt.set_as("1");
      modular_exp(sqrt, a, (p_copy + three) / eight, p_copy);
    } else if (d == (p_copy - one)) {
      sqrt.set_as("1");
      modular_exp(sqrt, a_copy * four, (p_copy - five) / eight, p_copy);
      sqrt *= a_copy;
      sqrt *= two;
      sqrt %= p_copy;
    }
  }
  return sqrt;
}

void extended_euclid_algo(const BigNumber &p, const BigNumber &q,
                          BigNumber &c) {
  /* use EEA to find multiplicative inverse */
  BigNumber tmp, m, r0, r1;
  BigNumber s0("1"), s1("0");
  r0 = p;
  r1 = q;
  while (!(r1.is_zero())) {
    m = r0 / r1;

    tmp = r0;
    r0 = r1;
    r1 = tmp % r1;

    tmp = s0;
    s0 = s1;
    s1 = tmp - (s1 * m);
  }
  c = s0;
}

class Point {
  private:
    BigNumber x;
    BigNumber y;
  public:
    // constructor
    Point();
    Point(const BigNumber&, const BigNumber&);

    // operators
    bool operator==(const Point&) const;
    Point operator-() const;  // negation
    Point operator+(const Point&) const;  // addition and doubling
    Point operator*(const BigNumber&) const;

    // helper functions
    void print() const;
    void get_x(BigNumber& xx) const {xx = x;}
    void get_y(BigNumber& yy) const {yy = y;}
    bool is_zero() const {
      BigNumber zero("0");
      return x == zero && y == zero;
    }

    void set_x(const BigNumber& xx) {x = xx;}
    void set_y(const BigNumber& yy) {y = yy;}
};

Point::Point() {
  BigNumber zero("0");
  set_x(zero);
  set_y(zero);
}

Point::Point(const BigNumber &xx, const BigNumber &yy) {
  set_x(xx);
  set_y(yy);
}

bool Point::operator==(const Point &P) const {
  return x == P.x && y == P.y;
}

Point Point::operator-() const {
  BigNumber xx, yy;
  get_x(xx);
  get_y(yy);
  yy.set_sign(true);
  return Point(xx, yy);
}

Point Point::operator+(const Point &Q) const {
  if (is_zero()) {
    BigNumber xx, yy;
    Q.get_x(xx);
    Q.get_y(yy);
    return Point(xx, yy);
  } else if (Q.is_zero()) {
    BigNumber xx, yy;
    get_x(xx);
    get_y(yy);
    return Point(xx, yy);
  } else if (x == Q.x && (!(y == Q.y) || y.is_zero())) return Point();
  BigNumber p(str_p), a(str_a), b(str_b);
  BigNumber two("2"), three("3");
  BigNumber l;
  if (*this == Q) {
    BigNumber inv;
    BigNumber t;
    t = two * y;
    t %= p;
    extended_euclid_algo(t, p, inv);
    inv %= p;
    l = (three * x * x + a) * inv;
  } else {
    BigNumber inv;
    BigNumber s, t;
    t = Q.x - x;
    t %= p;
    extended_euclid_algo(t, p, inv);
    inv %= p;
    s = Q.y - y;
    s %= p;
    l = s * inv;
  }
  l %= p;
  BigNumber x3, y3;
  x3 = ((l * l) - x - Q.x);
  x3 %= p;
  BigNumber t;
  t = x - x3;
  t %= p;
  y3 = (l * t - y);
  y3 %= p;
  return Point(x3, y3);
}

Point Point::operator*(const BigNumber& k) const {
  BigNumber k_copy;
  k_copy = k;
  Point r;
  BigNumber xx, yy;
  get_x(xx);
  get_y(yy);
  Point P(xx, yy);
  while (!(k_copy.is_zero())) {
    for (int i = 0; i < HEX2DEC(k_copy.get_bit(0)); ++i)
      r = r + P;
    for (int i = 0; i < 4; ++i)
      P = P + P;
    k_copy.rotate_right(1);
    k_copy.update_len(-1);
  }
  return r;
}

void Point::print() const {
  x.print(true);
  y.print(true);
}

class ECElGamal {
  public:
    Point solve(const BigNumber&, const bool);

    void encrypt(const Point&, const Point&, const Point&, const BigNumber&,
                 Point&, Point&);
    void decrypt(const Point&, const Point&, const BigNumber&, Point&);
};

Point ECElGamal::solve(const BigNumber &Px, const bool parity) {
  /* choose even y-coordinate if parity is true */
  BigNumber p(str_p), a(str_a), b(str_b);
  BigNumber Px_cub = (Px * Px * Px) % p;
  BigNumber y_sq = (Px_cub + Px * a + b) % p;
  BigNumber Py = square_root(y_sq, p);
  BigNumber lhs = (Py * Py) % p;
  BigNumber rhs;
  rhs = y_sq;
  if (lhs == rhs) {
    // check if valid
    bool odd = HEX2DEC(Py.get_bit(0)) % 2 == 1;
    if (!(parity ^ odd))
      Py = p - Py;
    return Point(Px, Py);
  }
  return Point();
}

void ECElGamal::encrypt(const Point &G, const Point &Pm, const Point &Pa,
                        const BigNumber &nk, Point &Pk, Point &Pb)
{
  Pk = G * nk;
  Pb = Pm + Pa * nk;
}

void ECElGamal::decrypt(const Point &Pk, const Point &Pb, const BigNumber &na,
                        Point &Pm)
{
  Pm = Pb + (- (Pk * na));
}

int main(int argc, const char * argv[]) {
  ECElGamal ec_elgamal;

  // encryption
  cout << "<EC-ElGamal encryption>" << endl;
  string str_m, str_pa, str_nk;
  cout << "Plaintext M = ";
  getline(cin, str_m);
  cout << "Pa = ";
  getline(cin, str_pa);
  cout << "nk = ";
  getline(cin, str_nk);

  Point Pm;
  remove_spaces(str_m);
  BigNumber one("1"), Mx(str_m);
  Mx.rotate_left(2);
  Mx.update_len(2);
  while (1) {
    Pm = ec_elgamal.solve(Mx, false);
    if (!Pm.is_zero()) break;
    Mx += one;
  }
  BigNumber My;
  Pm.get_x(Mx);
  Pm.get_y(My);
  cout << "Mx = ";
  Mx.print(true);
  cout << endl;
  cout << "My = ";
  My.print(true);
  cout << endl;

  Point Pa;
  remove_spaces(str_pa);
  bool odd_or_even = str_pa[1] == '2';
  BigNumber Pax(str_pa.substr(2));
  Pa = ec_elgamal.solve(Pax, odd_or_even);

  BigNumber gx(str_gx), gy(str_gy);
  Point G(gx, gy);
  remove_spaces(str_nk);
  BigNumber nk(str_nk);
  Point Pk, Pb;
  ec_elgamal.encrypt(G, Pm, Pa, nk, Pk, Pb);
  BigNumber Pkx, Pbx;
  Pk.get_x(Pkx);
  Pb.get_x(Pbx);
  cout << "Cm = {Pk,Pb} = {";
  Pkx.print(true);
  cout << ", ";
  Pbx.print(true);
  cout << "}" << endl << endl;

  // decryption
  cout << "<EC-ElGamal decryption>" << endl;
  string str_pk, str_pb, str_na;
  cout << "Pk = ";
  getline(cin, str_pk);
  cout << "Pb = ";
  getline(cin, str_pb);
  cout << "na = ";
  getline(cin, str_na);

  remove_spaces(str_pk);
  odd_or_even = str_pk[1] == '2';
  Pkx.set_as(str_pk.substr(2));
  Pk = ec_elgamal.solve(Pkx, odd_or_even);
  remove_spaces(str_pb);
  odd_or_even = str_pb[1] == '2';
  Pbx.set_as(str_pb.substr(2));
  Pb = ec_elgamal.solve(Pbx, odd_or_even);
  remove_spaces(str_na);
  BigNumber na(str_na);

  ec_elgamal.decrypt(Pk, Pb, na, Pm);
  BigNumber Pmx;
  Pm.get_x(Pmx);
  Pmx.rotate_right(2);
  Pmx.update_len(-2);
  cout << "Plaintext = ";
  Pmx.print(false);
  cout << endl;

  return 0;
}
