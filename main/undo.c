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
#include "random.h"
#include "rooms.h"
#include "score.h"
#include "sound.h"
#include <stdbool.h>

unsigned short undoStack[MAX_UNDO];
int undoTop;
int undoing;

void highlightUndo() {

	if (scoreCycle == SCORELINE_UNDO) {

		findBox

		    typedef struct {
			signed char x;
			signed y;
		} Point;

		const Point circlePoints[] = {
		    {4, 0},   {4, 1},   {4, 2},   {4, 3},   {3, 4},   {3, 5},   {3, 6},   {2, 6},
		    {2, 7},   {1, 8},   {0, 8},   {-1, 8},  {-2, 7},  {-2, 6},  {-3, 6},  {-3, 5},
		    {-3, 4},  {-4, 3},  {-4, 2},  {-4, 1},  {-4, 0},  {-4, -1}, {-4, -2}, {-4, -3},
		    {-3, -4}, {-3, -5}, {-3, -6}, {-2, -6}, {-2, -7}, {-1, -8}, {0, -8},  {1, -8},
		    {2, -7},  {2, -6},  {3, -6},  {3, -5},  {3, -4},  {4, -3},  {4, -2},  {4, -1},
		};

		// Calculate trixel coordinates
		// x: (char blocks * pixels per char block) + centering + (direction * pixel offset) / 4
		// pixels per PF y: char blocks * trixels per char + centering + pixel offset converted to
		// trixels (i.e. /3)

		int x = (manX * PIXELS_PER_CHAR + 2 + ((manFaceDirection * frameAdjustX) >> 2));
		int y = ((manY * (CHAR_HEIGHT / 3) + 4 - ((frameAdjustY * (0X100 / 3)) >> 8)));

		static int cp = 0;
		if (++cp >= (int)(sizeof(circlePoints) / sizeof(circlePoints[0])))
			cp = 0;

		addLocalFirework(x + circlePoints[cp].x, y + circlePoints[cp].y, 3, 15);
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

		int dir = (m >> 1) & 3;
		bool boxMoved = (m & 1) > 0;

		int x = (manX * PIXELS_PER_CHAR + 2 + ((manFaceDirection * frameAdjustX) >> 2));
		int y = ((manY * (CHAR_HEIGHT / 3) + 4 - ((frameAdjustY * (0X100 / 3)) >> 8)));

		manX = (m >> 9) & 0x3F;
		manY = (m >> 3) & 0x3F;

		unsigned char *from = ADDRESS_OF(manY) + manX + dirOffset[dir];
		unsigned char *to = from + dirOffset[dir];

		// Only if we pushed a box off, do we need to modify the to square

		if (boxMoved) {

#if ENABLE_UNDO_DOTGUIDE

			// dotted path lines showing box movement
			static const signed char mv[][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
			for (int i = 0; i < ((dir < 2) ? 3 : 4); i++)
				addLocalFirework(x - mv[dir][0] * PIXELS_PER_CHAR + i * mv[dir][0] * 2,
				                 y - mv[dir][1] * (CHAR_HEIGHT / 3) + i * mv[dir][1] * 3, 7,
				                 i * 5 + 20);
#endif

			ADDAUDIO(SFX_PUSH);

			int typeTo = CharToType[*to];
			if (Attribute[typeTo] & ATT_TARGETLIKE) {
				pillCount++;
				*to = CH_PILL1;
			} else
				*to = CH_BLANK;

			deadlock = 0; // force recalculation
			lastDeadlock = 0;
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