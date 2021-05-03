#include <iostream>

using namespace std;

class BigNumber {
  private:
    bool _sign;
    int _len;
    char *_array;
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
    BigNumber operator/(BigNumber&);  // modify itself to represent remainder
    BigNumber operator%(const BigNumber&);
    BigNumber& operator-=(const BigNumber&);  // prevent re-instancialize

    // helper functions
    void print() const;
    void rotate_left(const int);
    void rotate_right(const int);
    BigNumber div_and_mod(BigNumber&);  // divide and modulo can be calculated at the same time
    bool is_even();
};
