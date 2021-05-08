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
    bool operator==(const BigNumber&) const;
    bool operator<(const BigNumber&) const;
    BigNumber operator+(const BigNumber&) const;
    BigNumber operator-(const BigNumber&) const;
    BigNumber operator*(const BigNumber&) const;
    /* BigNumber operator/(BigNumber&);  // modify itself to represent remainder */
    /* BigNumber operator%(const BigNumber&); */
    BigNumber& operator-=(const BigNumber&);  // prevent re-instancialize
    BigNumber& operator*=(const BigNumber&);
    BigNumber& operator/=(const BigNumber&);
    BigNumber& operator%=(const BigNumber&);

    // helper functions
    void random(const int);
    void random(const BigNumber&, const BigNumber&);
    void print() const;
    void rotate_left(const int);
    void rotate_right(const int);
    /* BigNumber div_and_mod(BigNumber&);  // divide and modulo can be calculated at the same time */
    bool is_even();
    int get_len() {return this->_len;}
    char get_bit(const int i) {return this->_array[i];}
    void copy(const BigNumber&);
    void set_as(const string&);
    void last_bit_minus();
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
  if (this->_len > target._len) {
    res = true;
  } else if (this->_len < target._len) {
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
  if (this->_len != target._len) {
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

BigNumber BigNumber::operator+(const BigNumber &target) const {
  bool new_sign = false;
  int new_len = max(this->_len, target._len);
  char sum_array[N], carry = '0';
  for (int i = 0; i < N; ++i) sum_array[i] = '0';

  for (int i = 0; i <= new_len; ++i)  // start from low-digit to high-digit
    sum_array[i] = add(this->_array[i], target._array[i], carry, carry);
  if (sum_array[new_len] != '0') ++new_len;  // if carry exists at highest-digit, then the length will be increased

  BigNumber sum(new_sign, new_len, sum_array);
  return sum;
}

BigNumber BigNumber::operator-(const BigNumber &target) const {
  bool new_sign;
  int new_len;
  char diff_array[N], borrow = '0';
  for (int i = 0; i < N; ++i) diff_array[i] = '0';

  if (*this == target) {
    // a == b
    new_sign = false;
    new_len = 1;
  } else {
    if (*this > target) {
      // a > b
      new_sign = false;
      new_len = this->_len;
      for (int i = 0; i < new_len; ++i)  // start from low-digit to high-digit
        diff_array[i] = sub(this->_array[i], target._array[i], borrow, borrow);
    } else {
      // a < b
      new_sign = true;
      new_len = target._len;
      for (int i = 0; i < new_len; ++i)  // start from low-digit to high-digit
        diff_array[i] = sub(target._array[i], this->_array[i], borrow, borrow);
    }
    for (; diff_array[new_len - 1] == '0'; --new_len);  // find new length
  }

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

/* BigNumber BigNumber::operator/(BigNumber &target) { */
/*   return this->div_and_mod(target);  // use helper function to calculate both division and modulo */
/* } */

/* BigNumber BigNumber::operator%(const BigNumber &target) { */
/*   return *this;  // after division, the original a has become remainder */
/* } */

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
      new_len = this->_len;
      for (int i = 0; i < new_len; ++i)  // start from low-digit to high-digit
        diff_array[i] = sub(this->_array[i], target._array[i], borrow, borrow);
    } else {
      // a < b
      new_sign = true;
      new_len = target._len;
      for (int i = 0; i < new_len; ++i)  // start from low-digit to high-digit
        diff_array[i] = sub(target._array[i], this->_array[i], borrow, borrow);
    }
    for (; diff_array[new_len - 1] == '0'; --new_len);  // find new length
  }

  // assign to itself instead of create a new instance
  this->_sign = new_sign;
  for (int i = this->_len - 1; i >= 0; --i)
    this->_array[i] = diff_array[i];
  this->_len = new_len;

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

void modular_exp(BigNumber &y, const BigNumber &a, const BigNumber &m,
                 const BigNumber &n) {
  /* Compute y = (a ^ m) % n */
  BigNumber a_copy, m_copy;
  a_copy.copy(a);
  m_copy.copy(m);
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

BigNumber& BigNumber::operator/=(const BigNumber &target) {
  bool new_sign = this->_sign ^ target._sign;
  int new_len;
  char quotient_array[N];
  for (int i = N - 1; i >= 0; --i)  quotient_array[i] = '0';
  BigNumber target_copy;
  target_copy.copy(target);
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
  target_copy.copy(target);

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
  if (this->_sign)  // if it is negative
    cout << "-";
  for (int i = this->_len - 1; i >= 0; --i)
    cout << this->_array[i];
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
  for (int i = 0; i < this->_len - x; ++i)
    this->_array[i] = this->_array[i + x];
  for (int i = this->_len - x; i < this->_len; ++i)
    this->_array[i] = '0';
}

inline bool BigNumber::is_even() {
  return HEX2DEC(this->_array[0]) % 2 == 0;
}

inline void BigNumber::copy(const BigNumber &target) {
  this->_len = target._len;
  for (int i = this->_len - 1; i >= 0; --i)
    this->_array[i] = target._array[i];
}

inline void BigNumber::set_as(const string &array) {
  this->_sign = array[0] == '-';
  int _len = 0;
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

inline void BigNumber::last_bit_minus() {
  this->_array[0] = DEC2HEX(HEX2DEC(this->_array[0]) - 1);
}

inline void BigNumber::update_len(const int l) {
  this->_len += l;
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
    n_copy.copy(n);
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

  c.copy(m);
  c.rotate_left(4);
  c.update_len(4);
  c.padding(m, 4);
  c *= c;
  c %= key;
  return c;
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
  cout << "<Rabin Encryption" << endl;
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

  return 0;
}
