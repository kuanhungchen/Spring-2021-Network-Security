#include <iostream>

using namespace std;

#define N 100
#define max(a, b) a > b ? a : b

#define HEX2DEC(h) ({                             \
  int d;                                          \
  if (h >= '0' && h <= '9') {d = (int)(h - 48);}  \
  else {d = (int)(h - 97 + 10);}                  \
  d;                                              \
})

#define DEC2HEX(d) ({                             \
  char h;                                         \
  if ((int)d < 10) {h = (char)(48 + d);}          \
  else {h = (char)(97 + (d - 10));}               \
  h;                                              \
})

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
    BigNumber operator*(BigNumber&);
    BigNumber operator/(BigNumber&);
    BigNumber operator%(BigNumber&);

    // helper methods
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

BigNumber BigNumber::operator+(const BigNumber &target) {
  int new_len = max(_len, target._len);
  bool new_sign = false;
  char sum[N], carry = '0';
  for (int i = 0; i <= new_len; ++i) {
    sum[i] = add(_array[i], target._array[i], carry, carry);
  }
  if (sum[new_len] != '0') ++new_len;

  BigNumber res(new_sign, new_len, sum);

  return res;
}

BigNumber BigNumber::operator-(const BigNumber &target) {
  int new_len = 0;
  bool new_sign = false;
  char sum[N], borrow = '0';
  if (_len > target._len) {
    // a > b
    for (int i = 0; i < _len; ++i)
      sum[i] = sub(_array[i], target._array[i], borrow, borrow);
  } else if (_len < target._len) {
    // a < b
    new_sign = true;
    for (int i = 0; i < target._len; ++i)
      sum[i] = sub(target._array[i], _array[i], borrow, borrow);
  } else {
    for (new_len = _len; new_len > 0 && _array[new_len - 1] == target._array[new_len - 1]; --new_len);
    if (new_len != 0) {
      if (_array[new_len - 1] > target._array[new_len - 1]) {
        // a > b
        for (int i = 0; i < new_len; ++i)
          sum[i] = sub(_array[i], target._array[i], borrow, borrow);
      } else if (_array[new_len - 1] < target._array[new_len - 1]){
        // a < b
        new_sign = true;
        for (int i = 0; i < new_len; ++i)
          sum[i] = sub(target._array[i], _array[i], borrow, borrow);
      }
      for (; sum[new_len - 1] == '0'; --new_len);
    } else {
      new_len = 1;
      sum[0] = '0';
    }
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

  return 0;
}
