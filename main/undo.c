#include "undo.h"
#include "attribute.h"
#include "colour.h"
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

void pushUndo(int px, int py, unsigned char *from, unsigned char *to) {

	if (undoTop >= MAX_UNDO) {
		undoTop = 0;
		for (int i = 1; i < MAX_UNDO; i++)
			pushUndo(undoStack[i].manX, undoStack[i].manY, undoStack[i].from, undoStack[i].to);
	}

	undoStack[undoTop++] = (MOVE){px, py, from, to};
}

void undoLastPush() {
	while (undoLastMove())
		;
}

bool undoLastMove() {

	bool repeat = false;
	if (undoTop) {

		MOVE m = undoStack[--undoTop];

		manX = m.manX;
		manY = m.manY;

		if (m.to) {

			int type = CharToType[*m.to];
			if (Attribute[type] & ATT_TARGETLIKE) {
				pillCount++;
				*m.to = CH_PILL1;
			} else
				*m.to = CH_BLANK;

			type = CharToType[*m.from];
			if (Attribute[type] & ATT_TARGETLIKE) {
				pillCount--;
				*m.from = CH_BOX_LOCKED;
			} else
				*m.from = CH_BOX;

			repeat = false;
		}

		else
			repeat = true;

		deadlock = false; // force recalculation
	}

	else {
		FLASH(0xD6, 10);
		scoreCycle = SCORELINE_TIME; // exit undo mode
	}

	return repeat;
}

// EOF