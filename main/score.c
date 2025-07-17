
#include "defines.h"
#include "defines_cdfj.h"
#include <stdbool.h>

#include "colour.h"
#include "main.h"
#include "man.h"
#include "menu.h"
#include "random.h"
#include "rooms.h"
#include "score.h"
#include "sound.h"

// #define RGB_BLACK       0
#define RGB_RED 1
#define RGB_BLUE 2
// #define RGB_PURPLE      3
// #define RGB_GREEN 4
#define RGB_YELLOW 5
#define RGB_AQUA 6
// #define RGB_WHITE       7

#define SPARKLE 180 /* # frames to sparkle BG on extra life */

int actualScore;

enum SCORE_MODE scoreCycle, nextScoreCycle;
int guaranteedViewTime;

static unsigned char scoreLineNew[10];

#if !ENABLE_COLOUR_SCORE
static unsigned char scoreLineOld[10];
#endif

static unsigned char scoreLineColour[10];

void initScore() {

	actualScore = 0;
	guaranteedViewTime = 1;
}

void addScore(int score) {

	actualScore += score;

	ADDAUDIO(SFX_SCORE);
}

const int pwr[] = {
    1, 10, 100, 1000, 10000, 100000,
};

// right-to-left, least-significant first digit position
const unsigned char mask[] = {
    0x0F, 0xF0, 0xF0, 0x0F, 0x0F, 0x0F, 0xF0, 0xF0, 0x0F, 0x0F,
};

const bool mirror[] = {
    1, 1, 0, 0, 1, 1, 1, 0, 0, 1,
};

const int base[] = {
    _BUF_PF2_RIGHT, _BUF_PF2_RIGHT, _BUF_PF1_RIGHT, _BUF_PF1_RIGHT, _BUF_PF0_RIGHT,
    _BUF_PF2_LEFT,  _BUF_PF2_LEFT,  _BUF_PF1_LEFT,  _BUF_PF1_LEFT,  _BUF_PF0_LEFT,
};

void setScoreCycle(enum SCORE_MODE cycle) {

	nextScoreCycle = cycle;

	if (guaranteedViewTime > 50)
		guaranteedViewTime = 50;
}

static unsigned char bigDigitBuffer[DIGIT_SIZE];

void drawBigDigit(int digit, int pos, int offset, int colour) {

	unsigned char pmask = mask[pos];
	unsigned char *p = RAM + base[pos] + offset;

	// if (digit == DIGIT_SPACE) {
	//     for (int line = 0; line < DIGIT_SIZE; line++)
	//         p[line] &= pmask;
	//     return;
	// }

	int shift2 = (pmask == 0x0F) ? 0 : 4;
	if (!mirror[pos])
		shift2 ^= 4;

#if ENABLE_COLOUR_SCORE
	if (!offset && colour && !enableICC)
#endif
		if (colour & 7)
			colour = 7;

	int shift = (digit & 1) << 2;
	digit >>= 1;

	int dbase = offset + roller;
	dbase -= ((dbase * (0x10003 / 3)) >> 16) * 3; // = mod 3
	dbase = 1 << dbase;

	const unsigned char *dig;
	if (digit & 0x40)
		dig = bigDigitBuffer;
	else
		dig = EXTERNAL(__DIGIT_SHAPE) + digit * DIGIT_SIZE;

	unsigned char shape;

	if (mirror[pos]) {

		for (int line = 0; line < DIGIT_SIZE; line++) {
			shape = BitRev[((dig[line] >> shift) & 0xF) << shift2];
			p[line] &= ~shape;
			if (colour & dbase)
				p[line] |= shape;
			dbase = (dbase << 1) | (dbase >> 2);
		}
	}

	else {

		for (int line = 0; line < DIGIT_SIZE; line++) {
			shape = ((dig[line] >> shift) & 0xF) << shift2;
			p[line] &= ~shape;
			if ((colour & dbase))
				p[line] |= shape;
			dbase = (dbase << 1) | (dbase >> 2);
		}
	}
}

void fillBit(int line, unsigned char b) {
	unsigned char *bdp = bigDigitBuffer + (line << 2);
	bdp[0] = bdp[1] = bdp[2] = bdp[3] = b;
}

void doubleSizeScore(int x, int y, int letter, int col) {

	const unsigned char *sample = EXTERNAL(__CHAR_A_TO_Z) + (letter + 29) * 10;

	for (int line = 0; line < 5; line++)
		fillBit(line, sample[line]);

	drawBigDigit(0x80, x, y, /*0x80 | */ col);
	drawBigDigit(0x81, x + 1, y, /*0x80 |*/ col);

	for (int line = 0; line < 5; line++)
		fillBit(line, sample[line + 5]);

	drawBigDigit(0x80, x, y + DIGIT_SIZE, /*0x40 | */ col);
	drawBigDigit(0x81, x + 1, y + DIGIT_SIZE, /*0x40 | */ col);
}

unsigned char *drawDecimal2(unsigned char *buffer, unsigned char *colour_buffer,
                            unsigned int colour, int cvt) {

	int forced = 0;
	for (int digit = 3; digit >= 0; digit--) {

		int displayDigit = 0;
		while (cvt >= pwr[digit]) {
			displayDigit++;
			cvt -= pwr[digit];
		}

		forced |= displayDigit;

		if (forced || !digit) {
			*buffer++ = displayDigit;

			if (colour_buffer)
				*colour_buffer++ = colour;
		}
	}

	return buffer;
}

void drawMoves() {
	drawDecimal2(scoreLineNew + 7, scoreLineColour + 7, RGB_YELLOW, pillCount);
	drawDecimal2(scoreLineNew + 2, scoreLineColour + 2, RGB_YELLOW, moves);
}

void drawTime() {

	//	time = 0x65000;

	int tPos = time60ths >= 0xA00 ? time60ths >= 0x6400 ? 5 : 6 : 7;

	scoreLineNew[tPos] = LETTER('T');
	scoreLineColour[tPos++] = RGB_BLUE;

	// if (time60ths > 0xA00 || ++toggle & 16)
	drawDecimal2(scoreLineNew + tPos, scoreLineColour + tPos,
	             time60ths < 0xA00 ? RGB_RED : RGB_AQUA, time60ths >> 8);
}

void drawLives() {

	scoreLineNew[1] = LETTER('L');
	scoreLineColour[1] = RGB_BLUE;

	drawDecimal2(scoreLineNew + 2, scoreLineColour + 2, RGB_YELLOW, lives & 0x7F);
}

void drawTheScore(int score) {

	for (int digit = 5; digit >= 0; digit--) {

		int displayDigit = 0;
		while (score >= pwr[digit]) {
			displayDigit++;
			score -= pwr[digit];
		}

		scoreLineNew[7 - digit] = displayDigit;
		scoreLineColour[7 - digit] = 1; // digit + 1;
	}

	for (int digit = 5; digit; digit--) {
		if (!scoreLineNew[digit])
			scoreLineNew[digit] = DIGIT_SPACE;
		else
			break;
	}
}

void drawScore() {

	if (!--guaranteedViewTime) {

		if (scoreCycle == nextScoreCycle) {

			nextScoreCycle = scoreCycle + 1;
			if (nextScoreCycle > SCORELINE_SCORE)
				nextScoreCycle = manDead ? SCORELINE_LIVES : SCORELINE_TIME;

			guaranteedViewTime = 250;
		}

		else {

			scoreCycle = nextScoreCycle;
			guaranteedViewTime = 75;
		}
	}

	if (!exitMode && !manDead && time60ths < 0xA00) {
		//        ADDAUDIO(SFX_COUNTDOWN2);
		scoreCycle = SCORELINE_TIME;
	}

	scoreCycle = SCORELINE_TIME; // tmp

	for (int i = 0; i < 10; i++) {
#if !ENABLE_COLOUR_SCORE
		scoreLineOld[i] = scoreLineNew[i];
#endif
		scoreLineNew[i] = DIGIT_SPACE;
	}

	// scoreCycle = SCORELINE_SCORE; // TMP

	switch (scoreCycle) {
	case SCORELINE_TIME:
		drawMoves();
		break;
	case SCORELINE_SCORE:

		drawTheScore(actualScore);
		break;

	case SCORELINE_LIVES:
		drawTime();
		drawLives();
		break;

	default:
		break;
	}

	unsigned char *p = RAM + _BUF_PF0_LEFT;
	for (int line = 0; line < SCORE_SCANLINES; p++, line++)
		for (int i = 0; i < 6; i++)
			p[i * _ARENA_SCANLINES] = 0;

	for (int i = 0; i < 10; i++) {
#if !ENABLE_COLOUR_SCORE
		if (scoreLineNew[i] != scoreLineOld[i])
#endif

			drawBigDigit(scoreLineNew[i], 9 - i, 0, scoreLineColour[i]);
	}
}

// EOF