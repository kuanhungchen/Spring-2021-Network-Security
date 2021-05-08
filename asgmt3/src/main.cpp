#include <iostream>
#include <cstdlib>
#include <ctime>
#include <random>

using namespace std;

#define N 20000

const char *string_primes[3] = {
  "2", "3", "5"
};

int HEX2DEC(const char c) {
  int d;
  if (c >= '0' && c <= '9') d = (int)(c - 48);
  else d = (int)(c - 97 + 10);
  return d;
}

char DEC2HEX(const int d) {
  char c;
  if ((int)d < 10) c = (char)(d + 48);
  else c = (char)(d + 97 - 10);
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

void remove_spaces(string &s) {
  int cnt = 0;
  for (int i = 0; s[i]; ++i) {
    if (s[i] != ' ')
      s[cnt++] = s[i];
  }
  s[cnt] = '\0';
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
    void random(const int);
    void random(const BigNumber&, const BigNumber&);
    void print() const;
    void rotate_left(const int);
    void rotate_right(const int);
    bool is_even();
    int get_len() {return this->_len;}
    char get_bit(const int i) {return this->_array[i];}
    void set_as(const string&);
    void update_len(const int);
    void padding(const BigNumber&, const int);
};

BigNumber::BigNumber() {
  /* constructor: initialize */
  this->_sign = false;
  this->_len = 0;
  this->_array = new char[N];
  for (int i = 0; i < N; ++i)
    this->_array[i] = '0';
}

BigNumber::BigNumber(const string& array) {
  /* constructor: given a string */
  int _len = 0;
  this->_sign = array[0] == '-';
  for (int i = 0; i < array.size(); ++i) {
    if (array[i] != '\0') ++_len;
    else break;
  }
  this->_len = _len;
  this->_array = new char[N];
  for (int i = N - 1; i >= this->_len; --i)
    this->_array[i] = '0';
  for (int i = this->_len - 1; i >= 0; --i)
    this->_array[i] = array[this->_len - i - 1];
}

BigNumber::BigNumber(const bool sign, const int len, const char *array) {
  /* constructor: given sign, length, and char array */
  this->_sign = sign;
  this->_len = len;
  this->_array = new char[N];
  for (int i = N - 1; i >= this->_len; --i)
    this->_array[i] = '0';
  for (int i = this->_len - 1; i >= 0; --i)
    this->_array[i] = array[i];
}

BigNumber::~BigNumber() {
  /* destructor */
  delete [] this->_array;
}

bool BigNumber::operator>(const BigNumber &target) const {
  // only care about magnitude
  bool res;
  if (_len > target._len) {
    res = true;
  } else if (_len < target._len) {
    res = false;
  } else {
    int i;
    for (i = this->_len; i > 0 && this->_array[i - 1] == target._array[i - 1]; --i);
    res = !(i == 0 || this->_array[i - 1] < target._array[i - 1]);
  }
  return res;
}

bool BigNumber::operator==(const BigNumber &target) const {
  // only care about magnitude
  bool res;
  if (_len != target._len) {
    res = false;
  } else {
    int i;
    for (i = this->_len - 1; i >= 0 && this->_array[i] == target._array[i]; --i);
    res = (i == -1);
  }
  return res;
}

bool BigNumber::operator<(const BigNumber &target) const {
  // only care about magnitude
  bool res;
  if (this->_len > target._len) {
    res = false;
  } else if (this->_len < target._len) {
    res = true;
  } else {
    int i;
    for (i = this->_len; i > 0 && this->_array[i - 1] == target._array[i - 1]; --i);
    res = !(i == 0 || this->_array[i - 1] > target._array[i - 1]);
  }
  return res;
}
void add_array(int len, char *a, char *b, char *s) {
  char carry = '0';
  for (int i = 0; i <= len; ++i)
    s[i] = add(a[i], b[i], carry, carry);
}

void sub_array(int len, char *a, char *b, char *s) {
  char borrow = '0';
  for (int i = 0; i < len; ++i)
    s[i] = sub(a[i], b[i], borrow, borrow);
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
  for (new_len = this->_len * target._len + 1; new_len > 0 &&
      product_array[new_len - 1] == '0'; --new_len);  // find new length
  if (new_len == 0) {
    // if result is zero
    new_sign = false;
    new_len = 1;
    product_array[0] = '0';
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
    quotient._array[cur_len] = DEC2HEX(cur_val);  // put the current position's value into quotient
    target_copy.rotate_right(1);  // each iteration, divisor is 16 times less than previous iteration
    target_copy._len = max(0, target_copy._len - 1);
  }

  // find new length
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
    target_copy.rotate_right(1);  // each iteration, divisor is 16 times less than previous iteration
    target_copy._len = max(0, target_copy._len - 1);
  }

  // find new length
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

BigNumber& BigNumber::operator=(const BigNumber &target) {
  _sign = target._sign;
  _len = target._len;
  for (int i = N - 1; i >= _len; --i)
    _array[i] = '0';
  for (int i = _len - 1; i >= 0; --i)
    _array[i] = target._array[i];
  return *this;
}

BigNumber& BigNumber::operator+=(const BigNumber &target) {
  bool new_sign = false;
  int new_len = max(this->_len, target._len);
  char sum_array[N], carry = '0';
  for (int i = 0; i < N; ++i) sum_array[i] = '0';

  for (int i = 0; i <= new_len; ++i)  // start from low-digit to high-digit
    sum_array[i] = add(this->_array[i], target._array[i], carry, carry);
  if (sum_array[new_len] != '0') ++new_len;  // if carry exists at highest-digit, then the length will be increased

  _sign = new_sign;
  _len = new_len;
  for (int i = _len - 1; i >= 0; --i)
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
  for (new_len = this->_len * target._len + 1; new_len > 0 &&
      product_array[new_len - 1] == '0'; --new_len);  // find new length
  if (new_len == 0) {
    // if result is zero
    new_sign = false;
    new_len = 1;
    product_array[0] = '0';
  }

  // assign to itself instead of create a new instance
  this->_sign = new_sign;
  this->_len = new_len;
  for (int i = N - 1; i >= this->_len; --i)
    this->_array[i] = '0';
  for (int i = this->_len - 1; i >= 0; --i)
    this->_array[i] = product_array[i];
  return *this;
}

BigNumber& BigNumber::operator/=(const BigNumber &target) {
  bool new_sign = this->_sign ^ target._sign;
  int new_len;
  char quotient_array[N];
  for (int i = N - 1; i >= 0; --i)  quotient_array[i] = '0';
  BigNumber target_copy;
  target_copy = target;
  int cur_len = this->_len - target_copy._len;
  target_copy.rotate_left(cur_len);  // make divisor same length with dividend
  target_copy._len = min(N, target_copy._len + cur_len);
  for (int cur_val = 0; cur_len >= 0; --cur_len, cur_val = 0) {
    while (*this > target_copy || *this == target_copy) {
      // if divisor is less than dividend, add 1 to the current position in the quotient
      ++cur_val;
      *this -= target_copy;
    }
    quotient_array[cur_len] = DEC2HEX(cur_val);
    target_copy.rotate_right(1);  // each iteration, divisor is 16 times less than previous iteration
    target_copy._len = max(0, target_copy._len - 1);
  }

  // find new length
  int i;
  for (i = N - 1; i >= 0 && quotient_array[i] == '0'; --i);
  if (i == -1) {
    new_len = 1;
    quotient_array[0] = '0';
  } else {
    new_len = i + 1;
  }
  for (int i = N - 1; i >= 0; --i) {
    this->_array[i] = quotient_array[i];
  }
  this->_len = new_len;
  this->_sign = new_sign;
  return *this;
}

BigNumber& BigNumber::operator%=(const BigNumber &target) {
  BigNumber target_copy;
  target_copy = target;

  int cur_len = this->_len - target_copy._len;
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
  int i;
  for (i = N - 1; i >= 0 && this->_array[i] == '0'; --i);
  if (i == -1) {
    this->_len = 1;
    this->_array[0] = '0';
  } else {
    this->_len = i + 1;
  }
  return *this;
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
  /* a_copy %= p_copy; */

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
                          BigNumber &c, BigNumber &d) {
  BigNumber tmp, m, r0, r1;
  BigNumber s0("1"), s1("0");
  BigNumber t0("0"), t1("1");
  r0 = p;
  r1 = q;

  while (!(r1.get_len() == 1 && r1.get_bit(0) == '0')) {
    m = r0 / r1;

    tmp = r0;
    r0 = r1;
    r1 = tmp % r1;

    tmp = s0;
    s0 = s1;
    s1 = tmp - (s1 * m);

    tmp = t0;
    t0 = t1;
    t1 = tmp - (t1 * m);
  }
  c = s0;
  d = t0;
}

/* BigNumber BigNumber::div_and_mod(BigNumber &target) { */
/*   BigNumber quotient; */

/*   int cur_len = this->_len - target._len; */
/*   target.rotate_left(cur_len);  // make divisor same length with dividend */
/*   target._len = min(N, target._len + cur_len); */
/*   for (int cur_val = 0; cur_len >= 0; --cur_len, cur_val = 0) { */
/*     while (*this > target || *this == target) { */
/*       // if divisor is less than dividend, add 1 to the current position in the quotient */
/*       ++cur_val; */
/*       *this -= target; */
/*     } */
/*     quotient._array[cur_len] = DEC2HEX(cur_val);  // put the current position's value into quotient */
/*     target.rotate_right(1);  // each iteration, divisor is 16 times less than previous iteration */
/*     target._len = max(0, target._len - 1); */
/*   } */

/*   // find new length */
/*   int i; */
/*   for (i = N - 1; i >= 0 && quotient._array[i] == '0'; --i); */
/*   if (i == -1) { */
/*     quotient._len = 1; */
/*     quotient._array[0] = '0'; */
/*   } else { */
/*     quotient._len = i + 1; */
/*   } */
/*   for (i = N - 1; i >= 0 && this->_array[i] == '0'; --i); */
/*   if (i == -1) { */
/*     this->_len = 1; */
/*     this->_array[0] = '0'; */
/*   } else { */
/*     this->_len = i + 1; */
/*   } */

/*   return quotient; */
/* } */

inline void BigNumber::random(const int b) {
  this->_len = b;
  for (int i = 0; i < b; ++i) {
    this->_array[i] = DEC2HEX(rand() % (15 - 0 + 1) + 0);
  }
}

inline void BigNumber::random(const BigNumber &a, const BigNumber &b) {
  this->_len = b._len;
  for (int i = 0; i < b._len - 1; ++i)
    this->_array[i] = DEC2HEX(rand() % (15 - 0 + 1) + 0);
  if (b._array[b._len - 1] == '1')
    --this->_len;
  else
    this->_array[this->_len - 1] =
        DEC2HEX(rand() % (HEX2DEC(b._array[b._len - 1]) - 1 - 1 + 1) + 1);
}

inline void BigNumber::print() const {
  if (_sign)  // if it is negative
    cout << "-";
  for (int i = _len - 1; i >= 0; --i)
    cout << _array[i];
  cout << " (" << _len << ")" << endl;
}

inline void BigNumber::rotate_left(const int x){
  // shift all bits to left by x times
  for (int i = min(N, this->_len + x) - 1; i >= x; --i)
    this->_array[i] = this->_array[i - x];
  for (int i = x - 1; i >= 0; --i)
    this->_array[i] = '0';
}

inline void BigNumber::rotate_right(const int x) {
  // shift all bits to right by x times
  for (int i = 0; i < _len - x; ++i)
    _array[i] = _array[i + x];
  for (int i = _len - x; i < _len; ++i)
    _array[i] = '0';
}

inline bool BigNumber::is_even() {
  return HEX2DEC(this->_array[0]) % 2 == 0;
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

inline void BigNumber::update_len(const int l) {
  _len += l;
  if (_len <= 0) {
    _len = 1;
    _array[0] = '0';
  }
}

inline void BigNumber::padding(const BigNumber &m, const int b) {
  for (int i = 0; i < b; ++i)
    _array[i] = m._array[i];
}

class MillerRabin {
  private:
    const char **small_primes;

  public:
    MillerRabin();
    ~MillerRabin();
    bool primality_test(const BigNumber&, const int);
    BigNumber encrypt(const BigNumber&, const BigNumber&);
    void decrypt(const BigNumber&, const BigNumber&, const BigNumber&,
                 BigNumber&, BigNumber&, BigNumber&, BigNumber&);
};

MillerRabin::MillerRabin() {
  small_primes = new const char*[3];
  for (int i = 0; i < 3; ++i)
    small_primes[i] = string_primes[i];
}

MillerRabin::~MillerRabin() {
  if (small_primes)
    delete[] small_primes;
}

bool MillerRabin::primality_test(const BigNumber &n, const int t) {
  BigNumber one("1");
  BigNumber two("2");
  if (n < two) {
    cout << "n < 2" << endl;
    return false;
  }
  BigNumber n_copy, small_prime;
  for (int i = 0; i < 3; ++i) {
    n_copy = n;
    small_prime.set_as(small_primes[i]);
    n_copy %= small_prime;
    if (n_copy.get_len() == 1 && n_copy.get_bit(0) == '0') {
      cout << "divisible by small primes, not a prime" << endl;
      return false;
    }
  }

  int k = 0;
  BigNumber m = n - one;
  while (m.is_even()) {
    ++k;
    m /= two;
  }

  BigNumber a, y;
  for (int i = 1; i <= t; ++i) {
    /* cout << "trial: " << i << ", total: " << t << endl; */
    a.random(two, m);
    /* cout << "a:" << endl; */
    /* a.print(); */
    /* cout << "m:" << endl; */
    /* m.print(); */
    /* cout << "n:" << endl; */
    /* n.print(); */

    y.set_as("1");
    modular_exp(y, a, m, n);
    /* cout << "y (a ^ m % n)" << endl; */
    /* y.print(); */

    if (!(y == one) && !(y == (n - one))) {
      for (int j = 1; j <= k - 1 && !(y == (n - one)); ++j) {
        /* cout << "step: " << j << ", total: " << k - 1 << endl; */
        y *= y;
        /* cout << "y:" << endl; */
        /* y.print(); */
        y %= n;
        /* cout << "y:" << endl; */
        /* y.print(); */
        if (y == one) {
          cout << "y == 1, not a prime" << endl;
          return false;
        }
      }
      if (!(y == n - one)) {
        cout << "y != n - 1, not a prime" << endl;
        return false;
      }
    }
  }
  cout << "probably a prime" << endl;
  return true;
}

BigNumber MillerRabin::encrypt(const BigNumber &m, const BigNumber &key) {
  BigNumber c;
  c = m;

  // 16-bit repetition padding
  c.rotate_left(4);
  c.update_len(4);
  c.padding(m, 4);

  c *= c;
  c %= key;
  return c;
}

void MillerRabin::decrypt(const BigNumber &p, const BigNumber &q,
                          const BigNumber &cipher, BigNumber &c1,
                          BigNumber &c2, BigNumber &c3, BigNumber &c4) {
  BigNumber c, d;
  BigNumber n = p * q;
  BigNumber sqrt_p = square_root(cipher, p);
  sqrt_p.print();
  BigNumber sqrt_q = square_root(cipher, q);
  sqrt_q.print();
  extended_euclid_algo(p, q, c, d);
  c.print();
  d.print();

  c1 = (sqrt_q * c * p + sqrt_p * d * q) % n;
  c2 = (sqrt_q * c * p - sqrt_p * d * q) % n;
  c3 = n - c1;
  c4 = n - c2;
  c1.rotate_right(4);
  c1.update_len(4);
  c2.rotate_right(4);
  c2.update_len(4);
  c3.rotate_right(4);
  c3.update_len(4);
  c4.rotate_right(4);
  c4.update_len(4);
}

int main(int argc, const char * argv[]) {
  MillerRabin miller_rabin;
  srand(time(0));  // setup random seed

  // generate 256-bit prime number
  /* cout << "<Miller-Rabin>" << endl; */
  /* BigNumber random_prime; */
  /* random_prime.random(64); */
  /* while (!miller_rabin.primality_test(random_prime, 12)) { */
  /*   random_prime.random(64); */
  /* } */
  /* random_prime.print(); */

  // Rabin Encryption
  cout << "<Rabin Encryption>" << endl;
  string p, q;
  cout << "p = ";
  getline(cin, p);
  cout << "q = ";
  getline(cin, q);

  BigNumber big_num_p, big_num_q;
  remove_spaces(p);
  remove_spaces(q);
  big_num_p.set_as(p);
  big_num_q.set_as(q);

  BigNumber big_num_n = big_num_p * big_num_q;
  cout << "n = pq = ";
  big_num_n.print();

  string plaintext;
  cout << "Plaintext: ";
  getline(cin, plaintext);

  BigNumber big_num_plaintext;
  remove_spaces(plaintext);
  big_num_plaintext.set_as(plaintext);

  BigNumber big_num_ciphertext = miller_rabin.encrypt(big_num_plaintext,
                                                      big_num_n);
  cout << "Cyphertext = ";
  big_num_ciphertext.print();

  // Rabin Decryption
  cout << "<Rabin Decryption>" << endl;
  string ciphertext;
  cout << "Ciphertext = ";
  getline(cin, ciphertext);

  remove_spaces(ciphertext);
  big_num_ciphertext.set_as(ciphertext);

  cout << "Private Key:" << endl;
  cout << "p = ";
  getline(cin, p);
  cout << "q = ";
  getline(cin, q);

  remove_spaces(p);
  remove_spaces(q);
  big_num_p.set_as(p);
  big_num_q.set_as(q);

  BigNumber c1, c2, c3, c4;
  miller_rabin.decrypt(big_num_p, big_num_q, big_num_ciphertext,
                       c1, c2, c3, c4);
  c1.print();
  c2.print();
  c3.print();
  c4.print();

  return 0;
}
