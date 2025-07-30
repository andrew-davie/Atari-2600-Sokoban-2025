// clang-format off

#include <stdbool.h>

#include "defines.h"
#include "defines_cdfj.h"
#include "defines_from_dasm_for_c.h"

#include "main.h"
#include "drawscreen.h"

#include "animations.h"
#include "attribute.h"
#include "characterset.h"
#include "colour.h"
#include "menu.h"
#include "player.h"
#include "random.h"
#include "scroll.h"
#include "sound.h"
#include "swipe.h"
#include "rooms.h"

#include "score.h"
extern const unsigned char iCC_title[];
int startup;

// clang-format on

const unsigned char *img[_1ROW];

static unsigned char *const arenas[] = {
    RAM + _BUF_PF0_LEFT + SCORE_SCANLINES,
    RAM + _BUF_PF0_RIGHT + SCORE_SCANLINES,
};

static const unsigned char rollDirect[][CHAR_HEIGHT] = {
    {2,  0,  1,  5,  3,  4,  8,  6,  7,  11, 9,  10, 14, 12,
     13, 17, 15, 16, 20, 18, 19, 23, 21, 22, 26, 24, 25},
    {0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13,
     14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26},
    {1,  2,  0,  4,  5,  3,  7,  8,  6,  10, 11, 9,  13, 14,
     12, 16, 17, 15, 19, 20, 18, 22, 23, 21, 25, 26, 24},
};

extern int roller;

#define SPK_N 2
static unsigned char char_sparkle[SPK_N * CHAR_HEIGHT];
const unsigned char *p;

void grab(int size) {

	if (sparkleTimer) {

		for (int col = 0; col < size; col++) {
			unsigned char p2 = GET(p[col]);
			int type = CharToType[p2];
			if (Attribute[type] & (ATT_MANYBLANK | ATT_EXIT)) {
				rndX = (rndX >> 31) | (rndX << 1);
				img[col] = char_sparkle + (((rndX >> 16) * (SPK_N - 1) * CHAR_HEIGHT) >> 16);
			} else {
				if (Animate[type])
					p2 = *Animate[type];
				img[col] = charSet[p2].big;
			}
		}

	}

	else {
		for (int col = 0; col < size; col++) {
			unsigned char p2 = GET(p[col]);
			int type = CharToType[p2];
			if (Animate[type])
				p2 = *Animate[type];
			img[col] = charSet[p2].big;
		}

		p += size;
	}
}

void grabSmall(int size) {

	// if (sparkleTimer) {

	// 	for (int col = 0; col < size; col++) {
	// 		unsigned char p2 = GET(p[col]);
	// 		int type = CharToType[p2];
	// 		if (Attribute[type] & (ATT_MANYBLANK | ATT_EXIT)) {
	// 			rndX = (rndX >> 31) | (rndX << 1);
	// 			img[col] = char_sparkle + (((rndX >> 16) * (SPK_N - 1) * CHAR_HEIGHT) >> 16);
	// 		} else {
	// 			if (Animate[type])
	// 				p2 = *Animate[type];
	// 			img[col] = charSet[p2].small;
	// 		}
	// 	}

	// }

	// else {
	for (int col = 0; col < size; col++) {
		unsigned char p2 = GET(p[col]);
		int type = CharToType[p2];
		if (Animate[type])
			p2 = *Animate[type];
		img[col] = charSet[p2].small;
	}

	p += size;
	// }
}

#if ENABLE_SHAKE

extern int shakeX, shakeY;

void getShakenCoords(int *x, int *y) {

	// TODO: should this trim by the screen dims (i.e., is it scroll limits)

	int tempX = scrollX[displayMode] + shakeX;
	if (tempX < 0)
		tempX = 0;
	if (tempX >= _1ROW << (SHIFT_SCROLLX + 2))
		tempX = (_1ROW << (SHIFT_SCROLLX + 2)) - 1;

	int tempY = scrollY[displayMode] + shakeY;
	if (tempY < 0)
		tempY = 0;
	// if (tempY >= __BOARD_DEPTH << SHIFT_SCROLLY)
	//     tempY = ((__BOARD_DEPTH) << SHIFT_SCROLLY) - 1;

	*x = tempX;
	*y = tempY;
}
#endif

void sparkly() {

	for (int i = 0; i < SPK_N * CHAR_HEIGHT; i += 3)
		char_sparkle[i] = char_sparkle[i + 1] = char_sparkle[i + 2] = 1 << (getRandom32() & 7);
}

void drawScreen() { // 46391

	//    doSparkles();

#if ENABLE_SHAKE

	int x, y;
	getShakenCoords(&x, &y);

	int lcount = -(y >> SHIFT_SCROLLY) * 3;

	static int xtemp = 0;
	xtemp++;

	int frac = 0;
	int remainder = x >> SHIFT_SCROLLX;
	while (remainder >= PIXELS_PER_CHAR) {
		remainder -= PIXELS_PER_CHAR;
		frac++;
	}

	int shift = PIXELS_PER_CHAR - remainder;
#else
	int lcount = -(scrollY[0] >> SHIFT_SCROLLY) * 3;
	int shift = 4 - ((scrollX[0] >> SHIFT_SCROLLX) & 3);
#endif

	int startRow = (-lcount * (0x10000 / CHAR_HEIGHT)) >> 16;
	lcount += startRow * CHAR_HEIGHT;

#if ENABLE_SHAKE
	// int frac = 0; // tmpx >> (SHIFT_SCROLLX + 2);
#else
	int frac = 0; // scrollX >> (SHIFT_SCROLLX + 2);
#endif

	int scanline = 0;
	for (int row = startRow; scanline < SCANLINES; row++) {

		int height = SCANLINES - scanline < CHAR_HEIGHT ? SCANLINES - scanline : CHAR_HEIGHT;

		for (int half = 0; half < 2; half++) {

			p = ADDRESS_OF(row) + half * 4 + frac;
			grab(5);

			unsigned char *pf0 = arenas[half] + scanline;

			for (int y = -lcount; y < height; y++) {

				int lineColour = rollDirect[roller][y];

				int p = //((unsigned int)(img[0][lineColour]) << (32 - PIXELS_PER_CHAR) >>
				        // (32 - 6 * PIXELS_PER_CHAR)) |
				    ((unsigned int)(img[0][lineColour]) << (32 - PIXELS_PER_CHAR) >>
				     (32 - 5 * PIXELS_PER_CHAR)) |
				    ((unsigned int)(img[1][lineColour]) << (32 - PIXELS_PER_CHAR) >>
				     (32 - 4 * PIXELS_PER_CHAR)) |
				    ((unsigned int)(img[2][lineColour]) << (32 - PIXELS_PER_CHAR) >>
				     (32 - 3 * PIXELS_PER_CHAR)) |
				    ((unsigned int)(img[3][lineColour]) << (32 - PIXELS_PER_CHAR) >>
				     (32 - 2 * PIXELS_PER_CHAR)) |
				    ((unsigned int)(img[4][lineColour]) << (32 - PIXELS_PER_CHAR) >>
				     (32 - 1 * PIXELS_PER_CHAR));

				p >>= shift;

				*(pf0 + (_ARENA_SCANLINES << 1)) = BitRev[(unsigned char)p];
				*(pf0 + _ARENA_SCANLINES) = p >> 8;
				*pf0++ = BitRev[p >> 16];
			}
		}

		scanline += height + lcount;
		lcount = 0;
	}
}

void doSparkles() {
	if (sparkleTimer)
		sparkly();
}

void drawHalfScreen(int half) { // 17612 (A1)

	// static const unsigned char *const arenas[] = {
	//     RAM + _BUF_PF0_LEFT + SCORE_SCANLINES,
	//     RAM + _BUF_PF0_RIGHT + SCORE_SCANLINES,
	// };

#if ENABLE_SHAKE

	int x, y;
	getShakenCoords(&x, &y);

	// static int xx = 0;
	// xx += 1 << (SHIFT_SCROLLX - 3);
	// ;

	// x = xx;

	// if (half)
	// 	x += 1 << (SHIFT_SCROLLX + 2);

	int rem = (x >> (SHIFT_SCROLLX + 1)) + half * 2;

	int frac = 0;

	while (rem >= 3) {
		rem -= 3;
		frac++;
	}

	int lcount = -(y >> (SHIFT_SCROLLY + 1)) * 3;
	int shift = 2 - rem; // 2 - rem - half * 3; //((x >> (SHIFT_SCROLLX + 1)) & 1) - half * 2;
	                     //	int frac = x >> (SHIFT_SCROLLX + 2);

#else
	int lcount = -(scrollY >> (SHIFT_SCROLLY + 1)) * 3;
	int shift = 2 - ((scrollX >> (SHIFT_SCROLLX + 1)) & 1);
	int frac = scrollX >> (SHIFT_SCROLLX + 2);
#endif

	int rowOffset = (-lcount * (0x10000 / CHAR_HEIGHT_HALF)) >> 16;
	lcount += rowOffset * CHAR_HEIGHT_HALF;

	for (int row = 0, scanline = 0; scanline < SCANLINES; row++) {

		int height = SCANLINES - scanline;
		if (height > CHAR_HEIGHT_HALF)
			height = CHAR_HEIGHT_HALF;

		// for (int half = 0; half < 2; half++) {

		p = ADDRESS_OF(row + rowOffset) + half * 6 + frac;
		grabSmall(8);

		unsigned char *pf0 = arenas[half] + scanline;

		for (int y = -lcount; y < height; y++) {

			int lineColour = rollDirect[roller][y];

			// clang-format off

            // int p = ((img[0][lineColour] >> 5 << 19) |
            //          (img[1][lineColour] >> 5 << 16) |
            //          (img[2][lineColour] >> 5 << 13) |
            //          (img[3][lineColour] >> 5 << 10) |
            //          (img[4][lineColour] >> 5 << 7) |
            //          (img[5][lineColour] >> 5 << 4) |
            //          (img[6][lineColour] >> 5 << 1))
                     
            // char format:  AAA BBB C D
            // we want alternating AAA, BBB, AAA 

            int p = ((img[0][lineColour] >> 5 << 20)
                   | (((img[1][lineColour] >> 2) & 0x7) << 17)
                   | (img[2][lineColour] >> 5 << 14)
                   | (((img[3][lineColour] >> 2) & 0x7) << 11)
                   | (img[4][lineColour] >> 5 << 8)
                   | (((img[5][lineColour] >> 2) & 0x7) << 5)
                   | (img[6][lineColour] >> 5 << 2)
                   | (((img[7][lineColour] >> 2) & 0x7) >> 1)
                    )
                    >> shift;

			// clang-format on

			*(pf0 + (_ARENA_SCANLINES << 1)) = BitRev[(unsigned char)p];
			*(pf0 + _ARENA_SCANLINES) = p >> 8;
			*pf0++ = BitRev[p >> 16];
		}
		//}

		scanline += height + lcount;
		lcount = 0;
	}
}

void drawOverviewScreen(int startRow, int endRow) { // 35848 (A1)

	const unsigned char *rDirect = rollDirect[roller];

	unsigned char *ppf0 = RAM + _BUF_PF0_LEFT + startRow * 9;
	unsigned char *ppf1 = ppf0 + _ARENA_SCANLINES;
	unsigned char *ppf2 = ppf1 + _ARENA_SCANLINES;
	unsigned char *ppf3 = ppf2 + _ARENA_SCANLINES;
	unsigned char *ppf4 = ppf3 + _ARENA_SCANLINES;
	unsigned char *ppf5 = ppf4 + _ARENA_SCANLINES;

	p = ADDRESS_OF(startRow);

	unsigned char p2;
	unsigned char type;

	for (int row = startRow; row < endRow; row++) {

#if 1

		for (int col = 0; col < _1ROW; col++) {
			p2 = GET(p[col]);
			type = CharToType[p2];
			img[col] = charSet[Animate[type] ? *Animate[type] : p2].small;
		}

		p += _1ROW;
#else

		for (int col = 0; col < _1ROW; col++)
			img[col] = charSet[GET(p[col])];

		p += _1ROW;
#endif

		int s1 = row & 1;
		int s2 = s1 ^ 1;

		for (int iccLine = 0; iccLine < 9; iccLine++) {

			int rollColour = rDirect[iccLine];

			// clang-format off

            *ppf0++ = (img[0][rollColour] & 0b01) << 4 |
                      (img[1][rollColour] & 0b10) << 4 |
                      (img[2][rollColour] & 0b01) << 6 |
                      (img[3][rollColour] & 0b10) << 6;

            *ppf1++ = img[4][rollColour] >> 1 << 7 |
                      (unsigned int)img[5][rollColour] >> s1 << 31 >> 25 |
                      (unsigned int)img[6][rollColour] >> s2 << 31 >> 26 |
                      (unsigned int)img[7][rollColour] >> s1 << 31 >> 27 |
                      (unsigned int)img[8][rollColour] >> s2 << 31 >> 28 |
                      (unsigned int)img[9][rollColour] >> s1 << 31 >> 29 |
                      (unsigned int)img[10][rollColour] >> s2 << 31 >> 30 |
                      (unsigned int)img[11][rollColour] >> s1 << 31 >> 31;

            *ppf2++ = (unsigned int)img[12][rollColour] >> s2 << 31 >> 31 |
                      (unsigned int)img[13][rollColour] >> s1 << 31 >> 30 |
                      (unsigned int)img[14][rollColour] >> s2 << 31 >> 29 |
                      (unsigned int)img[15][rollColour] >> s1 << 31 >> 28 |
                      (unsigned int)img[16][rollColour] >> s2 << 31 >> 27 |
                      (unsigned int)img[17][rollColour] >> s1 << 31 >> 26 |
                      (unsigned int)img[18][rollColour] >> s2 << 31 >> 25 |
                      (unsigned int)img[19][rollColour] >> s1 << 7;

            *ppf3++ = (img[20][rollColour] & 0b01) << 4 |
                      (img[21][rollColour] & 0b10) << 4|
                      (img[22][rollColour] & 0b01) << 6 |
                      (img[23][rollColour] & 0b10) << 6;

            *ppf4++ = img[24][rollColour] >> 1 << 7 |
                      (unsigned int)img[25][rollColour] >> s1 << 31 >> 25 |
                      (unsigned int)img[26][rollColour] >> s2 << 31 >> 26 |
                      (unsigned int)img[27][rollColour] >> s1 << 31 >> 27 |
                      (unsigned int)img[28][rollColour] >> s2 << 31 >> 28 |
                      (unsigned int)img[29][rollColour] >> s1 << 31 >> 29 |
                      (unsigned int)img[30][rollColour] >> s2 << 31 >> 30 |
                      (unsigned int)img[31][rollColour] >> s1 << 31 >> 31;

            *ppf5++ = (unsigned int)img[32][rollColour] >> s2 << 31 >> 31 |
                      (unsigned int)img[33][rollColour] >> s1 << 31 >> 30 |
                      (unsigned int)img[34][rollColour] >> s2 << 31 >> 29 |
                      (unsigned int)img[35][rollColour] >> s1 << 31 >> 28 |
                      (unsigned int)img[36][rollColour] >> s2 << 31 >> 27 |
                      (unsigned int)img[37][rollColour] >> s1 << 31 >> 26 |
                      (unsigned int)img[38][rollColour] >> s2 << 31 >> 25|
                      (unsigned int)img[39][rollColour] >> s1 << 7;

			// clang-format on
		}
	}
}

bool drawBit(char x, int y, int colour) {

	int line = y * 3 + 21;
	if (line >= _ARENA_SCANLINES - 3 || line < SCORE_SCANLINES)
		return false;

	int col = x;
	if (col < 0 || col >= _1ROW)
		return false;

	unsigned char *base = _BUF_PF0_LEFT + RAM + line;

	if (col >= 20) {
		col -= 20;
		base += 3 * _ARENA_SCANLINES;
	}

	base += ((col + 4) >> 3) * _ARENA_SCANLINES;

	int shift = col < 4 ? col + 4 : col < 12 ? 11 - col : col - 12;
	int bit = 1 << shift;

	int roll = roller;
	for (int i = 0; i < 3; i++) {

		if (colour & (1 << roll))
			base[i] |= bit;
		else
			base[i] &= ~bit;

		if (++roll > 2)
			roll = 0;
	}

	//  }

	return true;
}

#define ICON_BASE 108

void initIconPalette() {

#if ENABLE_RAINBOW

	if (rainbow) {
		rollRainbow();
		doRainbowBackground();
	}

#endif

	unsigned char rollColours[3];

	int pal[3] = {0x44, 0x94, 0xD4};
	for (int i = 0; i < 3; i++)
		rollColours[i] = convertColour(startup ? palicc[menuIconPalette][i] : pal[i]);

	roller++;
	if (roller > 2)
		roller = 0; // tmp

	int baseRoller = roller;

	if (enableICC && ++baseRoller > 2)
		baseRoller = 0;

	unsigned char *pal0 = RAM + _BUF_MENU_COLUP0 + ICON_BASE - 1;

	for (int j = 0; j < __BOARD_DEPTH; j++) {
		for (int i = 0; i < 3; i++) {
			*pal0++ = rollColours[baseRoller];
			if (++baseRoller > 2)
				baseRoller = 0;
		}
	}
}

void drawIconScreen(int startRow, int endRow, bool staticx) { // --> 101102 cycles

	if (startup) {
		--startup;

		unsigned char *ppf = RAM + ICON_BASE + _BUF_MENU_GRP0A;

		unsigned char *pf1l = RAM + _BUF_MENU_PF1_LEFT;
		unsigned char *pf2l = RAM + _BUF_MENU_PF2_LEFT;
		unsigned char *pf2r = RAM + _BUF_MENU_PF1_RIGHT;
		unsigned char *pf1r = RAM + _BUF_MENU_PF2_RIGHT;

		int rl = 2; // roller;

		for (int i = 0; i < 198; i += 3) {

			rl++;
			if (rl > 2)
				rl = 0;
			*ppf = 0;
			*(ppf + _ARENA_SCANLINES) = pf1l[i + rl];
			*(ppf + _ARENA_SCANLINES * 2) = BitRev[pf2l[i + rl]];
			*(ppf + _ARENA_SCANLINES * 3) = pf1r[i + rl];
			*(ppf + _ARENA_SCANLINES * 4) = BitRev[pf2r[i + rl]];
			*(ppf + _ARENA_SCANLINES * 5) = 0;

			if (staticx) {

				for (int j = 0; j < 6; j++) {
					*(ppf + j * _ARENA_SCANLINES) |= getRandom32();
					*(ppf + j * _ARENA_SCANLINES) &= getRandom32();
				}
			}
			ppf++;
		}

		ppf = RAM + ICON_BASE + _BUF_MENU_GRP0A + 36;
		for (int i = 0; i < 22; i++) {
			*(ppf + i + _ARENA_SCANLINES * 3) =
			    *(ppf + i + _ARENA_SCANLINES * 3) | (getRandom32() & getRandom32());
			*(ppf + i + _ARENA_SCANLINES * 4) =
			    *(ppf + i + _ARENA_SCANLINES * 4) & 0xF | (getRandom32() & getRandom32() & 0xF0);
		}

	} else {
#define SHIFT 8
#define ICON_WIDTH 48
#define ICON_DEPTH 22
#define RESCALE(a) ((a << SHIFT) * 360 / 256 / ICON_WIDTH)

		static const short rescaleX[] = {

		    // converts 0-39 horizontal --> 0-47

		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),

		    // RESCALE(0),  RESCALE(1),  RESCALE(2),  RESCALE(3),  RESCALE(4),  RESCALE(5),
		    // RESCALE(6),
		    // RESCALE(7),  RESCALE(8),  RESCALE(9),

		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(18),
		    RESCALE(19),
		    RESCALE(20),
		    RESCALE(21),
		    RESCALE(22),
		    RESCALE(23),
		    RESCALE(24),
		    RESCALE(25),
		    RESCALE(26),
		    RESCALE(27),
		    RESCALE(28),
		    RESCALE(29),
		    RESCALE(30),
		    RESCALE(31),
		    RESCALE(32),
		    RESCALE(33),
		    RESCALE(34),
		    RESCALE(36),
		    RESCALE(37),
		    RESCALE(38),
		    RESCALE(39),
		};

		int shrunkY = (boundary.height * 14) >> 3;
		int extreme = boundary.width > shrunkY ? boundary.width : shrunkY;
		int scale = rescaleX[extreme];

		unsigned char cc[ICON_WIDTH]; // maps icon x (0-47) --> board column (0-39)

		int base = ((__BOARD_WIDTH / 2) << SHIFT) - (ICON_WIDTH / 2) * scale;
		for (int i = 0; i < ICON_WIDTH; i++) {
			cc[i] = (base < 0 || base >= ((__BOARD_WIDTH << SHIFT))) ? 0 : base >> SHIFT;
			base += scale;
		}

		unsigned char rr[ICON_DEPTH]; // maps icon row (0-21) --> board row (0-21)

		base = ((__BOARD_DEPTH / 2) << SHIFT) - (ICON_DEPTH / 2) * scale;
		for (int i = 0; i < ICON_DEPTH; i++) {
			rr[i] = (base < 0 || base >= ((__BOARD_DEPTH << SHIFT))) ? 0 : base >> SHIFT;
			base += scale;
		}

		int m1 = 4 + (startRow & 1);
		int m2 = m1 ^ 1;

		int br = roller;
		if (!enableICC)
			br--;

		for (int row = startRow; row < endRow; row++) {

			p = ADDRESS_OF(rr[row]);
			for (int col = 0; col < _1ROW; col++)
				img[col] = charSet[p[col]].small;

			unsigned char *ppf = RAM + ICON_BASE + row * 3 + _BUF_MENU_GRP0A;

			for (int col = 0; col < 6; col++) {

				unsigned char *p = cc + (col << 3);

				for (int segment = 0; segment < 3; segment++) {

					if (++br > 2)
						br = 0;

					int roll = segment * 3 + br;

					// clang-format off
                ppf[segment] = (unsigned char)(img[p[0]][roll] >> m2 << 7) |
                               (unsigned char)(img[p[1]][roll] >> m1 << 7) >> 1 |
                               (unsigned char)(img[p[2]][roll] >> m2 << 7) >> 2 |
                               (unsigned char)(img[p[3]][roll] >> m1 << 7) >> 3 |
                               (unsigned char)(img[p[4]][roll] >> m2 << 7) >> 4 |
                               (unsigned char)(img[p[5]][roll] >> m1 << 7) >> 5 |
                               (unsigned char)(img[p[6]][roll] >> m2 << 7) >> 6 |
                               (unsigned char)(img[p[7]][roll] >> m1 << 7) >> 7;
					// clang-format on

					if (staticx) {
						ppf[segment] |= getRandom32();
						ppf[segment] &= getRandom32();
					}
				}

				ppf += _ARENA_SCANLINES;
			}
		}
	}
}
// EOF