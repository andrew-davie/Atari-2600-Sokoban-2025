#ifndef __UNDO_H
#define __UNDO_H

#include <stdbool.h>

typedef struct {
	unsigned char manX, manY;
	unsigned char *from;
	unsigned char *to;
} MOVE;

#define MAX_UNDO 50
#define UNDO_SPEED 7

extern MOVE undoStack[MAX_UNDO];
extern int undoTop;
extern int undoing;

void initUndo();

int undoCount();
void pushUndo(int px, int py, unsigned char *from, unsigned char *to);
bool undoLastMove();

#endif
// EOF
