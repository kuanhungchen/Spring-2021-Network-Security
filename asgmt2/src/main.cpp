#include <iostream>

using namespace std;

#define printHex(x) printf("%02x\n", x);

uint8_t GF256_add(uint8_t a, uint8_t b, uint8_t mx) {
  return (a + b) ^ mx;
}

uint8_t GF256_mult_x(uint8_t a, uint8_t mx) {
  uint8_t highest_bit = a & 0x80;

  a <<= 1;
  return (highest_bit) ? a ^ mx : a;
}

uint8_t GF256_mult(uint8_t a, uint8_t b, uint8_t mx) {
  uint8_t c = 0;
  uint8_t highest_bit;

  for (int i = 0; i < 8; ++i) {
    if (b & 1) c ^= a;
    a = GF256_mult_x(a, mx);
    b >>= 1;
  }
  return c;
}

uint8_t GF256_inv(uint8_t a, uint8_t mx) {
  uint16_t t0 = 0, t1 = 1;
  uint16_t r0 = 0x11B, r1 = a;
  while (r1 != 0) {
    int8_t q, l0 = 0, l1 = 0;
    uint16_t tmp;

    // q = r0 / r1
    while ((1 << l0) - 1 < r0) ++l0;
    while ((1 << l1) - 1 < r1) ++l1;
    q = l0 - l1;

    // update r0, r1
    tmp = r0;
    r0 = r1;
    r1 = tmp ^ (r1 << q);

    // update t0, t1
    tmp = t0;
    t0 = t1;
    t1 = tmp ^ (t1 << q);
  }
  if (t0 >= 0x100) t0 ^= 0x11B;
  return t0;
}

int main(int argc, const char * argv[]) {

  uint8_t a = 0x95;
  printHex(GF256_inv(a, 0x1B));
  printHex(GF256_mult(0x95, GF256_inv(0x95, 0x1B), 0x1B));

  return 0;
}
