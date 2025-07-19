#include "undo.h"
#include "attribute.h"
#include "colour.h"
#include "deadlock.h"
#include "defines_cdfj.h"
#include "main.h"
#include "man.h"
#include "rooms.h"
#include "score.h"
#include <stdbool.h>

unsigned short undoStack[MAX_UNDO];
int undoTop;
int undoing;

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

		unsigned short m = undoStack[--undoTop];

		manX = (m >> 9) & 0x3F;
		manY = (m >> 3) & 0x3F;

		int dir = (m >> 1) & 3;
		bool boxMoved = (m & 1) > 0;

		unsigned char *from = ADDRESS_OF(manY) + manX + dirOffset[dir];
		unsigned char *to = from + dirOffset[dir];

		// Only if we pushed a box off, do we need to modify the to square

		if (boxMoved) {
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
				*from = CH_BOX_LOCKED;
			} else
				*from = CH_BOX;
		}

		else
			repeat = true;
	}

	if (!undoTop) {
		FLASH(0xD6, 10);
		scoreCycle = SCORELINE_TIME; // exit undo mode
		repeat = false;
	}

	return repeat;
}

// EOF