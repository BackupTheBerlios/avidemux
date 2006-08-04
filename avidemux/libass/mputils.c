#include "mputils.h"

#define APP_HOME_PATH ".avidemux"

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int font_fontconfig = 1;
char *font_name = NULL;

void my_mp_msg(int lvl, char *lvl_str, char *fmt, ...) {
	if(lvl > MSGL_V) return;
	printf("[ass] **%s**: ", lvl_str);
	va_list va;
	va_start(va, fmt);
	vprintf(fmt, va);
	va_end(va);
}

unsigned utf8_get_char(char **str) {
  uint8_t *strp = (uint8_t *)*str;
  unsigned c = *strp++;
  unsigned mask = 0x80;
  int len = -1;
  while (c & mask) {
    mask >>= 1;
    len++;
  }
  if (len <= 0 || len > 4)
    goto no_utf8;
  c &= mask - 1;
  while ((*strp & 0xc0) == 0x80) {
    if (len-- <= 0)
      goto no_utf8;
    c = (c << 6) | (*strp++ & 0x3f);
  }
  if (len)
    goto no_utf8;
  *str = (char *)strp;
  return c;

no_utf8:
  strp = (uint8_t *)*str;
  c = *strp++;
  *str = (char *)strp;
  return c;
}

char *get_path(char *filename) {
	char *home = getenv("HOME");
	assert(home);
	char *ret = (char*)malloc(strlen(home) + strlen(APP_HOME_PATH) + strlen(filename) + 3);
	
	sprintf(ret, "%s/%s/%s", home, APP_HOME_PATH, filename);
	
	return ret;
}

