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

MOVE undoStack[MAX_UNDO];
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
			pushUndo(undoStack[i].manX, undoStack[i].manY, undoStack[i].dir, undoStack[i].boxMoved);
	}
	undoStack[undoTop++] = (MOVE){px, py, dir, boxMoved};
}

void undoLastPush() {
	while (undoLastMove())
		;
}

bool undoLastMove() {
	/*
	    // Returns true if we only moved player, not a box
	    // allowing iteration until box pushed

	    for (int i = undoTop - 1; i >= 0; i--)
	        if (undoStack[i].to) { // there's a box move!

	            MOVE *m = &undoStack[--undoTop];

	            manX = m->manX;
	            manY = m->manY;

	            if (m->to) {

	                int type = CharToType[*m->to];
	                if (Attribute[type] & ATT_TARGETLIKE) {
	                    pillCount++;
	                    *m->to = CH_PILL1;
	                } else
	                    *m->to = CH_BLANK;

	                type = CharToType[*m->from];
	                if (Attribute[type] & ATT_TARGETLIKE) {
	                    pillCount--;
	                    *m->from = CH_BOX_LOCKED;
	                } else
	                    *m->from = CH_BOX;

	                deadlock = false; // force recalculation
	                return false;
	            }

	            return true;
	        }

	    // no box moves in undo list so we exit UNDO
	    // don't bother clearing stack; it will auto-drop as moves are added

	    FLASH(0xD6, 10);
	    scoreCycle = SCORELINE_TIME;

	    return false;
	}
	*/

	bool repeat = false;
	if (undoTop) {

		MOVE m = undoStack[--undoTop];

		manX = m.manX;
		manY = m.manY;

		int dir = m.dir;

		unsigned char *from = ADDRESS_OF(manY) + manX + dirOffset[dir];
		unsigned char *to = from + dirOffset[dir];

		int typeTo = CharToType[*to];
		if (Attribute[typeTo] & ATT_TARGETLIKE) {
			pillCount++;
			*to = CH_PILL1;
		} else
			*to = CH_BLANK;

		int typeFrom = CharToType[*from];

		// Only if we pused a box off, do we need to modify the square
		if (Attribute[typeTo] & ATT_BOX) {

			if (Attribute[typeFrom] & ATT_TARGETLIKE) {
				pillCount--;
				*from = CH_BOX_LOCKED;
			} else
				*from = CH_BOX;

			deadlock = false; // force recalculation
		}

		else
			repeat = true;

	} else {

		FLASH(0xD6, 10);
		scoreCycle = SCORELINE_TIME; // exit undo mode
	}

	return repeat;
}

// EOF