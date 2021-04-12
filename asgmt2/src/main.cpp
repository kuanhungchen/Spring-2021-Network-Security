#include <iostream>

using namespace std;

#define printHex(x) printf("0x%02x", x);

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
const uint8_t inv_transform_matrix[] = {  // matrix for InvByteSub
  0, 0, 1, 0, 0, 1, 0, 1,
  1, 0, 0, 1, 0, 0, 1, 0,
  0, 1, 0, 0, 1, 0, 0, 1,
  1, 0, 1, 0, 0, 1, 0, 0,
  0, 1, 0, 1, 0, 0, 1, 0,
  0, 0, 1, 0, 1, 0, 0, 1,
  1, 0, 0, 1, 0, 1, 0, 0,
  0, 1, 0, 0, 1, 0, 1, 0
};

void printState(int r, uint8_t **state) {
  cout << "Round " << r << ":" << endl;
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < Nb; ++j) {
      printHex(state[j][i]);
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
    if (inv_x & (1 << x_idx)) {
      for (int y_idx = 0; y_idx < 8; ++y_idx)
        if (transform_matrix[x_idx + 8 * y_idx])
          y ^= (1 << y_idx);
    }
  }
  return y;
}

uint8_t InvAffineTransform(uint8_t x) {
  uint8_t y = 0;
  for (int i = 0; i < 8; ++i)
    x ^= (0x63 & (1 << i));
  for (int x_idx = 0; x_idx < 8; ++x_idx) {
    if (x & (1 << x_idx)) {
      for (int y_idx = 0; y_idx < 8; ++y_idx)
        if (inv_transform_matrix[x_idx + 8 * y_idx])
          y ^= (1 << y_idx);
    }
  }
  return GF256_inv(y, 0x1b);
}

void AddRoundKey(uint8_t **state, uint8_t *round_key) {
  for (int i= 0; i < 4; ++i)
    for (int j = 0; j < Nb; ++j)
      state[i][j] ^= round_key[i + 4 * j];
}

void ShiftRow(uint8_t **state) {
  uint8_t *tmp = new uint8_t[Nb];
  int C1 = 1, C2 = 2, C3 = 3;

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

void InvByteSub(uint8_t **state) {
  /* Use Affine Transform instead of table look-ups. */
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < Nb; ++j)
      state[i][j] = InvAffineTransform(state[i][j]);
}

void MixColumns(uint8_t **state) {
  uint8_t *tmp = new uint8_t[4];
  uint8_t a, b, c, d, mx = 0x1b;

  for (int col = 0; col < 4; ++col) {
    a = GF256_mult(state[0][col], 0x02, mx);
    b = GF256_mult(state[1][col], 0x03, mx);
    c = GF256_mult(state[2][col], 0x01, mx);
    d = GF256_mult(state[3][col], 0x01, mx);
    tmp[0] = a ^ b ^ c ^ d;
    a = GF256_mult(state[0][col], 0x01, mx);
    b = GF256_mult(state[1][col], 0x02, mx);
    c = GF256_mult(state[2][col], 0x03, mx);
    d = GF256_mult(state[3][col], 0x01, mx);
    tmp[1] = a ^ b ^ c ^ d;
    a = GF256_mult(state[0][col], 0x01, mx);
    b = GF256_mult(state[1][col], 0x01, mx);
    c = GF256_mult(state[2][col], 0x02, mx);
    d = GF256_mult(state[3][col], 0x03, mx);
    tmp[2] = a ^ b ^ c ^ d;
    a = GF256_mult(state[0][col], 0x03, mx);
    b = GF256_mult(state[1][col], 0x01, mx);
    c = GF256_mult(state[2][col], 0x01, mx);
    d = GF256_mult(state[3][col], 0x02, mx);
    tmp[3] = a ^ b ^ c ^ d;
    for (int i = 0; i < 4; ++i)
      state[i][col] = tmp[i];
  }
  delete[] tmp;
}

void InvMixColumns(uint8_t **state) {
  uint8_t *tmp = new uint8_t[4];
  uint8_t a, b, c, d, mx = 0x1b;

  for (int col = 0; col < 4; ++col) {
    a = GF256_mult(state[0][col], 0x0e, mx);
    b = GF256_mult(state[1][col], 0x0b, mx);
    c = GF256_mult(state[2][col], 0x0d, mx);
    d = GF256_mult(state[3][col], 0x09, mx);
    tmp[0] = a ^ b ^ c ^ d;
    a = GF256_mult(state[0][col], 0x09, mx);
    b = GF256_mult(state[1][col], 0x0e, mx);
    c = GF256_mult(state[2][col], 0x0b, mx);
    d = GF256_mult(state[3][col], 0x0d, mx);
    tmp[1] = a ^ b ^ c ^ d;
    a = GF256_mult(state[0][col], 0x0d, mx);
    b = GF256_mult(state[1][col], 0x09, mx);
    c = GF256_mult(state[2][col], 0x0e, mx);
    d = GF256_mult(state[3][col], 0x0b, mx);
    tmp[2] = a ^ b ^ c ^ d;
    a = GF256_mult(state[0][col], 0x0b, mx);
    b = GF256_mult(state[1][col], 0x0d, mx);
    c = GF256_mult(state[2][col], 0x09, mx);
    d = GF256_mult(state[3][col], 0x0e, mx);
    tmp[3] = a ^ b ^ c ^ d;
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

void XORWords(uint8_t *src1, uint8_t *src2, uint8_t *des) {
  for (int i = 0; i < 4; ++i)
    des[i] = src1[i] ^ src2[i];
}

void KeyExpand(uint8_t *Key, uint8_t *RoundKeys) {
  uint8_t *tmp = new uint8_t [4 * sizeof(uint8_t)];
  uint8_t *rcon = new uint8_t [4 * sizeof(uint8_t)];

  // first round: generate round key by original key
  for (int i = 0; i < 4; ++i)
    tmp[i] = Key[12 + i];

  RotWord(tmp);
  SubWord(tmp);
  Rcon(rcon, 1);
  XORWords(tmp, rcon, tmp);

  XORWords(Key, tmp, RoundKeys);
  XORWords(Key + 4 * sizeof(uint8_t),
          RoundKeys, RoundKeys + 4 * sizeof(uint8_t));
  XORWords(Key + 8 * sizeof(uint8_t),
          RoundKeys + 4 * sizeof(uint8_t), RoundKeys + 8 * sizeof(uint8_t));
  XORWords(Key + 12 * sizeof(uint8_t),
          RoundKeys + 8 * sizeof(uint8_t), RoundKeys + 12 * sizeof(uint8_t));

  // remaining rounds: generate round key by previous one
  for (int r = 2; r <= Nr; ++r) {
    for (int i = 0; i < 4; ++i)
      tmp[i] = RoundKeys[(r - 1 - 1) * 16 + 12 + i];

    RotWord(tmp);
    SubWord(tmp);
    Rcon(rcon, r);
    XORWords(tmp, rcon, tmp);

    XORWords(RoundKeys + ((r - 1 - 1) * 16) * sizeof(uint8_t),
            tmp, RoundKeys + ((r - 1) * 16) * sizeof(uint8_t));
    XORWords(RoundKeys + (4 + (r - 1 - 1) * 16) * sizeof(uint8_t),
            RoundKeys + ((r - 1) * 16),
            RoundKeys + (4 + (r - 1) * 16) * sizeof(uint8_t));
    XORWords(RoundKeys + (8 + (r - 1 - 1) * 16) * sizeof(uint8_t),
            RoundKeys + (4 + (r - 1) * 16),
            RoundKeys + (8 + (r - 1) * 16) * sizeof(uint8_t));
    XORWords(RoundKeys + (12 + (r - 1 - 1) * 16) * sizeof(uint8_t),
            RoundKeys + (8 + (r - 1) * 16),
            RoundKeys + (12 + (r - 1) * 16) * sizeof(uint8_t));
  }
  delete[] rcon;
  delete[] tmp;
}

void AES_Encrypt_Round(uint8_t **state, uint8_t *RoundKey) {
  ByteSub(state);
  ShiftRow(state);
  MixColumns(state);
  AddRoundKey(state, RoundKey);
}

void AES_Encrypt_Final(uint8_t **state, uint8_t *RoundKey) {
  ByteSub(state);
  ShiftRow(state);
  AddRoundKey(state, RoundKey);
}

void AES_Decrypt_Round(uint8_t **state, uint8_t *RoundKey) {
  InvShiftRow(state);
  InvByteSub(state);
  AddRoundKey(state, RoundKey);
  InvMixColumns(state);
}

void AES_Decrypt_Final(uint8_t **state, uint8_t *RoundKey) {
  InvShiftRow(state);
  InvByteSub(state);
  AddRoundKey(state, RoundKey);
}

void AES_Encrypt(uint8_t *Plaintext, uint8_t *Ciphertext, uint8_t *Key) {
  // dump plain text to state
  uint8_t **state = new uint8_t *[4];
  state[0] = new uint8_t [4 * Nb];
  for (int i = 1; i < 4; ++i)
    state[i] = state[i - 1] + Nb;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < Nb; ++j)
      state[i][j] = Plaintext[i + j * 4];

  // key expansion, generate (Nk * Nr) words round keys from original key
  uint8_t *RoundKeys = new uint8_t[Nk * Nr * 4 * sizeof(uint8_t)];
  KeyExpand(Key, RoundKeys);

  // actual encryption
  AddRoundKey(state, Key);
  for (int r = 1; r < Nr; ++r) {
    AES_Encrypt_Round(state, RoundKeys + (r - 1) * Nk * 4 * sizeof(uint8_t));
    printState(r, state);
  }
  AES_Encrypt_Final(state, RoundKeys + (Nr - 1) * Nk * 4 * sizeof(uint8_t));
  printState(Nr, state);

  // dump state to cipher text
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < Nb; ++j)
      Ciphertext[i + j * 4] = state[i][j];

  delete[] state[0];
  delete[] state;
}

void AES_Decrypt(uint8_t *Plaintext, uint8_t *Ciphertext, uint8_t *Key) {
  /* Actual function to perform AES decryption. */

  // dump cipher text to state
  uint8_t **state = new uint8_t*[4];
  state[0]= new uint8_t [4 * Nb];
  for (int i = 1; i < 4; ++i)
    state[i] = state[i - 1] + Nb;
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < Nb; ++j)
      state[i][j] = Ciphertext[i + j * 4];

  // key expansion
  uint8_t *RoundKeys = new uint8_t[Nk * Nr * 4 * sizeof(uint8_t)];
  KeyExpand(Key, RoundKeys);

  // Nr rounds decryption
  AddRoundKey(state, RoundKeys + (Nr - 1) * Nk * 4 * sizeof(uint8_t));
  for (int r = Nr - 1; r > 0; --r) {
    AES_Decrypt_Round(state, RoundKeys + (r - 1) * Nk * 4 * sizeof(uint8_t));
    printState(Nr - r, state);
  }
  AES_Decrypt_Final(state, Key);
  printState(Nr, state);

  // dump state to plain text
  for (int i = 0; i < 4; ++i)
    for (int j = 0; j < 4; ++j)
      Plaintext[i + j * 4] = state[i][j];

  delete[] state[0];
  delete[] state;
}

int main(int argc, const char * argv[]) {
  string a, b;
  uint8_t *key = new uint8_t[16 * sizeof(uint8_t)];
  uint8_t *plaintext = new uint8_t[16 * sizeof(uint8_t)];
  uint8_t *ciphertext = new uint8_t[16 * sizeof(uint8_t)];
  uint8_t *decrypted = new uint8_t[16 * sizeof(uint8_t)];

  // parse key and plain text from input
  cout << "KEY:" << endl;
  getline(cin, a);
  for (int i = 0; i < 16; ++i)
    key[i] = strtol(a.substr(i * 5, 4).c_str(), 0, 16);
  cout << endl;

  cout << "Enter plaintext to encrypt:" << endl;
  getline(cin, b);
  for (int i = 0; i < 16; ++i)
    plaintext[i] = strtol(b.substr(i * 5, 4).c_str(), 0, 16);

  // run AES encryption
  AES_Encrypt(plaintext, ciphertext, key);
  cout << "CipherText:" << endl;
  for (int i = 0; i < 16; ++i) {
    printHex(ciphertext[i]);
    if (i != 15) cout << " ";
  }
  cout << endl;
  cout << endl;

  // run AES decryption
  cout << "Decryption:" << endl;
  AES_Decrypt(decrypted, ciphertext, key);
  cout << "Decrypted message in hex:" << endl;
  for (int i = 0; i < 16; ++i) {
    printHex(decrypted[i]);
    if (i != 15) cout << " ";
  }
  cout << endl;

  return 0;
}
