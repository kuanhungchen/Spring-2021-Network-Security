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
    void print() const;
    bool is_even() const;
    int get_len() const {return _len;}
    char get_bit(const int i) const {return _array[i];}

    void update_len(const int);
    void rotate_left(const int);
    void rotate_right(const int);
    void padding(const BigNumber&, const int);
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
  /* a subtraction helper function to prevent re-instancialize */
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

inline void BigNumber::print() const {
  if (_sign)  // if it is negative
    cout << "-";
  for (int i = _len - 1, cnt = 0; i >= 0; --i) {
    cout << _array[i];
    if (++cnt == 8) {
      cout << " ";
      cnt = 0;
    }
  }
  cout << endl;
}

inline bool BigNumber::is_even() const {
  return HEX2DEC(_array[0]) % 2 == 0;
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

inline void BigNumber::padding(const BigNumber &m, const int b) {
  /* padding the last b-bits in the tail with m */
  for (int i = 0; i < b; ++i)
    _array[i] = m._array[i];
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
  while (!(m_copy.get_len() == 1 && m_copy.get_bit(0) == '0')) {
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

class Point {
  private:
    BigNumber x;
    BigNumber y;
  public:
    Point();
    Point(const BigNumber&, const BigNumber&);

    bool operator==(const Point&) const;
    Point operator+(const Point&) const;
    Point operator*(const BigNumber&) const;

    void negating() {y.set_sign(true);}
    void print() const;
    void set_x(const BigNumber& xx) {x = xx;}
    void set_y(const BigNumber& yy) {y = yy;}
};

Point::Point() {
  x.set_as("0");
  y.set_as("0");
}

Point::Point(const BigNumber &xx, const BigNumber &yy) {
  x = xx;
  y = yy;
}

bool Point::operator==(const Point &P) const {
  return x == P.x && y == P.y;
}

Point Point::operator+(const Point &Q) const {
  BigNumber a(str_a), p(str_p);
  BigNumber two("2"), three("3");
  BigNumber l;
  if (*this == Q)
    l = (three * (x * x) + a) / (two * y);
  else
    l = (Q.y - y) / (Q.x - x);
  l %= p;
  BigNumber x3 = (l * l) - x - Q.x;
  x3 %= p;
  BigNumber y3 = l * (x - x3) - y;
  y3 %= p;
  return Point(x3, y3);
}

void Point::print() const {
  x.print();
  y.print();
}

class ECElGamal {
  private:
    BigNumber p;
    BigNumber a;
    BigNumber b;
    BigNumber gx, gy;
    Point G;
    BigNumber n;
  public:
    ECElGamal();

    Point find(const BigNumber&, const bool);
    Point solve(const BigNumber&);

    void encrypt(const BigNumber&, const Point&, const Point&, const Point&,
                 const Point&, const Point&);
    void decrypt(const BigNumber&, const Point&, const Point&,
                 const BigNumber&);
};

ECElGamal::ECElGamal() {
  p.set_as(str_p);
  a.set_as(str_a);
  b.set_as(str_b);
  gx.set_as(str_gx);
  gy.set_as(str_gy);
  G.set_x(gx);
  G.set_y(gy);
  n.set_as(str_n);
}


Point ECElGamal::find(const BigNumber &Px, const bool parity) {
  BigNumber Px_cub = Px * Px * Px;
  Px_cub %= p;
  BigNumber y_sq = Px_cub + Px * a + b;
  y_sq %= p;
  BigNumber y_sqrt = square_root(y_sq, p);
  // TODO: check parity
  return Point(Px, y_sqrt);
}

int main(int argc, const char * argv[]) {
  ECElGamal ec_elgamal;
  BigNumber Pax("668E9E1D01A306A1AB76C9949A973248E3AB5300");
  Point Pa = ec_elgamal.find(Pax, true);
  Pa.print();
  return 0;
}
