/*
 * This is a minimal encoder for uncompressed PNGs.
 *
 * It is based on this Python implementation:
 *   http://mainisusuallyafunction.blogspot.com/search/label/png
 */

#include "pngout.h"

static const uint32_t crc_table[16] = {
  0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
  0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
  0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
  0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

uint32_t crc_update(uint32_t crc, uint8_t data)
{
  uint8_t tbl_idx;
  tbl_idx = crc ^ (data >> (0 * 4));
  crc = crc_table[tbl_idx & 0x0f] ^ (crc >> 4);
  tbl_idx = crc ^ (data >> (1 * 4));
  crc = crc_table[tbl_idx & 0x0f] ^ (crc >> 4);
  return crc;
}

static void b8(struct pngout *s, uint8_t n)
{
  s->output[s->nout++] = n;
  s->crc32 = crc_update(s->crc32, n);
}

void adler(struct pngout *s, uint8_t n)
{
  b8(s, n);
  s->s1 = (s->s1 + n) % 65521;
  s->s2 = (s->s2 + s->s1) % 65521;
}

static void be32(struct pngout *s, uint32_t n)
{
  b8(s, n >> 24);
  b8(s, n >> 16);
  b8(s, n >> 8);
  b8(s, n);
}

static void le16(struct pngout *s, uint32_t n)
{
  b8(s, n);
  b8(s, n >> 8);
}

static void b8s(struct pngout *s, const uint8_t *b, size_t n)
{
  while (n--)
    b8(s, *b++);
}

void start_chunk(struct pngout *s, const char *name, uint32_t size)
{
  be32(s, size);
  s->crc32 = 0xffffffffUL;
  b8s(s, (const uint8_t*)name, 4);
}

void end_chunk(struct pngout *s)
{
  be32(s, 0xffffffffUL ^ s->crc32);
}

void finish(struct pngout *s)
{
  start_chunk(s, "IEND", 0);
  end_chunk(s);
}

void pngout_start(struct pngout *s, uint16_t width, uint16_t height)
{
  s->w = width;
  s->h = height;
  s->nout = 0;

  static uint8_t first[8] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a
  };
  b8s(s, first, sizeof(first));

  start_chunk(s, "IHDR", 13);
  be32(s, width);
  be32(s, height);
  b8(s, 8);
  b8(s, 2);
  b8(s, 0);
  b8(s, 0);
  b8(s, 0);
  end_chunk(s);

  s->bpl = 1 + 3 * width;
  start_chunk(s, "IDAT", 2 + height * (5 + s->bpl) + 4);
  b8(s, 0x78);
  b8(s, 0x01);
  s->i = 0;
  s->j = 0;
}

void pngout_rgb(struct pngout *s, uint8_t r, uint8_t g, uint8_t b)
{
  s->nout = 0;

  if (s->i == 0) {
    b8(s, (s->j + 1) == s->h);
    le16(s, 1 + 3 * s->w);
    le16(s, (1 + 3 * s->w) ^ 0xffff);
    s->s1 = 1;
    s->s2 = 0;
    adler(s, 0);   // line start
  }

  adler(s, r);
  adler(s, g);
  adler(s, b);
  s->i++;

  if (s->i == s->w) {
    be32(s, (s->s2 << 16) + s->s1);
    s->i = 0;
    if (++s->j == s->h) {
      end_chunk(s);
      finish(s);
    }
  }
}
