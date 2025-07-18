#include "deadlock.h"
#include "attribute.h"
#include <stdbool.h>

bool deadlock;

// Check if a position is a wall
bool is_wall(unsigned char *p) { return *p == CH_BRICKWALL; }

// Check if a position is a goal
bool is_goal(unsigned char *p) { return Attribute[CharToType[*p]] & ATT_TARGETLIKE; }

// Check if a position has a box NOT on a target
bool is_box(unsigned char *p) {

	int type = CharToType[*p];
	return (type != TYPE_BOX_LOCKED && type != TYPE_BOX_CORRECT &&
	        Attribute[CharToType[*p]] & ATT_BOX);
}

// Detect static corner deadlock
void is_corner_deadlock() {
	// if (is_goal(y, x))
	// 	return false; // goals are allowed
	bool wall_up = is_wall(me - _1ROW);
	bool wall_down = is_wall(me + _1ROW);
	bool wall_left = is_wall(me - 1);
	bool wall_right = is_wall(me + 1);

	bool thisDead =
	    // Any 90° corner
	    (wall_up && wall_left) || (wall_up && wall_right) || (wall_down && wall_left) ||
	    (wall_down && wall_right);

	if (thisDead) {
		*me = CH_BOX_DEADLOCK;
		deadlock = true;
	}
}

// Detect 2x2 box cluster deadlock
void is_box_block() {

	bool thisDead = is_box(me + _1ROW) && is_box(me + 1) && is_box(me + _1ROW + 1);

	if (thisDead) {
		*(me + _1ROW) = CH_BOX_DEADLOCK;
		*(me + _1ROW + 1) = CH_BOX_DEADLOCK;
		*(me + 1) = CH_BOX_DEADLOCK;
		*(me) = CH_BOX_DEADLOCK;
		deadlock = true;
	}
}

void checkDeadlocks() {
	is_corner_deadlock();
	is_box_block();
}
