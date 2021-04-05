#include <iostream>

using namespace std;

#define printHex(x) printf("%02x\n", x);

// number of rounds
Nr = 10;
// number of columns of Cipher Key
Nk = 4;
// block length divided by 32
Nb = 4;

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

void AddRoundKey(uint8_t **state, uint8_t *key) {
  for (int i= 0; i < 4; ++i)
    for (int j = 0; j < Nb; ++j)
      state[i][j] ^= key[i + 4 * j];
}

void ShiftRows(uint8_t **state) {
  uint8_t *tmp = new uint8_t[Nb];
  int C1, C2, C3;
  switch(Nb) {
    case 4:
      C1 = 1; C2 = 2; C3 = 3;
      break
    case 6:
      C1 = 1; C2 = 2; C3 = 3;
      break;
    case 8:
      C1 = 1; C2 = 3; C3 = 4;
      brea;
    default:
      break;
  }

  // first row doesn't shift
  // second row shift by C1 bytes
  for (int i = 0; i < Nb; ++i)
    tmp[i] = state[1][(i + C1) % Nb];
  memcpy(state[1], tmp, Nb * sizeof(uint8_t));
  // third row shift by C2 bytes
  for (int i = 0; i < Nb; ++i)
    tmp[i] = state[2][(i + C2) % Nb];
  memcpy(state[2], tmp, Nb * sizeof(uint8_t));
  // last row shift by C3 bytes
  for (int i = 0; i < Nb; ++i)
    tmp[i] = state[3][(i + C3) % Nb];
  memcpy(state[3], tmp, Nb * sizeof(uint8_t));

  delete[] tmp;
}

void MixColumns(uint8_t **state) {
  uint8_t *tmp = new uint8_t[4];
  uint8_t a, b, c, d;

  // first column
  a = GF256_mult_x(state[0][0], 2, mx);
  b = GF256_mult_x(state[1][0], 3, mx);
  c = GF256_mult_x(state[2][0], 1, mx);
  d = GF256_mult_x(state[3][0], 1, mx);
  tmp[0] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  a = GF256_mult_x(state[0][0], 1, mx);
  b = GF256_mult_x(state[1][0], 2, mx);
  c = GF256_mult_x(state[2][0], 3, mx);
  d = GF256_mult_x(state[3][0], 1, mx);
  tmp[1] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  a = GF256_mult_x(state[0][0], 1, mx);
  b = GF256_mult_x(state[1][0], 1, mx);
  c = GF256_mult_x(state[2][0], 2, mx);
  d = GF256_mult_x(state[3][0], 3, mx);
  tmp[2] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  a = GF256_mult_x(state[0][0], 3, mx);
  b = GF256_mult_x(state[1][0], 1, mx);
  c = GF256_mult_x(state[2][0], 1, mx);
  d = GF256_mult_x(state[3][0], 2, mx);
  tmp[3] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  for (int i = 0; i < 4; ++i)
    state[i][0] = tmp[i];

  // second column
  a = GF256_mult_x(state[0][1], 2, mx);
  b = GF256_mult_x(state[1][1], 3, mx);
  c = GF256_mult_x(state[2][1], 1, mx);
  d = GF256_mult_x(state[3][1], 1, mx);
  tmp[0] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  a = GF256_mult_x(state[0][1], 1, mx);
  b = GF256_mult_x(state[1][1], 2, mx);
  c = GF256_mult_x(state[2][1], 3, mx);
  d = GF256_mult_x(state[3][1], 1, mx);
  tmp[1] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  a = GF256_mult_x(state[0][1], 1, mx);
  b = GF256_mult_x(state[1][1], 1, mx);
  c = GF256_mult_x(state[2][1], 2, mx);
  d = GF256_mult_x(state[3][1], 3, mx);
  tmp[2] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  a = GF256_mult_x(state[0][1], 3, mx);
  b = GF256_mult_x(state[1][1], 1, mx);
  c = GF256_mult_x(state[2][1], 1, mx);
  d = GF256_mult_x(state[3][1], 2, mx);
  tmp[3] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  for (int i = 0; i < 4; ++i)
    state[i][1] = tmp[i];

  // third column
  a = GF256_mult_x(state[0][2], 2, mx);
  b = GF256_mult_x(state[1][2], 3, mx);
  c = GF256_mult_x(state[2][2], 1, mx);
  d = GF256_mult_x(state[3][2], 1, mx);
  tmp[0] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  a = GF256_mult_x(state[0][2], 1, mx);
  b = GF256_mult_x(state[1][2], 2, mx);
  c = GF256_mult_x(state[2][2], 3, mx);
  d = GF256_mult_x(state[3][2], 1, mx);
  tmp[1] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  a = GF256_mult_x(state[0][2], 1, mx);
  b = GF256_mult_x(state[1][2], 1, mx);
  c = GF256_mult_x(state[2][2], 2, mx);
  d = GF256_mult_x(state[3][2], 3, mx);
  tmp[2] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  a = GF256_mult_x(state[0][2], 3, mx);
  b = GF256_mult_x(state[1][2], 1, mx);
  c = GF256_mult_x(state[2][2], 1, mx);
  d = GF256_mult_x(state[3][2], 2, mx);
  tmp[3] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  for (int i = 0; i < 4; ++i)
    state[i][2] = tmp[i];

  // last column
  a = GF256_mult_x(state[0][3], 2, mx);
  b = GF256_mult_x(state[1][3], 3, mx);
  c = GF256_mult_x(state[2][3], 1, mx);
  d = GF256_mult_x(state[3][3], 1, mx);
  tmp[0] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  a = GF256_mult_x(state[0][3], 1, mx);
  b = GF256_mult_x(state[1][3], 2, mx);
  c = GF256_mult_x(state[2][3], 3, mx);
  d = GF256_mult_x(state[3][3], 1, mx);
  tmp[1] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  a = GF256_mult_x(state[0][3], 1, mx);
  b = GF256_mult_x(state[1][3], 1, mx);
  c = GF256_mult_x(state[2][3], 2, mx);
  d = GF256_mult_x(state[3][3], 3, mx);
  tmp[2] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  a = GF256_mult_x(state[0][3], 3, mx);
  b = GF256_mult_x(state[1][3], 1, mx);
  c = GF256_mult_x(state[2][3], 1, mx);
  d = GF256_mult_x(state[3][3], 2, mx);
  tmp[3] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
  for (int i = 0; i < 4; ++i)
    state[i][3] = tmp[i];

  delete[] tmp;
}


void AES_Encrypt(uint8_t *Plaintext, uint8_t *Ciphertext, uint8_t *Key);
void AES_Decrypt(uint8_t *Plaintext, uint8_t *Ciphertext, uint8_t *Key);

int main(int argc, const char * argv[]) {

  uint8_t a = 0x95;
  printHex(GF256_inv(a, 0x1B));
  printHex(GF256_mult(0x95, GF256_inv(0x95, 0x1B), 0x1B));

  return 0;
}
