#include "rooms.h"
#include "attribute.h"
#include "defines_cdfj.h"
#include "drawplayer.h"
#include "main.h"
#include "man.h"
#include "menu.h"
#include "random.h"
#include "score.h"
#include "scroll.h"

int pillCount;

// Sokoban level data

// clang-format off

const char *room[] = {

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

// To add new rooms just place either compressed or uncompressed room here.
// Rooms are standard-format, represented as a single 0-terminated string
// tools/compressroom.py will compress a room for you

// ----------------------------------------------------------------------------------------------

#if 1
//      ###
// ######@##
// #    .* #
// #   #   #
// #####$# #
//     #   #
//     #####

"5=3#|6#@2#|#4 .* #|#3 #3 #|5#$# #|4=#3 #|4=5#",

#endif
// ----------------------------------------------------------------------------------------------

#if 1
//   ########
//  ##  . . #
//  # *$# # #
//  #.  #$# ##
// ##* #  #  #
// #   # *   #
// # $#@ .#  #
// #     $ ###
// ##  #   #
//  ########

"2=8#|=2#2 . . #|=# *$# # #|=#.2 #$# 2#|2#* #2 #2 #|#3 # *3 #|# $#@ .#2 #|#5 $ 3#|2#2 #3 #|=8#",

#endif
// ----------------------------------------------------------------------------------------------

#if 1
//    ####
//    #  #
//    #$ #
//    #  # ####
// #### ####  #
// #   .. ..  #
// #  $*. *.$@##
// #   #$ $#$  #
// #####   #   #
//     #########

"3=4#|3=#2 #|3=#$ #|3=#2 #=4#|4# 4#2 #|#3 2. 2.2 #|#2 $*. *.$@2#|#3 #$ $#$2 #|5#3 #3 #|4=9#",

#endif
// ----------------------------------------------------------------------------------------------

#if 1
//   ####
//   #  #
//   #$ #
//   #  # ####
// ### ####  #
// #  .. ..  ###
// # $*. *.$@  #
// ## #$ $#$ # #
//  # #   #  # #
//  #  ######  #
//  ##        ##
//   ##########

"2=4#|2=#2 #|2=#$ #|2=#2 #=4#|3# 4#2 #|#2 2. 2.2 3#|# $*. *.$@2 #|2# #$ $#$ # #|=# #3 #2 # #|=#2 6#2 #|=2#8 2#|2=10#",

#endif
// ----------------------------------------------------------------------------------------------

#if 1
//            ######
//     ####  ##    #
//   ###  #  #  ## ###
// ###    #### #   $ #
// #  $ @ ...*..  $  #
// # $ $  ## ###   ###
// ### ###   # #####
//  #      ###
//  #   ####
//  #####

"11=6#|4=4#2=2#4 #|2=3#2 #2=#2 2# 3#|3#4 4# #3 $ #|#2 $ @ 3.*2.2 $2 #|# $ $2 2# 3#3 3#|3# 3#3 # 5#|=#6 3#|=#3 4#|=5#",


#endif
// ----------------------------------------------------------------------------------------------

#if 1
//       ######
//       #    #
//       # @  ###
// ####  #      #
// #  ####..#.#$#####
// # $ $ ##...      #
// #     .....#$$   #
// ###### ##$## #####
//      #  $    #
//      #### ####
//        #  #
//        #  #####
//      ### $    #
//      #  $ $   #
//      # #$# ####
//      #     #
//      #######

"6=6#|6=#4 #|6=# @2 3#|4#2=#6 #|#2 4#2.#.#$5#|# $ $ 2#3.6 #|#5 5.#2$3 #|6# 2#$2# 5#|5=#2 $4 #|5=4# 4#|7=#2 #|7=#2 5#|5=3# $4 #|5=#2 $ $3 #|5=# #$# 4#|5=#5 #|5=7#",

#endif
// ----------------------------------------------------------------------------------------------


};

// clang-format on

static short firstX = 999;
static short firstY = 999;
static short lastX = 0;
static short lastY = 0;

short unpackLength;

void putObject(bool dryRun, int *x, int y, unsigned char ch, int length) {

	if (!length)
		length = 1;

	unsigned char *board = ADDRESS_OF(y) + *x;
	for (int i = 0; i < length; i++) {
		if (!dryRun)
			*board++ = ch;

		if (ch == CH_PILL_1)
			pillCount++;
	}

	if (dryRun && ch == CH_BRICKWALL) {
		if (*x < firstX)
			firstX = *x;
		if (*x > lastX)
			lastX = *x;

		if (y < firstY)
			firstY = y;
		if (y > lastY)
			lastY = y;
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

void clearBoard() {

	unsigned char *board = ADDRESS_OF(0);
	for (int i = 0; i < __BOARD_SIZE; i++)
		board[i] = CH_STEELWALL;

	//    zeroBuffer((int *)ADDRESS_OF(0), __BOARD_SIZE / 4); }
}

void unpackRoom(BOUNDARY *boundary, bool dryRun, int roomNumber) {

	pillCount = 0;

	firstX = 999;
	firstY = 999;
	lastX = 0;
	lastY = 0;

	int x = boundary->x, y = boundary->y;

	unpackLength = 0;

	if (!dryRun)
		clearBoard();

	const char *p = room[roomNumber];
	while (*p) {

		switch (*p) {
		case NEXTLINE:
			y += unpackLength ? unpackLength : 1;
			x = boundary->x;
			break;

		case STEEL:
			putObject(dryRun, &x, y, CH_STEELWALL, unpackLength);
			break;

		case WALL:
			putObject(dryRun, &x, y, CH_BRICKWALL, unpackLength);
			break;

		case PLAYERONNORMAL:
			manX = x;
			manY = y;

			//			setScroll();

			putObject(dryRun, &x, y, CH_BLANK, unpackLength);
			break;

		case BOXONTARGET:
			putObject(dryRun, &x, y, CH_BOX_LOCKED, unpackLength);
			break;

		case PLAYERONGOAL:
			manX = x;
			manY = y;

			//		setScroll();
			putObject(dryRun, &x, y, CH_PILL_1, unpackLength);
			break;

		case TARGET:
			putObject(dryRun, &x, y, CH_PILL_1, unpackLength);
			break;

		case GAP:
		case GAP2:
		case GAP3:
			putObject(dryRun, &x, y, CH_BLANK, unpackLength);
			break;

		case BOX:
			putObject(dryRun, &x, y, CH_BOX, unpackLength);
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

	if (dryRun) {
		int width = lastX - firstX + 1;
		boundary->width = width < 0 ? 0 : width;

		int height = lastY - firstY + 1;
		boundary->height = height < 0 ? 0 : height;
	}

	// else {

	// 	unsigned char *board;
	// 	for (int y = 0; y < __BOARD_DEPTH; y++) {

	// 		for (int x = 0; x < __BOARD_WIDTH; x++) {
	// 			board = ADDRESS_OF(y) + x;
	// 			if (*board == CH_BLANK)
	// 				*board = CH_STEELWALL;
	// 			else
	// 				break;
	// 		}

	// 		for (int x = __BOARD_WIDTH - 1; x >= 0; x--) {
	// 			board = ADDRESS_OF(y) + x;
	// 			if (*board == CH_BLANK)
	// 				*board = CH_STEELWALL;
	// 			else
	// 				break;
	// 		}
	// 	}

	// 	for (int x = 0; x < __BOARD_WIDTH; x++) {

	// 		for (int y = 0; y < __BOARD_DEPTH; y++) {
	// 			board = ADDRESS_OF(y) + x;
	// 			if (*board != CH_BRICKWALL) //== CH_BLANK || *board == CH_STEELWALL)
	// 				*board = CH_STEELWALL;
	// 			else
	// 				break;
	// 		}

	// 		for (int y = __BOARD_DEPTH - 1; y >= 0; y--) {
	// 			board = ADDRESS_OF(y) + x;
	// 			if (*board != CH_BRICKWALL) //== CH_BLANK || *board == CH_STEELWALL)
	// 				*board = CH_STEELWALL;
	// 			else
	// 				break;
	// 		}
	// 	}
	// }

	//	actualScore = pillCount;

	setScroll();
}

// EOF
