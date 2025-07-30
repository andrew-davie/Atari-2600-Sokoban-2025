
#include "deadlock.h"
#include "attribute.h"
#include "colour.h"
#include "main.h"
#include "man.h"
#include "score.h"
#include "sound.h"
#include <stdbool.h>

int deadlock;
int lastDeadlock;

// int deadlockCounter;

// Check if a position is a wall
bool isWall(unsigned char *p) { return *p == CH_BRICKWALL; }

// Check if a position is a goal
bool isGoal(unsigned char *p) { return Attribute[CharToType[*p]] & ATT_TARGETLIKE; }

bool isBox(unsigned char *p) { return Attribute[CharToType[*p]] & ATT_BOX; }

bool isUnplacedBox(unsigned char *p) {
	return (isBox(p) && !(Attribute[CharToType[*p]] & ATT_TARGETLIKE));
}

bool isBoxorWall(unsigned char *p) { return (Attribute[CharToType[*p]] & ATT_DEADLOCK); }

bool isUnplacedBoxorWall(unsigned char *p) { return isBoxorWall(p) && !isGoal(p); }

bool validate(unsigned char *offset[4]) {

	bool dead = isUnplacedBox(offset[0]) && isBoxorWall(offset[1]) && isBoxorWall(offset[2]) &&
	            isBoxorWall(offset[3]);
	if (dead) {
		// axiom: offset[0] and offset[1] ARE boxes
		for (int i = 0; i < 4; i++)
			if (isUnplacedBox(offset[i]))
				*offset[i] = CH_BOX_DEADLOCK;
	}

	return dead;
}

void checkDeadlocks() {

	if (*me == CH_BOX_DEADLOCK)
		*me = CH_BOX;

	//      BOX 0       BOX 1       BOX 2       BOX 3
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	// -1  | |W| |     | |W| |     | | | |     | | | |   -_1ROW
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	//  0  |W[B] |     | [B]W|     |W[B] |     | [B]W|   0
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	// +1  | | | |     | | | |     | |W| |     | |W| |   +_1ROW
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	//
	//     -1 0 1      -1 0 1      -1 0 1      -1 0 1
	//
	// [ ] = "me"
	//  W  = wall

	bool wall_up = isWall(me - _1ROW);
	bool wall_down = isWall(me + _1ROW);
	bool wall_left = isWall(me - 1);
	bool wall_right = isWall(me + 1);

	bool dead = !isGoal(me) && ((wall_up && wall_left) || (wall_up && wall_right) ||
	                            (wall_down && wall_left) || (wall_down && wall_right));

	// Against-wall options

	//      0/4/3       1/4/5       2/3/8       BOX 3
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	// -1  | | | |     | |X|X|     | | | |     | | | |   -_1ROW
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	//  0  |B[B] |     | [B]B|     |X[B] |     | [B]X|   0
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	// +1  |X|X| |     | | | |     |X|B| |     | |B|X|   +_1ROW
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	//
	//      BOX 4       BOX 5       BOX 6       BOX 7
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	// -1  |X|X| |     | | | |     |X|B| |     | | | |   -_1ROW
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	//  0  |B[B] |     | [B]B|     |X[B] |     |X[B] |   0
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	// +1  | | | |     | |x|X|     | | | |     |X|B| |   +_1ROW
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	//     -1 0 1      -1 0 1      -1 0 1      -1 0 1

	//      BOX 8       BOX 9       BOX A       BOX B
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	// -1  | |B|X|     | | | |     | | | |     | | | |   -_1ROW
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	//  0  | [B]X|     | [B] |     | [B] |     | [B] |   0
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	// +1  | | | |     | | | |     | | | |     | | | |   +_1ROW
	//     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
	//     -1 0 1      -1 0 1      -1 0 1      -1 0 1

	// [] = "me"

	unsigned char *block[][4] = {
	    {me, me - 1, me + _1ROW - 1, me + _1ROW}, {me, me + 1, me - _1ROW, me - _1ROW + 1},
	    {me, me + _1ROW, me - 1, me + _1ROW - 1}, {me, me + _1ROW, me + 1, me + _1ROW + 1},
	    {me, me - 1, me - _1ROW - 1, me - _1ROW}, {me, me + 1, me + _1ROW, me + _1ROW + 1},
	    {me, me - _1ROW, me - 1, me - _1ROW - 1}, {me, me + _1ROW, me - 1, me + _1ROW - 1},
	    {me, me - _1ROW, me + 1, me - _1ROW + 1},

	};

	for (unsigned int i = 0; i < sizeof(block) / sizeof(block[0]); i++)
		dead |= validate(block[i]);

	if (dead) {
		// if (deadlockCounter < 1)
		// 	deadlockCounter = 100;

		deadlock++;

		if (isUnplacedBox(me))
			*me = CH_BOX_DEADLOCK;
	}
}
