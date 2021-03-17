#include <iostream>

using namespace std;

#define N 20000

int max(const int a, const int b) {
  if (a > b) return a;
  return b;
}

int min(const int a, const int b) {
  if (a < b) return a;
  return b;
}

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

char add(const char a, const char b, const char c, char &carry) {
  /* addition for single bit */
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
  /* subtraction for single bit */
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
  /* multiplication for single bit */
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
    bool _sign;     // true means negative
    int _len;       // length of digits, sign exclusive
    char *_array;   // digits of big number, sign exclusive
  public:
    // constructor
    BigNumber();
    BigNumber(const string&);
    BigNumber(const bool, const int, const char*);

    // destructor
    ~BigNumber();

    // operators
    BigNumber operator+(const BigNumber&);
    BigNumber operator-(const BigNumber&);
    BigNumber operator*(const BigNumber&);
    BigNumber operator/(BigNumber&);
    BigNumber operator%(BigNumber&);

    // helper methods
    int BiggerThan(const BigNumber&);
    void Print();
    void SetSign(const bool sign) {_sign = sign;}
    void SetLen(const int len) {_len = len;}
    void SetArray(const char*);
};

BigNumber::BigNumber() {
  _sign = false;
  _len = 0;
  _array = new char[N];
  for (int i = 0; i < N; ++i)
    _array[i] = '0';
}

BigNumber::BigNumber(const bool sign, const int len, const char *array) {
  _sign = sign;
  _len = len;
  _array = new char[N];
  for (int i = N - 1; i >= _len; --i)
    _array[i] = '0';
  for (int i = _len - 1; i >= 0; --i)
    _array[i] = array[i];
}

BigNumber::BigNumber(const string& array) {
  _sign = array[0] == '-';
  _len = array.size() - _sign;
  _array = new char[N];
  for (int i = N - 1; i >= _len; --i)
    _array[i] = '0';
  for (int i = _len - 1; i >= 0; --i)
    _array[i] = array[_len - i - 1];
}

BigNumber::~BigNumber() {
  delete [] _array;
}

inline void BigNumber::Print() {
  if (_sign)
    cout << "-";
  for (int i = _len - 1; i >= 0; --i)
    cout << _array[i];
  cout << endl;
}

inline void BigNumber::SetArray(const char *array) {
  for (int i = 0; i < N; ++i)
    _array[i] = array[i];
}


int BigNumber::BiggerThan(const BigNumber &target) {
  int res = -2;  // 1: a > b, 0: a == b, -1: a < b
  if (_len > target._len) {
    res = 1;
  } else if (_len < target._len) {
    res = -1;
  } else {
    int i;
    for (i = _len; i > 0 && _array[i - 1] == target._array[i - 1]; --i);
    if (i == 0) res = 0;
    else if (_array[i - 1] > target._array[i - 1]) {
      res = 1;
    } else {
      res = -1;
    }
  }
  return res;
}

BigNumber BigNumber::operator+(const BigNumber &target) {
  bool new_sign = false;
  int new_len = max(_len, target._len);
  char sum[N], carry = '0';
  for (int i = 0; i < N; ++i) sum[i] = '0';

  for (int i = 0; i <= new_len; ++i)
    sum[i] = add(_array[i], target._array[i], carry, carry);
  if (sum[new_len] != '0') ++new_len;
  BigNumber res(new_sign, new_len, sum);

  return res;
}

BigNumber BigNumber::operator-(const BigNumber &target) {
  bool new_sign = false;
  int new_len;
  char sum[N], borrow = '0';
  for (int i = 0; i < N; ++i) sum[i] = '0';

  int cmp = BiggerThan(target);
  if (cmp == 0) {
    // a == b
    new_len = 1;
    sum[0] = '0';
  } else {
    if (cmp == 1) {
      // a > b
      new_len = _len;
      for (int i = 0; i < new_len; ++i)
        sum[i] = sub(_array[i], target._array[i], borrow, borrow);
    } else if (cmp == -1) {
      // a < b
      new_sign = true;
      new_len = target._len;
      for (int i = 0; i < new_len; ++i)
        sum[i] = sub(target._array[i], _array[i], borrow, borrow);
    }
    for (; sum[new_len - 1] == '0'; --new_len);
  }
  BigNumber res(new_sign, new_len, sum);

  return res;
}

BigNumber BigNumber::operator*(const BigNumber &target) {
  bool new_sign = false;
  int new_len;
  char sum[N], carry;
  for (int i = 0; i < N; ++i) sum[i] = '0';

  for (int i = 0; i < target._len; ++i) {
    if (target._array[i] == '0') continue;
    for (int j = 0; j < _len; ++j) {
      carry = mul(target._array[i], _array[j], sum[i + j], sum[i + j]);
      int x = 1;
      while (i + j + x < N && carry != '0') {
        sum[i + j + x] = add(sum[i + j + x], carry, '0', carry);
        ++x;
      }
    }
  }
  for (new_len = _len * target._len + 1; new_len > 0 &&
      sum[new_len - 1] == '0'; --new_len);
  if (new_len == 0) {
    new_len = 1;
    sum[0] = '0';
  }
  BigNumber res(new_sign, new_len, sum);

  return res;
}

int main(int argc, const char * argv[]) {
  string a = argv[1];
  string b = argv[2];

  //string a, b;
  //cout << "a= ";
  //cin >> a;
  //cout << "b= ";
  //cin >> b;

  BigNumber big_num_a(a);
  BigNumber big_num_b(b);
  //cout << "a= ";
  //big_num_a.Print();
  //cout << "b= ";
  //big_num_b.Print();

  BigNumber big_num_c = big_num_a + big_num_b;
  cout << "a+b = ";
  big_num_c.Print();
  BigNumber big_num_d = big_num_a - big_num_b;
  cout << "a-b = ";
  big_num_d.Print();
  BigNumber big_num_e = big_num_a * big_num_b;
  cout << "a*b = ";
  big_num_e.Print();

  return 0;
}
