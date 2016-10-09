#include <stdio.h>

int main() {
  int c;
  int len;
  int cols;

  len = 0;
  cols = 12;

  while ((c = fgetc(stdin)) != EOF) {
    ++len;
    if (len % cols == 0) {
      printf("0x%02x,\n", (unsigned)c);
    } else {
      printf("0x%02x, ", (unsigned)c);
    }
  }
  printf("0x00\n"); /* put terminating zero */
  printf("len: %d", len);
}

/* vi:set ft=c ts=2 sw=2 et fdm=marker: */
