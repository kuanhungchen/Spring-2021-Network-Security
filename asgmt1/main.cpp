#include <iostream>

using namespace std;

#define N 20000

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
    bool operator>(const BigNumber&);
    bool operator==(const BigNumber&);
    bool operator<(const BigNumber&);
    BigNumber operator+(const BigNumber&);
    BigNumber operator-(const BigNumber&);
    BigNumber operator*(const BigNumber&);
    BigNumber operator/(const BigNumber&);
    BigNumber operator%(const BigNumber&);
    BigNumber& operator-=(const BigNumber&);  // to prevent re-instancialize

    // helper methods
    void print();
    void rotate_left();
    void recycle() {delete this;}
};

BigNumber::BigNumber() {
  this->_sign = false;
  this->_len = 0;
  this->_array = new char[N];
  for (int i = 0; i < N; ++i)
    this->_array[i] = '0';
}

BigNumber::BigNumber(const string& array) {
  this->_sign = array[0] == '-';
  this->_len = this->_sign ? array.size() - 1 : array.size();
  this->_array = new char[N];
  for (int i = N - 1; i >= this->_len; --i)
    this->_array[i] = '0';
  for (int i = this->_len - 1; i >= 0; --i)
    this->_array[i] = array[this->_len - i - 1];
}

BigNumber::BigNumber(const bool sign, const int len, const char *array) {
  this->_sign = sign;
  this->_len = len;
  this->_array = new char[N];
  for (int i = N - 1; i >= this->_len; --i)
    this->_array[i] = '0';
  for (int i = this->_len - 1; i >= 0; --i)
    this->_array[i] = array[i];
}

BigNumber::~BigNumber() {
  delete [] this->_array;
}

bool BigNumber::operator>(const BigNumber &target) {
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

bool BigNumber::operator==(const BigNumber &target) {
  // only care about magnitude
  bool res;
  if (this->_len != target._len) {
    res = false;
  } else {
    int i;
    for (i = this->_len; i > 0 && this->_array[i - 1] == target._array[i - 1]; --i);
    res = (i == 0);
  }
  return res;
}

bool BigNumber::operator<(const BigNumber &target) {
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

BigNumber BigNumber::operator+(const BigNumber &target) {
  bool new_sign = false;
  int new_len = max(this->_len, target._len);
  char sum_array[N], carry = '0';
  for (int i = 0; i < N; ++i) sum_array[i] = '0';

  for (int i = 0; i <= new_len; ++i)
    sum_array[i] = add(this->_array[i], target._array[i], carry, carry);
  if (sum_array[new_len] != '0') ++new_len;

  BigNumber sum(new_sign, new_len, sum_array);
  return sum;
}

BigNumber BigNumber::operator-(const BigNumber &target) {
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
      for (int i = 0; i < new_len; ++i)
        diff_array[i] = sub(this->_array[i], target._array[i], borrow, borrow);
    } else {
      // a < b
      new_sign = true;
      new_len = target._len;
      for (int i = 0; i < new_len; ++i)
        diff_array[i] = sub(target._array[i], this->_array[i], borrow, borrow);
    }
    for (; diff_array[new_len - 1] == '0'; --new_len);
  }

  BigNumber diff(new_sign, new_len, diff_array);
  return diff;
}

BigNumber BigNumber::operator*(const BigNumber &target) {
  bool new_sign = this->_sign ^ target._sign;
  int new_len;
  char product_array[N], carry;
  for (int i = 0; i < N; ++i) product_array[i] = '0';

  for (int i = 0; i < target._len; ++i) {
    if (target._array[i] == '0') continue;  // ignore when the bit is zero
    for (int j = 0; j < this->_len; ++j) {
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
      product_array[new_len - 1] == '0'; --new_len);
  if (new_len == 0) {
    // if result is zero
    new_sign = false;
    new_len = 1;
    product_array[0] = '0';
  }

  BigNumber product(new_sign, new_len, product_array);
  return product;
}

BigNumber BigNumber::operator/(const BigNumber &target) {
  bool new_sign = this->_sign ^ target._sign;
  int new_len = 0;
  char quotient_array[N];
  char remainder_array[N];
  for (int i = 0; i < N; ++i) {
    quotient_array[i] = '0';
    remainder_array[i] = '0';
  }

  BigNumber remainder;
  for (int cur_idx = this->_len - 1, cur_val = 0; cur_idx >= 0; --cur_idx, cur_val = 0) {
    remainder.rotate_left();  // remainder *= 16
    remainder._array[0] = this->_array[cur_idx];
    while (remainder > target || remainder == target) {
      ++cur_val;  // increment quotient when target is larger than remainder
      remainder -= target;  // use new operator to prevent re-instancialize
    }
    quotient_array[cur_idx] = DEC2HEX(cur_val);
    if (new_len != 0 || (new_len == 0 && cur_val != 0)) ++new_len;
  }
  if (new_len == 0) {
    // if result is zero
    new_sign = false;
    new_len = 1;
    quotient_array[0] = '0';
  }

  BigNumber quotient(new_sign, new_len, quotient_array);
  return quotient;
}

BigNumber BigNumber::operator%(const BigNumber &target) {
  // similar to division but now can ignore quotient
  bool new_sign = this->_sign ^ target._sign;
  int new_len = 0;
  char remainder_array[N];
  for (int i = 0; i < N; ++i) {
    remainder_array[i] = '0';
  }

  BigNumber remainder;
  for (int cur_idx = this->_len - 1, cur_val = 0; cur_idx >= 0; --cur_idx, cur_val = 0) {
    remainder.rotate_left();  // remainder *= 16
    remainder._array[0] = this->_array[cur_idx];
    while (remainder > target || remainder == target) {
      ++cur_val;
      remainder -= target;
    }
    if (new_len != 0 || (new_len == 0 && cur_val != 0)) ++new_len;
  }

  return remainder;
}

BigNumber& BigNumber::operator-=(const BigNumber &target) {
  // add this operator to prevent re-instancialize
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
      for (int i = 0; i < new_len; ++i)
        diff_array[i] = sub(_array[i], target._array[i], borrow, borrow);
    } else {
      // a < b
      new_sign = true;
      new_len = target._len;
      for (int i = 0; i < new_len; ++i)
        diff_array[i] = sub(target._array[i], this->_array[i], borrow, borrow);
    }
    for (; diff_array[new_len - 1] == '0'; --new_len);
  }
  this->_sign = new_sign;
  this->_len = new_len;
  for (int i = this->_len - 1; i >= 0; --i)
    this->_array[i] = diff_array[i];

  return *this;
}

inline void BigNumber::print() {
  if (this->_sign)
    cout << "-";
  for (int i = this->_len - 1; i >= 0; --i)
    cout << this->_array[i];
  cout << endl;
}

inline void BigNumber::rotate_left() {
  for (int i = min(N - 1, this->_len); i > 0; --i)
    this->_array[i] = this->_array[i - 1];
  this->_array[0] = '0';
  this->_len = min(this->_len + 1, N);
}

int main(int argc, const char * argv[]) {
  string a, b;
  cout << "a= ";
  cin >> a;
  cout << "b= ";
  cin >> b;

  BigNumber big_num_a(a);
  BigNumber big_num_b(b);

  BigNumber big_num_c = big_num_a + big_num_b;
  cout << "a+b = ";
  big_num_c.print();
  BigNumber big_num_d = big_num_a - big_num_b;
  cout << "a-b = ";
  big_num_d.print();
  BigNumber big_num_e = big_num_a * big_num_b;
  cout << "a*b = ";
  big_num_e.print();
  BigNumber big_num_f = big_num_a / big_num_b;
  cout << "a/b = ";
  big_num_f.print();
  BigNumber big_num_g = big_num_a % big_num_b;
  cout << "a%b = ";
  big_num_g.print();

  return 0;
}
