#include "undo.h"
#include "animations.h"
#include "attribute.h"
#include "colour.h"
#include "deadlock.h"
#include "defines.h"
#include "defines_cdfj.h"
#include "main.h"
#include "man.h"
#include "player.h"
#include "rooms.h"
#include "score.h"
#include "sound.h"
#include <stdbool.h>

unsigned short undoStack[MAX_UNDO];
int undoTop;
int undoing;

void highlightUndo() {

	if (scoreCycle == SCORELINE_UNDO) {
		typedef struct {
			int x;
			int y;
		} Point;

		static const Point circlePoints[60] = {
		    {4, 0},   {4, 1},   {4, 2},   {4, 2},   {4, 3},   {3, 4},   {3, 5},   {3, 5},
		    {3, 6},   {2, 6},   {2, 7},   {2, 7},   {1, 8},   {1, 8},   {0, 8},   {0, 8},
		    {0, 8},   {-1, 8},  {-1, 8},  {-2, 7},  {-2, 7},  {-2, 6},  {-3, 6},  {-3, 5},
		    {-3, 5},  {-3, 4},  {-4, 3},  {-4, 2},  {-4, 2},  {-4, 1},  {-4, 0},  {-4, -1},
		    {-4, -2}, {-4, -2}, {-4, -3}, {-3, -4}, {-3, -5}, {-3, -5}, {-3, -6}, {-2, -6},
		    {-2, -7}, {-2, -7}, {-1, -8}, {-1, -8}, {0, -8},  {0, -8},  {0, -8},  {1, -8},
		    {1, -8},  {2, -7},  {2, -7},  {2, -6},  {3, -6},  {3, -5},  {3, -5},  {3, -4},
		    {4, -3},  {4, -2},  {4, -2},  {4, -1},
		};

		int x = (manX * PIXELS_PER_CHAR + 2 + ((manFaceDirection * frameAdjustX) >> 2));
		int y = ((manY * (CHAR_HEIGHT / 3) + 4 - ((frameAdjustY * (0X100 / 3)) >> 8)));

#define CPS 320
		// #define CPCOLOURSPEED 10

		static unsigned int cp = 0;
		// static int cpc = 0;
		// static int cpcolour = 0;

		// if (cpc++ > CPCOLOURSPEED) {
		// 	cpcolour = (cpcolour + 1) & 7;
		// 	if (!cpcolour)
		// 		cpcolour = 1;
		// 	cpc = 0;
		// }

		static int lastcp = 0;

		++cp;
		int cp2 = (cp * CPS) >> 8;
		if (cp2 >= (sizeof(circlePoints) / sizeof(circlePoints[0]))) {
			cp = 0;
			cp2 = 0;
		}

		if (cp2 != lastcp) {

			addLocalFirework(x + circlePoints[cp2].x, y + circlePoints[cp2].y, 3, 15);

			// for (int ox = -1; ox < 1; ox++)
			// 	for (int oy = -1; oy < 2; oy++) {
			// 		addLocalFirework(ox + x + circlePoints[cp2].x, oy + y + circlePoints[cp2].y, 7,
			// 		                 6);
			// 	}

			lastcp = cp2;
		}
	}
}

void initUndo() {
	undoTop = 0;
	undoing = 0;
}

void pushUndo(int px, int py, int dir, bool boxMoved) {

	if (undoTop >= MAX_UNDO) {
		undoTop = 0;
		for (int i = 1; i < MAX_UNDO; i++)
			undoStack[undoTop++] = undoStack[i];
	}

	undoStack[undoTop++] =
	    ((px & 0x3F) << 9) | ((py & 0x3F) << 3) | ((dir & 3) << 1) | ((int)boxMoved & 1);
}

bool undoLastMove() {

	// Returns true if we only moved player, not a box
	// allowing iteration until box pushed

	bool repeat = false;
	if (undoTop) {

		ADDAUDIO(SFX_SPACE);

		unsigned short m = undoStack[--undoTop];

		manX = (m >> 9) & 0x3F;
		manY = (m >> 3) & 0x3F;

		int dir = (m >> 1) & 3;
		bool boxMoved = (m & 1) > 0;

		unsigned char *from = ADDRESS_OF(manY) + manX + dirOffset[dir];
		unsigned char *to = from + dirOffset[dir];

		// Only if we pushed a box off, do we need to modify the to square

		if (boxMoved) {

			// 			// clang-format off
			//              int arrowOffset[4][8][2] = {
			// {{3 ,0},{4,0},{5,0},{6,0},{5,1},{5,-1},{4,2},{4,-2},},
			// {{3 ,0},{4,0},{5,0},{6,0},{4,1},{4,-1},{5,2},{5,-2},},
			// {{0 ,3},{0,4},{0,5},{0,6},{-1,5},{1,5},{-2,4},{2,4},},
			// {{0 ,3},{0,4},{0,5},{0,6},{-1,4},{1,4},{-2,5},{2,5},},
			//             };

			//             int arrowStart[4][2] = {{-15,-2},{6,-2},{0,-24},{0,11}};
			// 			// clang-format on

			// int x = (manX * PIXELS_PER_CHAR + 2 + ((manFaceDirection * frameAdjustX) >> 2));
			// int y = ((manY * (CHAR_HEIGHT / 3) + 6 - ((frameAdjustY * (0X100 / 3)) >> 8)));

			// // 			for (int i = 0; i < 8; i++)
			// // 				addLocalFirework(x + arrowOffset[dir][i][0], y +
			// arrowOffset[dir][i][1],
			// // 7, 20);

			// static int circleindex = 0;

			// int circle_x[35] = {5,  5,  5,  4,  4,  3,  3,  2,  1,  0,  -1, -2,
			//                     -3, -3, -3, -4, -5, -5, -5, -5, -5, -4, -4, -3,
			//                     -3, -2, -1, 0,  1,  2,  3,  3,  3,  4,  5};
			// int circle_y[35] = {0, 1,  2,  3,  4,  5,  6,  7,  7,  7,  7,  7,  6,  5,  4,  3,
			// 2, 1,
			//                     0, -1, -2, -3, -4, -5, -6, -7, -7, -7, -7, -7, -6, -5, -4,
			//                     -3, -2};

			// const int offX[] = {-5, 5, 0, 0};
			// const int offY[] = {0, 0, -10, 10};

			// for (int i = 0; i < SPLATS; i++)
			// 	addLocalFirework(x + ((circle_x[i] * 0xC0) >> 8) + offX[dir],
			// 	                 y + circle_y[i] + offY[dir], 2, 120);

			ADDAUDIO(SFX_PUSH);

			int typeTo = CharToType[*to];
			if (Attribute[typeTo] & ATT_TARGETLIKE) {
				pillCount++;
				*to = CH_PILL1;
			} else
				*to = CH_BLANK;

			deadlock = false; // force recalculation
		}

		int typeFrom = CharToType[*from];
		if (boxMoved) {
			if (Attribute[typeFrom] & ATT_TARGETLIKE) {
				pillCount--;
				*from = CH_BOX_UNDO_CORRECT;
				startCharAnimation(TYPE_BOX_UNDO_CORRECT, AnimateBase[TYPE_BOX_UNDO_CORRECT]);
			} else {
				*from = CH_BOX_UNDO;
				startCharAnimation(TYPE_BOX_UNDO, AnimateBase[TYPE_BOX_UNDO]);
			}
		}

		else
			repeat = true;
	}

	if (!undoTop) {
		//		FLASH(0xD6, 4);
		scoreCycle = SCORELINE_TIME; // exit undo mode
		hackStartAnimation(animationList[ANIM_PLAYER], ID_Stand);
		repeat = false;
	}

	else {
		//		ARENA_COLOUR = 0x40;
	}

	return repeat;
}

// EOF