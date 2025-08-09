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

bool findBox(int *x, int *y) {

	static const signed char offX[] = {-1, 1, 0, 0};
	static const signed char offY[] = {0, 0, -1, 1};

	bool found = false;
	int localTop = undoTop;

	*x = manX;
	*y = manY;

	while (!found && localTop) {

		unsigned short m = undoStack[--localTop];

		int dir = (m >> 1) & 3;
		found = (m & 1) > 0;

		*x += (found ? 1 : -1) * offX[dir];
		*y += (found ? 1 : -1) * offY[dir];
	}

	return found;
}

void circleBox(int boxX, int boxY, int duration, char colour) {

	typedef struct {
		signed char x;
		signed char y;
	} Point;

	static const Point circlePoints[] = {
	    {4, 0},   {4, 1},   {4, 2},   {4, 3},   {3, 4},   {3, 5},   {3, 6},   {2, 6},
	    {2, 7},   {1, 8},   {0, 8},   {-1, 8},  {-2, 7},  {-2, 6},  {-3, 6},  {-3, 5},
	    {-3, 4},  {-4, 3},  {-4, 2},  {-4, 1},  {-4, 0},  {-4, -1}, {-4, -2}, {-4, -3},
	    {-3, -4}, {-3, -5}, {-3, -6}, {-2, -6}, {-2, -7}, {-1, -8}, {0, -8},  {1, -8},
	    {2, -7},  {2, -6},  {3, -6},  {3, -5},  {3, -4},  {4, -3},  {4, -2},  {4, -1},
	};

	// Calculate trixel coordinates
	// x: (char blocks * pixels per char block) + centering + (direction * pixel offset)
	// / 4 pixels per PF y: char blocks * tr8xels per char + centering + pixel offset
	// converted to trixels (i.e. /3)

	int x = (boxX * PIXELS_PER_CHAR + 2);     // + ((manFaceDirection * frameAdjustX) >> 2));
	int y = ((boxY * (CHAR_HEIGHT / 3) + 4)); // - ((frameAdjustY * (0X100 / 3)) >> 8)));

	if (y < 0)
		FLASH(0x99, 9);

	static const char cplay[] = {7, 7, 7, 7, 7, 7, 0, 0, 0, 7, 7, 7, 7, 7, 7, 0, 0, 0,
	                             7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 0, 0, 0, 0, 0, 0};
	static char col = 0;
	if (++col >= sizeof(cplay) / sizeof(cplay[0]))
		col = 0;

	for (unsigned int i = 0; i < sizeof(circlePoints) / sizeof(circlePoints[0]); i++)
		addLocalPixel(x + circlePoints[i].x, y + circlePoints[i].y,
		              colour ? colour : cplay[(int)col], duration);
}

void highlightUndo() {

	if (!undoing && scoreCycle == SCORELINE_UNDO) {

		frameAdjustX += 2 * ((frameAdjustX < 0) - (frameAdjustX > 0));
		frameAdjustY += ((frameAdjustY < 0) - (frameAdjustY > 0)) * 3;

		int boxX, boxY;

		if (findBox(&boxX, &boxY)) {

			circleBox(boxX, boxY, 1, 0);
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

	// TODO: compress non-box moves for player when stack too big

	undoStack[undoTop++] =
	    ((px & 0x3F) << 10) | ((py & 0x7F) << 3) | ((dir & 3) << 1) | ((int)boxMoved & 1);
}

bool undoLastMove() {

	// Returns true if we only moved player, not a box
	// allowing iteration until box pushed

	bool repeat = false;
	bool boxMoved = false;
	while (undoTop && findBox(0, 0) && !boxMoved) {

		ADDAUDIO(SFX_SPACE);

		unsigned short m = undoStack[--undoTop];

		int dir = (m >> 1) & 3;
		boxMoved = (m & 1) > 0;

		int oldX = manX;
		int oldY = manY;

		manX = (m >> 10) & 0x3F;
		manY = (m >> 3) & 0x7F;

		static const unsigned char mfd[] = {-1, 1, 1, 1};
		manFaceDirection = mfd[dir];

		// frameAdjustX = manFaceDirection * (oldX - manX) * PIXELS_PER_CHAR * 4;
		// frameAdjustY = (manY - oldY) * CHAR_HEIGHT;

		unsigned char *from = ADDRESS_OF(manY) + manX + dirOffset[dir];
		unsigned char *to = from + dirOffset[dir];

		int typeFrom = CharToType[*from];

		// Only if we pushed a box off, do we need to modify the to square

		if (boxMoved) {

#if ENABLE_UNDO_DOTGUIDE

			// dotted path lines showing box movement
			static const signed char mv[][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};
			for (int i = 0; i < ((dir < 2) ? 3 : 4); i++)
				addLocalPixel(x - mv[dir][0] * PIXELS_PER_CHAR + i * mv[dir][0] * 2,
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

			if (Attribute[typeFrom] & ATT_TARGETLIKE) {
				pillCount--;
				*from = CH_BOX_UNDO_CORRECT;
				startCharAnimation(TYPE_BOX_UNDO_CORRECT, AnimateBase[TYPE_BOX_UNDO_CORRECT] + 2);
			} else {
				*from = CH_BOX_UNDO;
				startCharAnimation(TYPE_BOX_UNDO, AnimateBase[TYPE_BOX_UNDO] + 2);
			}

			circleBox(oldX, oldY, 30, 1);
		}

		else {
			// repeat = true;
			// undoing = UNDO_SPEED;
		}
	}

	if (!undoTop) {
		killAudio(SFX_MAGIC);
		scoreCycle = SCORELINE_TIME; // exit undo mode
		hackStartAnimation(animationList[ANIM_PLAYER], ID_Stand);
		repeat = false;
	}

	return repeat;
}

// EOF