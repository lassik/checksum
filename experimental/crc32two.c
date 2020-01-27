#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t crc_table[256];

void make_crc_table(void) {
  /* make exclusive-or pattern from polynomial (0xedb88320) */
  static const unsigned char p[] = {0,  1,  2,  4,  5,  7,  8,
                                    10, 11, 12, 16, 22, 23, 26};
  uint32_t poly = 0;
  for (uint32_t n = 0; n < (int)(sizeof(p) / sizeof(unsigned char)); n++) {
    poly |= (uint32_t)1 << (31 - p[n]);
  }
  // poly = 0xedb88320;

  for (uint32_t n = 0; n < 256; n++) {
    uint32_t crc = n;
    for (uint32_t k = 0; k < 8; k++) {
      crc = crc & 1 ? poly ^ (crc >> 1) : crc >> 1;
    }
    crc_table[n] = crc;
  }
}

uint32_t crc32_byte(uint32_t crc, uint32_t byte) {
  return crc_table[(crc ^ byte) & 0xff] ^ (crc >> 8);
}

uint32_t crc32_file(const char *filename) {
  FILE *fp;
  int byte;
  uint32_t crc = 0;
  unsigned int i = 0;

  if (!(fp = fopen(filename, "rb")))
    exit(2);
  crc = ~crc;
  printf("byte -1 = %lx\n", (unsigned long)crc);
  while ((byte = fgetc(fp)) != EOF) {
    crc = crc32_byte(crc, byte);
    printf("byte %u = %lx\n", i++, (unsigned long)crc);
  }
  if (ferror(fp))
    exit(3);
  fclose(fp);
  return ~crc;
}

int main(void) {
  make_crc_table();
  for (int i = 0; i < 256; i++) {
    printf("%lx\n", (unsigned long)(crc_table[i]));
  }
  printf("%lx\n", (unsigned long)(crc32_file("crc.scm")));
  return 0;
}
