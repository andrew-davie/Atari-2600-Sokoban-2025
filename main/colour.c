#include <stdbool.h>

#include "defines.h"
#include "defines_cdfj.h"

#include "main.h"

#include "colour.h"
#include "random.h"
#include "score.h"
#include "scroll.h"

unsigned char bgPalette[22];
unsigned char fgPalette[2];
int flashTime = 1;

static int lastBgCol;

int roller;

void interleaveColour() {
	if (++roller > 2 || !enableICC)
		roller = 0;
}

#if ENABLE_SECAM

unsigned char secamConvert(unsigned char col) {

	unsigned char c = EXTERNAL(__XLATESECAM)[col >> 4];

	if ((col & 0xF) >= 4) {
		if (!c) // black -> white
			c = 0xE;
		else if (c == 2) // blue -> aqua
			c = 0xA;
	}
	return c;
}
#endif

int convertColour(unsigned char colour) {

	switch (mm_tv_type) {

#if ENABLE_SECAM
	case SECAM: {
		colour = secamConvert(colour);
		break;
	}
#endif

	case PAL:
	case PAL_60:
		if (!(colour & 1))
			colour = EXTERNAL(__XLATE)[colour >> 4] | (colour & 0xF);
		break;

	default:
		break;
	}

	return colour;
}

void setFlash2(unsigned char colour, int time) {
	ARENA_COLOUR = convertColour(colour);
	flashTime = time;
}

void doFlash() {

	if (!flashTime)
		return;

	if (flashTime < 0) {
		if (!++flashTime)
			ARENA_COLOUR = 1;

	} else if (!--flashTime)
		if (
#if ENABLE_SECAM
		    mm_tv_type == SECAM ||
#endif
		    (flashTime = ARENA_COLOUR-- & 0xF) == 1)
			ARENA_COLOUR = 1;
}

#define FRANGE (0x10000 / 22)
void setBackgroundPalette(const unsigned char *c) {

#if ENABLE_SECAM
	if (mm_tv_type == SECAM)
		for (int bgLine = 0; bgLine < 22; bgLine++)
			bgPalette[bgLine] = convertColour(c[2]);

	else
#endif
	{
		int c1 = c[2] & 0xF0;
		int c2 = c[3] & 0xF0;
		int i1 = c[2] & 0xE;
		int i2 = c[3] & 0xE;

		// A 6

		int system = c[2] & 1; // 1

		int cStep = ((c2 - c1)) * FRANGE; // 0
		int iStep = ((i2 - i1)) * FRANGE; // 0x400/22

		i1 = i1 << 16;
		c1 = c1 << 16;

		for (int bgLine = 0; bgLine < __BOARD_DEPTH; bgLine++) {

			bgPalette[bgLine] = convertColour(((c1 >> 16) & 0xF0) | ((i1 >> 16) & 0xE) | system);

			i1 += iStep;
			c1 += cStep;
		}
	}

	fgPalette[0] = convertColour(c[0]);
	fgPalette[1] = convertColour(c[1]);
}

void setPalette() {

	//  FLASH(0x9a, 2);
	interleaveColour();

	unsigned char bgCol = flashTime ? ARENA_COLOUR : 0;

	int i = 0;
	unsigned char *pfCol = RAM + _BUF_COLUPF;
	unsigned char *bkCol = RAM + _BUF_COLUBK;

	int bgCharLine = (scrollY[displayMode] >> EXTERNAL(__shiftMode)[displayMode]) * 3;
	int pfCharLine = 0;

	int size = EXTERNAL(__rowSize)[displayMode];
	while (bgCharLine >= size) {
		bgCharLine -= size;
		pfCharLine++;
	}

	if (bgCol != lastBgCol) {
		lastBgCol = bgCol;
		for (int j = 0; j < _ARENA_SCANLINES; j++)
			bkCol[j] = bgCol;
	}

	lastDisplayMode = displayMode;
	unsigned char rollColour[5];

	rollColour[0] = rollColour[3] = convertColour(fgPalette[1]);
	rollColour[1] = rollColour[4] = bgPalette[pfCharLine];
	rollColour[2] = convertColour(fgPalette[0]);

	int roll = roller;

	if (displayMode != DISPLAY_OVERVIEW) {

		int rollx = roll;

		if (!enableICC)
			rollx = 5;

		unsigned char cc0 = convertColour(EXTERNAL(__scoreColour)[rollx]);
		unsigned char cc1 = convertColour(EXTERNAL(__scoreColour)[rollx + 1]);
		unsigned char cc2 = convertColour(EXTERNAL(__scoreColour)[rollx + 2]);

		while (i < SCORE_SCANLINES) {

			pfCol[0] = cc0;
			pfCol[1] = cc1;
			pfCol[2] = cc2;

			pfCol += 3;
			i += 3;
		}
	}

	int r1 = roll + 1;
	int r2 = roll + 2;

	while (i < _ARENA_SCANLINES) {

		pfCol[0] = rollColour[roll];
		pfCol[1] = rollColour[r1];
		pfCol[2] = rollColour[r2];

		pfCol += 3;

		bgCharLine += 3;
		if (bgCharLine >= size) {
			bgCharLine = 0;
			rollColour[1] = rollColour[4] = bgPalette[++pfCharLine];
		}

		i += 3;
	}
}

void chooseColourScheme() {

	int paletteBase = (mm_tv_type == NTSC) ? 0 : __PALETTE_COUNT;
	currentPalette = rangeRandom(__PALETTE_COUNT) + paletteBase;
	currentPalette <<= 2;
}

// EOF