#include <stdint.h>
#include <stdlib.h>

struct pngout {
  uint16_t w, h;
  uint8_t output[64];
  size_t nout;
  size_t bpl;
  uint32_t crc32;
  uint16_t s1, s2;
  uint16_t i, j;
};

void pngout_start(struct pngout *s, uint16_t width, uint16_t height);
void pngout_rgb(struct pngout *s, uint8_t r, uint8_t g, uint8_t b);
