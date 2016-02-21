#include <stdio.h>
#include "pngout.h"

int main()
{
  struct pngout s;

  FILE *f = fopen("new.png", "w");
  pngout_start(&s, 1, 2);
  fwrite(s.output, s.nout, 1, f);

  int i;
  for (i = 0; i < 2; i++) {
    pngout_rgb(&s, 0x55, 0x66, 0x77);
    fwrite(s.output, s.nout, 1, f);
  }

  return 0;
}

