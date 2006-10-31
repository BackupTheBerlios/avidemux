#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "default.h"
#include "math.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_image.h"

#include "ADM_print_priv.h"
// Borrowed from decomb
// Borrowed from the author of IT.dll, whose name I
// could not determine.

void drawDigit(ADMImage *dst, int x, int y, int num) 
{

	x = x * 10;
	y = y * 20;

	int pitch = dst->_width;
	for (int tx = 0; tx < 10; tx++) {
		for (int ty = 0; ty < 20; ty++) {
			unsigned char *dp = YPLANE(dst);
                        dp+=(y+ty)*pitch+(x+tx)*2;
//&dst->GetWritePtr()[(x + tx) * 2 + (y + ty) * pitch];
			if (font[num][ty] & (1 << (15 - tx))) {
				if (tx & 1) {
					dp[0] = 250;
					dp[-1] = 128;
					dp[1] = 128;
				} else {
					dp[0] = 250;
					dp[1] = 128;
					dp[3] = 128;
				}
			} else {
				if (tx & 1) {
					dp[0] = (unsigned char) ((dp[0] * 3) >> 2);
					dp[-1] = (unsigned char) ((dp[-1] + 128) >> 1);
					dp[1] = (unsigned char) ((dp[1] + 128) >> 1);
				} else {
					dp[0] = (unsigned char) ((dp[0] * 3) >> 2);
					dp[1] = (unsigned char) ((dp[1] + 128) >> 1);
					dp[3] = (unsigned char) ((dp[3] + 128) >> 1);
				}
			}
		}
	}
}


void drawString(ADMImage *dst, int x, int y, const char *s) 
{
	for (int xx = 0; *s; ++s, ++xx) {
		drawDigit(dst, x + xx, y, *s - ' ');
	}
}

