#ifndef __BITSHAPES_H
#define __BITSHAPES_H

#include "main.h"

// clang-format off

enum FRAME {

    FRAME_BLANK,                // 00
    FRAME_STAND,                // 01
    FRAME_ARMS_IN_AIR,          // 02
    FRAME_HUNCH,                // 03
    FRAME_HAIR,                 // 04
    FRAME_PUSH,                 // 05
    FRAME_PUSH2,                // 06
    FRAME_HAIR2,                // 07
    FRAME_IMPATIENT,            // 08
    FRAME_SHADES,               // 09
    FRAME_SHADES_ARM,           // 10
    FRAME_LOOK1,                // 11
    FRAME_LOOK2,                // 12
    FRAME_BLINK,                // 13
    FRAME_WALK1,                // 14
    FRAME_WALK2,                // 15
    FRAME_WALK3,                // 16
    FRAME_WALK4,                // 17
    FRAME_SNATCH_DOWN,          // 18
    FRAME_SKELETON,             // 19
    FRAME_SKELETON4,            // 20
    FRAME_ARMSCROSSED,          // 21
    FRAME_WALKDOWN1,            // 22
    FRAME_WALKDOWN2,            // 23
    FRAME_WALKDOWN3,            // 24
    FRAME_WALKUP1,              // 25
    FRAME_WALKUP2,              // 26
    FRAME_WALKUP3,              // 27

    // actions start after frames

    ACTION_PUSH,
    ACTION_JOYSTICK,
    ACTION_MOVE,
    ACTION_POSITION,
    ACTION_DELAY,
    ACTION_FLIP,
    ACTION_LOOP,
    ACTION_AUTOFRAME,
    ACTION_NEXTSQUARE,
    ACTION_STOP,

};

#define PUSHX    ACTION_PUSH
#define JOYSTICK    ACTION_JOYSTICK
#define POSITION(x,y)    ACTION_POSITION,x,y,
#define DELAY(n)    ACTION_DELAY,n,
#define FLIP   ACTION_FLIP
#define LOOP    ACTION_LOOP,
#define SERVER(server)    ACTION_AUTOFRAME,server,
#define STOP    ACTION_STOP,

// clang-format on

enum AnimationIdent {

	ID_Stand,       // 00
	ID_PushRight,   // 01
	ID_WipeHair,    // 02
	ID_Blink,       // 03
	ID_Shades,      // 04
	ID_Die,         // 05
	ID_Walk,        // 06
	ID_PushLeft,    // 07
	ID_PushDown,    // 08
	ID_PushUp,      // 09
	ID_Skeleton,    // 10
	ID_Skeleton2,   // 11
	ID_ArmsCrossed, // 12
	ID_EndPush2,    // 13
	ID_Drip,        // 14
	ID_Talk2,       // 15
	ID_Locked,      // 16
	ID_WalkUp,      // 17
	ID_WalkDown,    // 18
	ID_StandUp,     // 19
	ID_StandDown,   // 20
	ID_ArmsUp,      // 21
	                //
	AUTO_Walk,      // 22
	AUTO_WalkUp,    // 23
	AUTO_WalkDown,  // 24
};

extern enum AnimationIdent playerAnimationID;

void initAnimations();

extern const signed char *const AnimationVector[]; // animJames[];
extern const int AnimationIndex[];

// extern const signed char animationList[player]->animation;
// extern const signed char animationList[player]->animationLoop;
// extern unsigned int playerAnimationCount;

extern const signed char playerBigSprite[][2 + SPRITE_DEPTH * 3];
extern const unsigned char redirect[];

typedef struct Animation {
	struct Animation *frameServer;
	int frame;
	int count;
	const signed char *animation;
	const signed char *loop;
} Animation;

extern Animation *animationList[];

#define ANIM_PLAYER 0
#define ANIM_AUTO_WALK 1
#define ANIM_AUTO_WALKUP 2
#define ANIM_AUTO_WALKDOWN 3
#define ANIM_AUTO_PUSH 4

void startAnimation(Animation *anim, enum AnimationIdent id);
void processAnimation(Animation *anim);
void updateAnimations();

#endif
// EOF