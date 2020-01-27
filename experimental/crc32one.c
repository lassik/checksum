#include <stdint.h>
#include <stdio.h>

uint32_t crc_table[256];

void make_crc_table(void) {
  unsigned long POLYNOMIAL = 0xedb88320;
  unsigned long crc;
  unsigned char b = 0;
  do {
    // Start with the data byte
    crc = b;
    for (unsigned long bit = 8; bit > 0; --bit) {
      if (crc & 1)
        crc = (crc >> 1) ^ POLYNOMIAL;
      else
        crc = (crc >> 1);
    }
    crc_table[(size_t)b] = crc;
  } while (0 != ++b);
}

int main(void) {
  make_crc_table();
  for (int i = 0; i < 256; i++) {
    printf("%lx\n", (unsigned long)(crc_table[i]));
  }
  return 0;
}
