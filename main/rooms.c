#include "rooms.h"
#include "attribute.h"
#include "defines_cdfj.h"
#include "main.h"
#include "man.h"
#include "random.h"
#include "scroll.h"

int pillCount;

// Sokoban level data

#define NEXTLINE '|'
#define WALL '#'
#define PLAYERONGOAL '+'
#define PLAYERONNORMAL '@'
#define BOXONTARGET '*'
#define TARGET '.'
#define GAP ' '
#define GAP2 '-'
#define GAP3 '_'
#define BOX '$'
#define STEEL '='

#include "scrapedRooms.c"

struct stats roomStats[sizeof(room) / sizeof(room[0])];

short unpackLength;

void putObject(int *x, int y, unsigned char ch, int length) {

	if (!length)
		length = 1;

	unsigned char *board = ADDRESS_OF(y) + *x;
	for (int i = 0; i < length; i++) {
		*board++ = ch;

		if (ch == CH_PILL_1)
			pillCount++;
	}

	*x += length;
	unpackLength = 0;
}

int getRoomCount() { return sizeof(room) / sizeof(room[0]); }

int getRandomRoomNumber() { return rangeRandom(getRoomCount()); }

void setScroll() {

	int playerShoePixelX = manX * PIXELS_PER_CHAR + (PIXELS_PER_CHAR / 2) - 160 / 10;
	scrollX[DISPLAY_NORMAL] = playerShoePixelX << SHIFT_SCROLLX;
	clamp(&scrollX[DISPLAY_NORMAL], SCROLL_MAXIMUM_X_NORMAL);

	int playerShoePixelY = (manY + 1) * CHAR_HEIGHT / 3 - _ICC_SCANLINES / 2;
	scrollY[DISPLAY_NORMAL] = playerShoePixelY << SHIFT_SCROLLY;
	clamp(&scrollY[DISPLAY_NORMAL], SCROLL_MAXIMUM_Y_NORMAL);

	playerShoePixelX = manX * 3;
	scrollX[DISPLAY_HALF] = playerShoePixelX << SHIFT_SCROLLX;
	clamp(&scrollX[DISPLAY_HALF], SCROLL_MAXIMUM_X_HALF);

	playerShoePixelY =
	    (manY + 1) * CHAR_HEIGHT_HALF / 3 - (_ICC_SCANLINES - SCORE_SCANLINES / 3) / 2;
	scrollY[DISPLAY_HALF] = playerShoePixelY << SHIFT_SCROLLY;
	clamp(&scrollY[DISPLAY_HALF], SCROLL_MAXIMUM_Y_HALF);
}

void clearBoard(char ch) {

	int ch4 = (ch << 24) | (ch << 16) | (ch << 8) | ch;
	unsigned int *board = (unsigned int *)(RAM + _BOARD);
	for (int i = 0; i < __BOARD_SIZE / 4; i++)
		board[i] = ch4;
}

void initRooms() {
	for (unsigned int i = 0; i < sizeof(roomStats) / sizeof(roomStats[0]); i++)
		roomStats[i] = (struct stats){0, 0};
}

void unpackRoom(int roomNumber) {

	pillCount = 0;

	int left = (__BOARD_WIDTH - room[roomNumber].width) >> 1;

	int x = left;
	int y = (__BOARD_DEPTH - room[roomNumber].height) >> 1;

	unpackLength = 0;

	clearBoard(CH_STEELWALL);

	const char *p = room[roomNumber].design;
	while (*p) {

		switch (*p) {
		case NEXTLINE:
			y += unpackLength ? unpackLength : 1;
			x = left;
			break;

		case STEEL:
			putObject(&x, y, CH_STEELWALL, unpackLength);
			break;

		case WALL:
			putObject(&x, y, CH_BRICKWALL, unpackLength);
			break;

		case PLAYERONNORMAL:
			manX = x;
			manY = y;

			putObject(&x, y, CH_BLANK, unpackLength);
			break;

		case BOXONTARGET:
			putObject(&x, y, CH_BOX_LOCKED, unpackLength);
			break;

		case PLAYERONGOAL:
			manX = x;
			manY = y;

			putObject(&x, y, CH_PILL_1, unpackLength);
			break;

		case TARGET:
			putObject(&x, y, CH_PILL_1, unpackLength);
			break;

		case GAP:
		case GAP2:
		case GAP3:
			putObject(&x, y, CH_BLANK, unpackLength);
			break;

		case BOX:
			putObject(&x, y, CH_BOX, unpackLength);
			break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			unpackLength = 10 * unpackLength + (*p - '0');
			break;

		default:
			break;
		}

		p++;
	}

	setScroll();
}

// EOF
