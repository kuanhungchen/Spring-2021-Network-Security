#include <iostream>

using namespace std;

#define printHex(x) printf("%02x", x);

const int Nr = 10;  // number of rounds
const int Nk = 4;  // number of columns in key
const int Nb = 4;  // number of columns in state
const uint8_t transform_matrix[] = {  // matrix for ByteSub
  1, 0, 0, 0, 1, 1, 1, 1,
  1, 1, 0, 0, 0, 1, 1, 1,
  1, 1, 1, 0, 0, 0, 1, 1,
  1, 1, 1, 1, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 0, 0, 0,
  0, 1, 1, 1, 1, 1, 0, 0,
  0, 0, 1, 1, 1, 1, 1, 0,
  0, 0, 0, 1, 1, 1, 1, 1
};

void printState(int r, uint8_t **state) {
  cout << "Round " << r << ":" << endl;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < Nb; ++j) {
      printHex(state[i][j]);
      if (!(i == 3 && j == 3)) cout << " ";
      else cout << endl;
    }
  }
}

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
  uint16_t r0 = 0x11b, r1 = a;

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
  if (t0 >= 0x100) t0 ^= 0x11b;
  return t0;
}

uint8_t AffineTransform(uint8_t x) {
  uint8_t y = 0x63;
  uint8_t inv_x = GF256_inv(x, 0x1b);
  for (int x_idx = 0; x_idx < 8; ++x_idx) {
    if (x & (1 << x_idx)) {
      for (int y_idx = 0; y_idx < 8; ++y_idx)
        if (transform_matrix[x_idx + 8 * y_idx])
          y ^= (1 << y_idx);
    }
  }
  return y;
}

void AddRoundKey(uint8_t **state, uint8_t *round_key) {
  for (int i= 0; i < 4; ++i)
    for (int j = 0; j < Nb; ++j)
      state[i][j] ^= round_key[i + 4 * j];
}

void ShiftRow(uint8_t **state) {
  uint8_t *tmp = new uint8_t[Nb];
  int C1, C2, C3;
  switch(Nb) {
    case 4:
      C1 = 1; C2 = 2; C3 = 3;
      break;
    case 6:
      C1 = 1; C2 = 2; C3 = 3;
      break;
    case 8:
      C1 = 1; C2 = 3; C3 = 4;
      break;
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

void ByteSub(uint8_t **state) {
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < Nb; ++j)
      state[i][j] = AffineTransform(state[i][j]);
}

void MixColumns(uint8_t **state) {
  uint8_t *tmp = new uint8_t[4];
  uint8_t a, b, c, d, mx=0x1b;

  for (int col = 0; col < 4; ++col) {
    a = GF256_mult(state[0][col], 0x02, mx);
    b = GF256_mult(state[1][col], 0x03, mx);
    c = GF256_mult(state[2][col], 0x01, mx);
    d = GF256_mult(state[3][col], 0x01, mx);
    tmp[0] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
    a = GF256_mult(state[0][col], 0x01, mx);
    b = GF256_mult(state[1][col], 0x02, mx);
    c = GF256_mult(state[2][col], 0x03, mx);
    d = GF256_mult(state[3][col], 0x01, mx);
    tmp[1] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
    a = GF256_mult(state[0][col], 0x01, mx);
    b = GF256_mult(state[1][col], 0x01, mx);
    c = GF256_mult(state[2][col], 0x02, mx);
    d = GF256_mult(state[3][col], 0x03, mx);
    tmp[2] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
    a = GF256_mult(state[0][col], 0x03, mx);
    b = GF256_mult(state[1][col], 0x01, mx);
    c = GF256_mult(state[2][col], 0x01, mx);
    d = GF256_mult(state[3][col], 0x02, mx);
    tmp[3] = GF256_add(a, GF256_add(b, GF256_add(c, d, mx), mx), mx);
    for (int i = 0; i < 4; ++i)
      state[i][col] = tmp[i];
  }
  delete[] tmp;
}

void RotWord(uint8_t *word) {
  uint8_t tmp = word[0];
  for (int i = 0; i < 3; ++i) word[i] = word[i + 1];
  word[3] = tmp;
}

void SubWord(uint8_t *word) {
  for (int i = 0; i < 4; ++i)
    word[i] = AffineTransform(word[i]);
}

void Rcon(uint8_t *rcon, int r) {
  uint8_t tmp = 1;
  for (int i = 1; i < r; ++i)
    tmp = GF256_mult_x(tmp, 0x1b);
  rcon[0] = tmp;
  rcon[1] = rcon[2] = rcon[3] = 0;
}


void AES_Encrypt(uint8_t *Plaintext, uint8_t *Ciphertext, uint8_t *Key);
void AES_Decrypt(uint8_t *Plaintext, uint8_t *Ciphertext, uint8_t *Key);

int main(int argc, const char * argv[]) {

  uint8_t a = 0x95;
  printHex(GF256_inv(a, 0x1B));
  printHex(GF256_mult(0x95, GF256_inv(0x95, 0x1B), 0x1B));

  return 0;
}
