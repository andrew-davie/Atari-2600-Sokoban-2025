#ifndef __UNDO_H
#define __UNDO_H

#include <stdbool.h>

#define MAX_UNDO 100
#define UNDO_SPEED 6

extern unsigned short undoStack[MAX_UNDO];
extern int undoTop;
extern int undoing;

void initUndo();

int undoCount();
void pushUndo(int px, int py, int dir, bool boxMoved);
bool undoLastMove();

#endif
// EOF
