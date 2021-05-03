#include <iostream>
#include <cstdlib>
#include "big_number.h"
#include <time.h>

using namespace std;

class MillerRabin {
  private:

  public:
    MillerRabin();
    ~MillerRabin();
    bool primality_test(BigNumber&, const int);
};

bool MillerRabin::primality_test(BigNumber &num, const int t) {
  BigNumber one("1");
  BigNumber two("2");
  /* if (!(num < two)) return false; */
  /* for (auto x : this->small_primes) { */

  /* } */

  /* int k = 0; */
  /* BigNumber m = num - one; */
  /* while (m.is_even()) { */
  /*   ++k; */
  /*   m.rotate_right(1); */
  /* } */

  /* for (int i = 1; i <= t; ++i) { */

  /* } */

  return false;

}

MillerRabin::MillerRabin() {
  cout << "Hello world!" << endl;
}

MillerRabin::~MillerRabin() {
  cout <<"Bye!" << endl;
}

int main(int argc, const char * argv[]) {
  MillerRabin miller_rabin;
  BigNumber num("17");
  miller_rabin.primality_test(num, 5);
  srand(time(0));
  printf("%d\n", rand());
  return 0;
}
