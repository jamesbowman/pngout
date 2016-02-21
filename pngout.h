#include <stdint.h>
#include <stdlib.h>

struct pngout {
  uint8_t output[64];   /* output buffer */
  size_t nout;          /* number of bytes to output */

  uint16_t w, h;        /* width, height */
  size_t bpl;           /* bytes per line */
  uint32_t crc32;       /* running CRC32 */
  uint16_t s1, s2;      /* Adler CRC sums */
  uint16_t i, j;        /* i : column, j : row */
};

void pngout_start(struct pngout *s, uint16_t width, uint16_t height);
void pngout_rgb(struct pngout *s, uint8_t r, uint8_t g, uint8_t b);
