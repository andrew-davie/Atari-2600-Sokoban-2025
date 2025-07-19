#ifndef __UNDO_H
#define __UNDO_H

#include <stdbool.h>

typedef struct {
	int manX : 6;
	int manY : 6;
	int dir : 2;
	bool boxMoved : 1;
} MOVE;

#define MAX_UNDO 40
#define UNDO_SPEED 7

extern MOVE undoStack[MAX_UNDO];
extern int undoTop;
extern int undoing;

void initUndo();

int undoCount();
void pushUndo(int px, int py, int dir, bool boxMoved);
bool undoLastMove();

#endif
// EOF
