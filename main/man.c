// clang-format off

#include "defines.h"
#include "main.h"
#include "man.h"

#include "player.h"
#include "sound.h"

// clang-format on

int manX;
int manY;
int frameAdjustX;
int frameAdjustY;

enum FaceDirection manFaceDirection;
bool manDead;
static bool handled;

int undoCount;
unsigned char undo[1000];

enum DIR {
	DIR_UP = 1,
	DIR_DOWN = 2,
	DIR_LEFT = 4,
	DIR_RIGHT = 8,
};

const signed char xInc[] = {

    // RLDU
    0,  // 0000
    0,  // 0001
    0,  // 0010
    0,  // 0011
    -1, // 0100
    -1, // 0101
    -1, // 0110
    0,  // 0111
    1,  // 1000
    1,  // 1001
    1,  // 1010
    0,  // 1011
    0,  // 1100
    0,  // 1101
    0,  // 1110
    0,  // 1111
};

const signed char yInc[] = {

    // RLDU
    0,  // 0000
    -1, // 0001
    1,  // 0010
    0,  // 0011
    0,  // 0100
    -1, // 0101
    1,  // 0110
    0,  // 0111
    0,  // 1000
    -1, // 1001
    1,  // 1010
    0,  // 1011
    0,  // 1100
    0,  // 1101
    0,  // 1110
    0,  // 1111
};

const unsigned char joyDirectBit[] = {
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN,
};

const signed char faceDirection[] = {
    FACE_LEFT,
    FACE_RIGHT,
    0,
    0,
};

const signed char animDeltaX[] = {
    1,
    1,
    0,
    0,
};

const signed char animDeltaY[] = {
    0,
    0,
    -1,
    1,
};

void initMan() {

	manDead = false;

	manFaceDirection = FACE_RIGHT;

	startAnimation(animationList[ANIM_PLAYER], ID_Stand);

	undoCount = 0;
	for (int i = 0; i < sizeof(undo); i++)
		undo[i] = 0;
}

const signed char dirOffset[] = {-1, 1, -_1ROW, +_1ROW};

void moveMan() { handled = false; }

// EOF