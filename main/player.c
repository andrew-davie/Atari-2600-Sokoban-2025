
#include "defines.h"
#include "defines_cdfj.h"
#include "defines_from_dasm_for_c.h"
#include <stdbool.h>

#include "drawplayer.h"
#include "main.h"

#include "animations.h"
#include "atarivox.h"
#include "attribute.h"
#include "bitpatterns.h"
#include "colour.h"
#include "joystick.h"
#include "man.h"
#include "player.h"
#include "random.h"
#include "rooms.h"
#include "score.h"
#include "scroll.h"
#include "sound.h"
#include "undo.h"

unsigned char pillFlagFrom, pillFlagTo;

unsigned char originType;

// clang-format off

#define PUSH_UP 2
#define PUSH_DOWN 3
#define PUSH_LEFT 0
#define PUSH_RIGHT 1



const signed char AnimationDefault[] = {
    FRAME_STAND,1,
    STOP
};

const signed char AnimationUndo[] = {
//    ACTION_FLIP,                      // CANNOT flip during undo due to manFaceDirection requrements

    FRAME_STAND, 20,
    LOOP

    FRAME_LOOK1, 3,
    FRAME_LOOK2, 21,
    FRAME_LOOK1, 3,
    FRAME_STAND, 3,
    LOOP
};

//------------------------------------------------------------------------------

const signed char FrameServer_Walk[] = {

    FRAME_WALK1, 3,
    FRAME_WALK2, 3,
    FRAME_WALK3, 3,
    FRAME_WALK4, 3,
    LOOP
};

Animation autoWalk = {
    0,
    FRAME_BLANK,
    0,
    FrameServer_Walk,
    FrameServer_Walk,
};

const signed char AnimationWalk[] = {

    SERVER(ANIM_AUTO_WALK)

    POSITION(1,0)
    DELAY(1)
    POSITION(2,0)
    DELAY(1)
    POSITION(3,0)
    DELAY(1)
    POSITION(4,0)
    DELAY(1)
    POSITION(5,0)
    DELAY(1)
    POSITION(6,0)
    DELAY(1)
    POSITION(7,0)
    DELAY(1)
    POSITION(8,0)
    DELAY(1)
    POSITION(9,0)
    DELAY(1)
    POSITION(10,0)
    DELAY(1)
    POSITION(11,0)
    DELAY(1)
    POSITION(12,0)
    DELAY(1)
    POSITION(13,0)
    DELAY(1)
    POSITION(14,0)
    DELAY(1)
    POSITION(15,0)
    DELAY(1)
    POSITION(16,0)
    DELAY(1)
    POSITION(17,0)
    DELAY(1)
    POSITION(18,0)
    DELAY(1)
    POSITION(19,0)
    DELAY(1)
    POSITION(20,0)
    DELAY(1)

    ACTION_NEXTSQUARE,1,0,
    STOP
};

//------------------------------------------------------------------------------

const signed char FrameServer_WalkDown[] = {

    FRAME_WALKDOWN1, 6,
    FRAME_WALKDOWN2, 6,
    FRAME_WALKDOWN1, 6,
    FRAME_WALKDOWN3, 6,
    LOOP
};

Animation autoWalkDown = {
    0,
    FRAME_BLANK,
    0,
    FrameServer_WalkDown,
    FrameServer_WalkDown,
};

const signed char AnimationWalkDown[] = {

    SERVER(ANIM_AUTO_WALKDOWN)

    POSITION(0,-1)
    DELAY(1)
    POSITION(0,-2)
    DELAY(1)
    // POSITION(0,-3)
    // DELAY(1)
    POSITION(0,-4)
    DELAY(1)
    POSITION(0,-5)
    DELAY(1)
    // POSITION(0,-6)
    // DELAY(1)
    POSITION(0,-7)
    DELAY(1)
    POSITION(0,-8)
    DELAY(1)
    POSITION(0,-9)
    DELAY(1)

    POSITION(0,-1-9)
    DELAY(1)
    POSITION(0,-2-9)
    DELAY(1)
    // POSITION(0,-3-9)
    // DELAY(1)
    POSITION(0,-4-9)
    DELAY(1)
    POSITION(0,-5-9)
    DELAY(1)
    // POSITION(0,-6-9)
    // DELAY(1)
    POSITION(0,-7-9)
    DELAY(1)
    POSITION(0,-8-9)
    DELAY(1)
    POSITION(0,-9-9)
    DELAY(1)

    POSITION(0,-1-18)
    DELAY(1)
    POSITION(0,-2-18)
    DELAY(1)
    // POSITION(0,-3-18)
    // DELAY(1)
    POSITION(0,-4-18)
    DELAY(1)
    POSITION(0,-5-18)
    DELAY(1)
    // POSITION(0,-6-18)
    // DELAY(1)
    POSITION(0,-7-18)
    DELAY(1)
    POSITION(0,-8-18)
    DELAY(1)
    POSITION(0,-9-18)
    DELAY(1)
    
    ACTION_NEXTSQUARE,0,1,
    STOP
};

//------------------------------------------------------------------------------

const signed char FrameServer_WalkUp[] = {

    FRAME_WALKUP1, 4,
    FRAME_WALKUP2, 4,
    FRAME_WALKUP1, 4,
    FRAME_WALKUP3, 4,
    LOOP
};

Animation autoWalkUp = {
    0,
    FRAME_BLANK,
    0,
    FrameServer_WalkUp,
    FrameServer_WalkUp,
};


const signed char AnimationWalkUp[] = {

    SERVER(ANIM_AUTO_WALKUP)

    POSITION(0,1)
    DELAY(1)
    POSITION(0,2)
    DELAY(1)
    // POSITION(0,3)
    // DELAY(1)
    POSITION(0,4)
    DELAY(1)
    POSITION(0,5)
    DELAY(1)
    // POSITION(0,6)
    // DELAY(1)
    POSITION(0,7)
    DELAY(1)
    POSITION(0,8)
    DELAY(1)
    POSITION(0,9)
    DELAY(1)

    POSITION(0,1+9)
    DELAY(1)
    POSITION(0,2+9)
    DELAY(1)
    // POSITION(0,3+9)
    // DELAY(1)
    POSITION(0,4+9)
    DELAY(1)
    POSITION(0,5+9)
    DELAY(1)
    // POSITION(0,6+9)
    // DELAY(1)
    POSITION(0,7+9)
    DELAY(1)
    POSITION(0,8+9)
    DELAY(1)
    POSITION(0,9+9)
    DELAY(1)

    POSITION(0,1+18)
    DELAY(1)
    POSITION(0,2+18)
    DELAY(1)
    // POSITION(0,3+18)
    // DELAY(1)
    POSITION(0,4+18)
    DELAY(1)
    POSITION(0,5+18)
    DELAY(1)
    // POSITION(0,6+18)
    // DELAY(1)
    POSITION(0,7+18)
    DELAY(1)
    POSITION(0,8+18)
    DELAY(1)
    POSITION(0,9+18)
    DELAY(1)


    ACTION_NEXTSQUARE,0,-1,
    STOP
};




Animation playerUndo = {
    0, FRAME_SKELETON, 0, AnimationUndo, AnimationUndo
};



Animation playerAnim = {
    0,
    FRAME_BLANK,
    0,
    AnimationDefault,
    AnimationDefault,
};

const signed char AnimationArmsUp[] = {
    FRAME_ARMS_IN_AIR,255
};

const signed char AnimationStandUp[] = {
    FRAME_WALKUP1,1,
    ACTION_LOOP
};

const signed char AnimationStandDown[] = {
//    FRAME_WALKDOWN1,2,
    FRAME_STAND,1,
    ACTION_LOOP
};



const signed char FrameServer_Push[] = {

    FRAME_PUSH, 4,
    FRAME_PUSH2, 4,
    LOOP
};

Animation autoPush = {
    0,
    FRAME_BLANK,
    0,
    FrameServer_Push,
    FrameServer_Push,
};


// const signed char AnimationPush[] = {

//     SERVER(ANIM_AUTO_PUSH)
//     ACTION_PUSH, PUSH_RIGHT,
//     DELAY(20)
//     STOP
// };

const signed char AnimationLocked[] = {
    FRAME_HUNCH, 3,
    ACTION_STOP,
};

const signed char AnimationEndPush2[] = {
    // ACTION_POSITION, 4, 0,
    // FRAME_PUSH2, 3,
    // ACTION_POSITION, 2, 0,
    // ACTION_POSITION,12,0,
    //     FRAME_ARMS_IN_AIR,12,
//    ACTION_POSITION -3,0,

//    ACTION_POSITION, 6, 0,
//    FRAME_PUSH,1,
    // ACTION_POSITION, 5, 0,
    // FRAME_PUSH,1,
    // ACTION_POSITION, 10, 0,
    // FRAME_PUSH,10,
    // ACTION_POSITION, 5, 0,
    // FRAME_ARMS_IN_AIR,5,
    FRAME_PUSH,4,

    ACTION_MOVE, 1, 0,
    //      FRAME_HUNCH, 3,
    // ACTION_POSITION, -15, 0,
    // FRAME_STAND,30,
    ACTION_STOP,
};

// #if ENABLE_SHAKE
// const signed char AnimationShake[] = {
//     ACTION_POSITION, 0,0,
//     FRAME_SHAKE,10,
//     FRAME_SHAKE2,10,
//     FRAME_SHAKE3,10,
//     ACTION_LOOP,
//     ACTION_STOP,
// };
// #endif

const signed char AnimationWipeHair[] = {
    //    FRAME_WIPE_HAIR, 22,
    //    FRAME_HAIR, 64,
    FRAME_HAIR2, 4,
    FRAME_HAIR, 4,
    FRAME_HAIR2, 4,
    FRAME_HAIR, 4,
    ACTION_STOP,
};

// const signed char AnimationHoldBoulder[] = {
//     FRAME_ARMS_IN_AIR, 10,
//     ACTION_LOOP,
//     FRAME_HOLD_BOULDER_1, 5,
//     ACTION_STOP,
// };

// const signed char AnimationStoop[] = { //=jump
//     ACTION_POSITION, 0,0,
//     FRAME_HUNCH, 1,
//     ACTION_LOOP,
//     ACTION_STOP,
// };

// const signed char AnimationImpatient[] = {
//     FRAME_ARMSCROSSED, 50,
//     FRAME_IMPATIENT, 10,
//     FRAME_IMPATIENT2, 10,
//     FRAME_IMPATIENT, 10,
//     FRAME_IMPATIENT2, 10,
//     FRAME_IMPATIENT, 10,
//     FRAME_IMPATIENT2, 10,
//     // FRAME_IMPATIENT, 10,
//     // FRAME_IMPATIENT2, 10,
//     ACTION_STOP,
// };

// const signed char AnimationLook[] = {
//     ACTION_POSITION, -1,0,
//     FRAME_LOOK1, 5,
//     ACTION_POSITION, -1,0,
//     FRAME_LOOK2, 20,
//     ACTION_POSITION, -1,0,
//     FRAME_LOOK1, 10,
//     FRAME_MOON,10,
//     ACTION_FLIP,
//     FRAME_MOON,10,
//     ACTION_FLIP,
//     FRAME_MOON,10,
//     ACTION_FLIP,
//     FRAME_MOON,10,
//     ACTION_FLIP,
//     FRAME_MOON,10,
//     ACTION_FLIP,
//     FRAME_MOON,10,
//     ACTION_FLIP,
//     FRAME_MOON,10,
//     ACTION_FLIP,
//     FRAME_LOOK1, 10,
//     ACTION_STOP,
// };

const signed char AnimationBlink[] = {
    FRAME_BLINK, 6,
    ACTION_STOP,
};

// const signed char AnimationTurn[] = {
//     FRAME_LOOK1, 4,
//     FRAME_LOOK2, 4,
//     ACTION_FLIP,
//     ACTION_STOP,
// };

const signed char AnimationShades[] = {


    FRAME_STAND, 50,
    FRAME_SHADES_ARM, 40,
    FRAME_SHADES, 100,
    FRAME_SHADES, 100,
    FRAME_SHADES, 100,
    FRAME_SHADES, 100,
    FRAME_SHADES_ARM, 25,
    FRAME_STAND, 100,
    FRAME_STAND, 100,
    FRAME_LOOK1, 6,
    FRAME_LOOK2, 6,
    ACTION_FLIP,
    FRAME_STAND, 100,
    FRAME_STAND, 100,
    FRAME_STAND, 100,

    FRAME_LOOK1, 6,
    FRAME_LOOK2, 6,
    ACTION_FLIP,
    ACTION_LOOP
};

const signed char AnimationDie[] = {

    FRAME_ARMS_IN_AIR, 10,
    FRAME_HUNCH,5,
    FRAME_SKELETON,  25,
    ACTION_STOP,
};



// const signed char AnimationPushRight[] = {

//     ACTION_PUSH, PUSH_RIGHT,

//     ACTION_POSITION, 5, 0,
//     FRAME_PUSH, 4,
//     ACTION_POSITION, 3, 0,
//     FRAME_PUSH, 6,
//     ACTION_POSITION, 0,0,
//     FRAME_IMPATIENT, 8,
//     ACTION_STOP,
// };


#define PUSH(n) \
    ACTION_POSITION, 4*n+5, 0, \
    DELAY(1) \
    ACTION_POSITION, 4*n+5 +1, 0, \
    DELAY(1) \
    ACTION_POSITION, 4*n+5+2, 0, \
    DELAY(1)

const signed char AnimationPushLeft[] = {

    ACTION_PUSH, PUSH_LEFT,
    SERVER(ANIM_AUTO_PUSH)

    PUSH(0)
    PUSH(1)
    PUSH(2)
    PUSH(3)
    PUSH(4)
    
    ACTION_MOVE, -1, 0,
    ACTION_STOP,
};


const signed char AnimationPushRight[] = {

    ACTION_PUSH, PUSH_RIGHT,
    SERVER(ANIM_AUTO_PUSH)

    PUSH(0)
    PUSH(1)
    PUSH(2)
    PUSH(3)
    PUSH(4)
    
    ACTION_MOVE, 1, 0,
    ACTION_STOP,
};


#define PUSHDOWN(n) \
    FRAME_SNATCH_DOWN, 2, \
    ACTION_POSITION, 0, -3*n -7,

const signed char AnimationPushDown[] = {

        ACTION_PUSH, PUSH_DOWN,

    PUSHDOWN(0)
    PUSHDOWN(1)
    PUSHDOWN(2)
    PUSHDOWN(3)
    PUSHDOWN(4)
    PUSHDOWN(5)
    PUSHDOWN(6)
    PUSHDOWN(7)
    PUSHDOWN(8)

    ACTION_MOVE, 0, 1,
    ACTION_STOP,
};

#define PUSHUP(n) \
    FRAME_ARMS_IN_AIR, 2, \
    ACTION_POSITION, 0,  3 * n + 9,


const signed char AnimationPushUp[] = {

    ACTION_PUSH, PUSH_UP,

    PUSHUP(0)
    PUSHUP(1)
    PUSHUP(2)
    PUSHUP(3)
    PUSHUP(4)
    PUSHUP(5)
    PUSHUP(6)
    PUSHUP(7)
    PUSHUP(8)

    ACTION_MOVE, 0, -1,
    ACTION_STOP,
};

const signed char AnimationSkeleton2[] = {
    FRAME_SKELETON4, 8,
    FRAME_BLANK, 4, // FRAME_SKELETON, 8,
    ACTION_LOOP,
//    ACTION_STOP,
};

const signed char AnimationSkeleton[] = {

    //FRAME_SKELETON,  20,
    //FRAME_SKELETON4, 20,
//    FRAME_SKELETON3, 8,
    //FRAME_SKELETON2, 8,
    //FRAME_SKELETON5, 20,
    FRAME_ARMS_IN_AIR,     255,
    //    ACTION_LOOP,
    //    ACTION_STOP,
};

// const signed char AnimationStartup[] = {
//      FRAME_SKELETON5, 8,
//      FRAME_SKELETON2, 8,
//      FRAME_SKELETON3, 8,
//      FRAME_SKELETON, 15,

// #define DX 4

//     FRAME_STAND,DX,
//     FRAME_SKELETON, DX,
//     FRAME_STAND,DX,
//     FRAME_SKELETON, DX,
//     FRAME_STAND,DX,
//     FRAME_SKELETON, DX,
//     FRAME_STAND,DX,
//     FRAME_SKELETON, DX,

// FRAME_BLANK, 90,
// FRAME_STAND, DX,
// FRAME_BLANK, DX,
// FRAME_STAND, DX,
// FRAME_BLANK, DX,
// FRAME_STAND, DX,
// FRAME_BLANK, DX,
// FRAME_STAND, DX,
// FRAME_BLANK, DX,

// FRAME_STAND, 1,
// ACTION_STOP

// FRAME_STAND,10,

// ACTION_POSITION, -1,0,
// FRAME_LOOK1, 5,
// ACTION_POSITION, -1,0,
// FRAME_LOOK2, 20,
// ACTION_POSITION, -1,0,
// FRAME_LOOK1, 5,
// ACTION_POSITION, 0,0,
//    FRAME_STAND, 255,
//    ACTION_STOP,
// };

const signed char AnimationArmsCrossed[] = {
    FRAME_ARMSCROSSED, 80,
    ACTION_STOP,
};

const signed char AnimationTalk2[] = {
FRAME_HAIR, 10,
FRAME_BLINK, 10,
ACTION_STOP,
};

// #if __ENABLE_DRIP

const signed char AnimationDrip[] = {
    FRAME_IMPATIENT, 15,
    FRAME_STAND, 5,
    FRAME_HAIR2, 6,
    FRAME_HAIR, 6,
    FRAME_HAIR2, 6,
    FRAME_HAIR,  6,
    ACTION_STOP,
};

// const signed char AnimationDrip2[] = {
// //    FRAME_IMPATIENT,25,
// //    FRAME_STAND,10,
//     ACTION_POSITION, -1,0,
//     FRAME_LOOK1, 5,
//     ACTION_POSITION, -1,0,
//     FRAME_LOOK2, 15,
//     ACTION_POSITION, -1,0,
//     FRAME_LOOK1, 5,
// //    ACTION_POSITION, 0,0,
// //    FRAME_ARMSCROSSED, 90,
//     ACTION_STOP,
// };

// #endif // ENABLE_DRIP

// clang-format on

const signed char *const AnimationVector[] = {

    AnimationDefault,     // 00
    AnimationPushRight,   // 01
    AnimationWipeHair,    // 02
    AnimationBlink,       // 03
    AnimationShades,      // 04
    AnimationDie,         // 05
    AnimationWalk,        // 06
    AnimationPushLeft,    // 07
    AnimationPushDown,    // 08
    AnimationPushUp,      // 09
    AnimationSkeleton,    // 10
    AnimationSkeleton2,   // 11
    AnimationArmsCrossed, // 12
    AnimationEndPush2,    // 13
    AnimationDrip,        // 14
    AnimationTalk2,       // 15
    AnimationLocked,      // 16
    AnimationWalkUp,      // 17
    AnimationWalkDown,    // 18
    AnimationStandUp,     // 19
    AnimationStandDown,   // 20
    AnimationArmsUp,      // 21
    AnimationUndo,        // 22
    //
    FrameServer_Walk,     // 23
    FrameServer_WalkUp,   // 24
    FrameServer_WalkDown, // 25
    FrameServer_Push,     // 26

};

// clang-format off

enum AnimationIdent playerAnimationID = ID_Stand;
//const signed char animationList[player]->animation = AnimationDefault;
//const signed char animationList[player]->animationLoop = AnimationDefault;
//unsigned int playerAnimationCount = 0;

#define HAIR 0
#define SKIN 1
#define TOP1 2
#define TOP2 3
#define BOOT 4
#define PANT 5
#define BELT 6
#define SOLE 5
#define BONE 8

// const unsigned char redirect[] = {0, 1, 1, 2};
const unsigned char playerColour[] = {

    0x22, // 0 HAIR
    0x38, // 1 SKIN
    0x96, // 2 TOP1
    0xD6, // 3 TOP2
    0x24, // 4 BOOT
    0x64, // 5 PANT
    0x08, // 6 BELT
    0x08, // 5 SOLE
    0x08, // 8 BONE

};


#define SP(a, b, c, d) a, b, (c << 4) + d

const signed char playerBigSprite[][2 + SPRITE_DEPTH * 3] = {

    // Format
    // X, Y offsets
    // [GRP0, GRP1, COLUP0, COLUP1,]      * (CHAR_HEIGHT+0)

    {
        // FRAME_BLANK = 23

        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

        // 0,
        // 0,

        // SP(________, ________, ____, ____), // 00
        // SP(________, ________, ____, ____), // 01
        // SP(________, ________, ____, ____), // 02
        // SP(________, ________, ____, ____), // 03
        // SP(________, ________, ____, ____), // 04
        // SP(________, ________, ____, ____), // 05
        // SP(________, ________, ____, ____), // 06
        // SP(________, ________, ____, ____), // 05
        // SP(________, ________, ____, ____), // 08
        // SP(________, ________, ____, ____), // 09
        // SP(________, ________, ____, ____), // 10
        // SP(________, ________, ____, ____), // 11
        // SP(________, ________, ____, ____), // 12
        // SP(________, ________, ____, ____), // 13
        // SP(________, ________, ____, ____), // 14
        // SP(________, ________, ____, ____), // 15
        // SP(________, ________, ____, ____), // 16
        // SP(________, ________, ____, ____), // 15
        // SP(________, ________, ____, ____), // 18
        // SP(________, ________, ____, ____), // 19
        // SP(________, ________, ____, ____), // 20
        // SP(________, ________, ____, ____), // 21
        // SP(________, ________, ____, ____), // 22
        // SP(________, ________, ____, ____), // 21
        // SP(________, ________, ____, ____), // 22
        // SP(________, ________, ____, ____), // 21
        // SP(________, ________, ____, ____), // 22
        // SP(________, ________, ____, ____), // 22
        // SP(________, ________, ____, ____), // 21
        // SP(________, ________, ____, ____), // 22

    },

    {
        // FRAME_STAND= 0

        2 + 0,
        0,

        SP(____XXX_, ________, HAIR, HAIR), // 00
        SP(___XXXXX, X_______, HAIR, HAIR), // 01
        SP(__XXXXXX, XX______, HAIR, HAIR), // 02
        SP(__XXXXXX, ________, SKIN, HAIR), // 03
        SP(__XXXXXX, ________, SKIN, HAIR), // 04
        SP(__XX_X_X, ________, SKIN, HAIR), // 05
        SP(__XXXXXX, ________, SKIN, HAIR), // 06
        SP(__XXXXX_, ________, SKIN, SKIN), // 05
        SP(___X____, ________, SKIN, SKIN), // 08
        SP(___XXXX_, ________, SKIN, SKIN), // 09
        SP(__XXXXXX, ________, TOP1, TOP1), // 10
        SP(_XXXXXXX, X_______, TOP2, TOP2), // 11
        SP(_XXXXXXX, X_______, TOP1, TOP1), // 12
        SP(XX_XXXX_, XX______, TOP2, TOP2), // 13
        SP(XX_XXXX_, XX______, TOP1, TOP1), // 14
        SP(___XXXX_, XX______, TOP2, SKIN), // 15
        SP(XX_XXXX_, ________, BELT, BELT), // 16
        SP(XX_XXXXX, ________, PANT, PANT), // 15
        SP(___XX_XX, ________, PANT, PANT), // 18
        SP(__XXX_XX, ________, PANT, PANT), // 19
        SP(__XX__XX, ________, BOOT, BOOT), // 20
        SP(__XXX_XX, X_______, BOOT, BOOT), // 21
        SP(__XXX_XX, X_______, SOLE, SOLE), // 22

    },

    {
        // FRAME_ARMS_IN_AIR = 1

        -3 + 0,
        -6,

        SP(________, _XXX____, HAIR, HAIR), // 00
        SP(________, _XXXX___, HAIR, HAIR), // 01
        SP(_____XX_, XXXXXX__, SKIN, HAIR), // 02
        SP(_____XX_, XXXXXXXX, SKIN, HAIR), // 03
        SP(____XX__, XXXXX_XX, TOP2, HAIR), // 04
        SP(____XX__, XXXXX_XX, TOP1, HAIR), // 05
        SP(____XX__, XXXXX_XX, TOP2, SKIN), // 06
        SP(____XX__, XXXXX_XX, TOP1, SKIN), // 05
        SP(____XXX_, XXXX__XX, TOP2, SKIN), // 08
        SP(_____XXX, _XXX_XX_, TOP1, SKIN), // 09
        SP(______XX, XXXXXXX_, TOP1, TOP1), // 10
        SP(_______X, XXXXXXX_, TOP2, TOP2), // 11
        SP(_______X, XXXXXX__, TOP1, TOP1), // 12
        SP(________, XXXXX___, TOP2, TOP2), // 13
        SP(________, XXXX____, TOP1, TOP1), // 14
        SP(________, XXXX____, TOP2, TOP2), // 15
        SP(________, XXXX____, BELT, BELT), // 16
        SP(_______X, XXXXX___, PANT, PANT), // 15
        SP(_______X, XXXXX___, PANT, PANT), // 18
        SP(_______X, X__XX___, PANT, PANT), // 19
        SP(______XX, X__XXX__, BOOT, BOOT), // 20
        SP(______XX, X___XX__, BOOT, BOOT), // 21
        SP(______XX, X___XXX_, SOLE, SOLE), // 22

    },

    // {   // FRAME_HOLD_BOULDER_1 = 2

    //     2+0, 0,   // X,Y OFFSET

    //     _XX_XXXX,_XX_____, HAIR, BELT,     //00
    //     _XX_XXXX,_XX_____, HAIR, BELT,     //01
    //     ___XXXXX,________, HAIR, HAIR,     //02
    //     XX_XXXXX,_XX_____, SKIN, SKIN,     //03
    //     XX_X_X_X,_XX_____, SKIN, SKIN,     //04
    //     XX_XXXXX,_XX_____, SKIN, SKIN,     //05
    //     XX_XX_XX,_XX_____, SKIN, SKIN,     //06
    //     XX_XXXXX,_XX_____, SKIN, SKIN,     //05
    //     XXX_XXX_,_XX_____, SKIN, SKIN,     //08
    //     _XXXXXXX,XX______, SKIN, SKIN,     //09
    //     __XXXXXX,XX______, TOP1, TOP1,     //10
    //     __XXXXXX,X_______, TOP2, TOP2,     //11
    //     ___XXXXX,________, TOP1, ____,     //12
    //     ___XXXX_,________, TOP2, ____,     //13
    //     ___XXXX_,________, TOP1, ____,     //14
    //     ___XXXX_,________, TOP2, ____,     //15
    //     ___XXXX_,________, BELT, ____,     //16
    //     ___XXXX_,________, PANT, ____,     //15
    //     ___XXXXX,________, PANT, ____,     //18
    //     __XXX_XX,________, PANT, ____,     //19
    //     __XXX_XX,X_______, BOOT, BOOT,     //20
    //     __XXX__X,X_______, BOOT, BOOT,     //21
    //     __XXX__X,XX______, SOLE, SOLE,     //22

    //     -4,0,    // x,y

    //     _X_XX_X_, HAIR,    // 0
    //     _X_X_X__, SKIN,    // 1
    //     _X___X__, TOP2,    // 2
    //     _XXXX___, TOP1,    // 3
    //     __XXX___, TOP2,    // 4
    //     __XX____, TOP1,    // 5
    //     __XX____, TOP2,    // 6
    //     __X_X___, PNT2,    // 5
    //     _XX_X___, BOOT,    // 8

    // },

    // {   // FRAME_WIPE_HAIR = 3

    //     2+0, 0,

    //     ____XXX_,________, HAIR, ____,     //00
    //     ___XXXX_,________, HAIR, ____,     //01
    //     __XXXXXX,________, HAIR, ____,     //02
    //     __XXXXXX,X_______, HAIR, HAIR,     //03
    //     __XXXXXX,XX______, SKIN, HAIR,     //04
    //     __X_X_X_,XX______, SKIN, SKIN,     //05
    //     __XXXXX_,XX______, SKIN, TOP1,     //06
    //     __XXXXX_,XX______, SKIN, TOP2,     //05
    //     ___X_XX_,XX______, SKIN, TOP1,     //08
    //     ___XXXX_,XX______, SKIN, TOP2,     //09
    //     __XXXXXX,XX______, TOP1, TOP1,     //10
    //     _XXXXXXX,X_______, TOP2, TOP2,     //11
    //     _XXXXXXX,________, TOP1, ____,     //12
    //     XX_XXXX_,________, TOP2, ____,     //13
    //     XX_XXXX_,________, TOP1, ____,     //14
    //     ___XXXX_,________, TOP2, ____,     //15
    //     XX_XXXX_,________, BELT, ____,     //16
    //     XX_XXXXX,________, PANT, ____,     //15
    //     ___XX_XX,________, PANT, ____,     //18
    //     __XXX_XX,________, PANT, ____,     //19
    //     __XX__XX,________, BOOT, BOOT,     //20
    //     __XXX_XX,X_______, BOOT, BOOT,     //21
    //     __XXX_XX,X_______, SOLE, SOLE,     //22

    // },

    {
        // FRAME_HUNCH = 4

        3 + 0,
        0,

        SP(________, ________, ____, ____), // 00
        SP(____XXXX, ________, HAIR, HAIR), // 00
        SP(___XXXXX, X_______, HAIR, HAIR), // 01
        SP(__XXXXXX, XX______, HAIR, HAIR), // 02
        SP(__XXXXXX, ________, SKIN, HAIR), // 03
        SP(__XXXXXX, ________, SKIN, HAIR), // 04
        SP(__XX_X_X, ________, SKIN, HAIR), // 05
        SP(__XXXXXX, ________, SKIN, SKIN), // 06
        SP(__XXXXX_, ________, SKIN, SKIN), // 05
        SP(___XX_X_, ________, SKIN, SKIN), // 08
        SP(__XXXXX_, ________, TOP1, TOP1), // 10
        SP(_XXXXXX_, ________, TOP2, TOP2), // 11
        SP(XXXXXXXX, ________, TOP1, TOP1), // 12
        SP(XXXXXXXX, ________, TOP2, TOP2), // 13
        SP(XXX_XXXX, ________, TOP1, TOP1), // 14
        SP(XXX_XX_X, X_______, TOP2, SKIN), // 15
        SP(_XXXXX_X, X_______, BELT, BELT), // 16
        SP(__XXXXX_, X_______, PANT, SKIN), // 15
        SP(__XXX___, ________, PANT, PANT), // 18
        SP(__XXX___, ________, PANT, PANT), // 19
        SP(__XXX___, ________, BOOT, BOOT), // 20
        SP(__XXXX__, ________, BOOT, BOOT), // 21
        SP(__XXXX__, ________, SOLE, SOLE), // 22

    },

    {
        // FRAME_HAIR = 5

        2 + 0,
        0,

        SP(____XXX_, ________, HAIR, HAIR), // 00
        SP(___XXXXX, ________, HAIR, HAIR), // 01
        SP(__XXXXXX, X_______, HAIR, HAIR), // 02
        SP(__XXXXXX, X_______, SKIN, HAIR), // 03
        SP(__XXXXXX, ________, SKIN, SKIN), // 04
        SP(__XX_X_X, ________, SKIN, SKIN), // 05
        SP(__XXXXXX, ________, SKIN, SKIN), // 06
        SP(__XXXXXX, ________, SKIN, SKIN), // 05
        SP(___XX_X_, ________, SKIN, SKIN), // 08
        SP(____XXX_, ________, SKIN, SKIN), // 09
        SP(__XXXXXX, X_______, TOP1, TOP1), // 10
        SP(_XXXXXXX, X_______, TOP2, TOP2), // 11
        SP(_XXXXXXX, XX______, TOP1, TOP1), // 12
        SP(XX_XXXX_, XX______, TOP2, TOP2), // 13
        SP(XX_XXXX_, XX______, TOP1, TOP1), // 14
        SP(___XXXX_, XX______, TOP2, SKIN), // 15
        SP(XX_XXXX_, ________, BELT, BELT), // 16
        SP(XX_XXXXX, ________, PANT, PANT), // 15
        SP(___XX_XX, ________, PANT, PANT), // 18
        SP(__XXX_XX, ________, PANT, PANT), // 19
        SP(__XX__XX, ________, BOOT, BOOT), // 20
        SP(__XXX_XX, X_______, BOOT, BOOT), // 21
        SP(__XXX_XX, X_______, SOLE, SOLE), // 22

    },

    {
        // FRAME_PUSH = 6

        1,
        0,

        SP(________, XXXX____, HAIR, HAIR), // 00
        SP(_______X, XXXXX___, HAIR, HAIR), // 01
        SP(______XX, XXXXXXX_, HAIR, HAIR), // 02
        SP(______XX, XXXXX___, HAIR, HAIR), // 03
        SP(______XX, XXXX____, HAIR, SKIN), // 04
        SP(______XX, XX_X____, HAIR, SKIN), // 05
        SP(______XX, XXXX____, SKIN, SKIN), // 06
        SP(______XX, XX______, SKIN, SKIN), // 05
        SP(_______X, XXX_____, SKIN, SKIN), // 08
        SP(_____XXX, XXXXXXXX, TOP2, TOP2), // 09
        SP(_____XXX, XXX____X, TOP1, TOP1), // 10
        SP(____XXXX, XXXXXXXX, TOP2, TOP2), // 11
        SP(____XXXX, XXXXXXX_, TOP1, TOP1), // 12
        SP(____XXXX, XX______, TOP2, TOP2), // 13
        SP(___XXXXX, ________, TOP1, TOP1), // 14
        SP(___XXXXX, ________, TOP2, TOP2), // 15
        SP(___XXXXX, ________, BELT, BELT), // 16
        SP(__XXXXXX, ________, PANT, PANT), // 15
        SP(__XXX_XX, X_______, PANT, PANT), // 18
        SP(_XXX___X, XX______, PANT, PANT), // 19
        SP(XXX_____, XX______, BOOT, BOOT), // 20
        SP(XX______, XX______, BOOT, BOOT), // 21
        SP(XXX_____, XXX_____, SOLE, SOLE), // 22

    },

    {
        // FRAME_PUSH2 = 5

        2,
        0,

        SP(________, XXXX____, HAIR, HAIR), // 00
        SP(_______X, XXXXX___, HAIR, HAIR), // 01
        SP(______XX, XXXXXXX_, HAIR, HAIR), // 02
        SP(______XX, XXXXX___, HAIR, HAIR), // 03
        SP(______XX, XXXX____, HAIR, SKIN), // 04
        SP(______XX, XX_X____, HAIR, SKIN), // 05
        SP(______XX, XXXX____, SKIN, SKIN), // 06
        SP(______XX, XX______, SKIN, SKIN), // 05
        SP(_______X, XXX_____, SKIN, SKIN), // 08
        SP(_____XXX, XXXXXXXX, TOP2, TOP2), // 09
        SP(_____XXX, XXX____X, TOP1, TOP1), // 10
        SP(____XXXX, XXXXXXXX, TOP2, TOP2), // 11
        SP(____XXXX, XXXXXXX_, TOP1, TOP1), // 12
        SP(____XXXX, XX______, TOP2, TOP2), // 13
        SP(___XXXXX, ________, TOP1, TOP1), // 14
        SP(___XXXXX, ________, TOP2, TOP2), // 15
        SP(___XXXXX, ________, BELT, BELT), // 16
        SP(____XXXX, ________, PANT, PANT), // 15
        SP(____XXXX, ________, PANT, PANT), // 18
        SP(_____XXX, ________, PANT, PANT), // 19
        SP(____XXXX, ________, BOOT, BOOT), // 20
        SP(_____XX_, ________, BOOT, BOOT), // 21
        SP(_____XXX, ________, SOLE, SOLE), // 22

    },

    {
        // FRAME_HAIR2 = 8

        2 + 0,
        0,

        SP(____XXX_, ________, HAIR, HAIR), // 00
        SP(___XXXXX, ________, HAIR, HAIR), // 01
        SP(__XXXXXX, ________, HAIR, HAIR), // 02
        SP(__XXXXXX, ________, SKIN, HAIR), // 03
        SP(__XXXXXX, ________, SKIN, SKIN), // 04
        SP(__X_X_XX, ________, SKIN, SKIN), // 05
        SP(__XXXXXX, ________, SKIN, SKIN), // 06
        SP(___XXXXX, ________, SKIN, SKIN), // 05
        SP(___X_XX_, ________, SKIN, SKIN), // 08
        SP(____XXX_, ________, SKIN, SKIN), // 09
        SP(__XXXXXX, X_______, TOP1, TOP1), // 10
        SP(_XXXXXXX, X_______, TOP2, TOP2), // 11
        SP(_XXXXXXX, XX______, TOP1, TOP1), // 12
        SP(XX_XXXX_, XX______, TOP2, TOP2), // 13
        SP(XX_XXXX_, XX______, TOP1, TOP1), // 14
        SP(___XXXX_, XX______, TOP2, SKIN), // 15
        SP(XX_XXXX_, ________, BELT, BELT), // 16
        SP(XX_XXXXX, ________, PANT, PANT), // 15
        SP(___XX_XX, ________, PANT, PANT), // 18
        SP(__XXX_XX, ________, PANT, PANT), // 19
        SP(__XX__XX, ________, BOOT, BOOT), // 20
        SP(__XXX_XX, X_______, BOOT, BOOT), // 21
        SP(__XXX_XX, X_______, SOLE, SOLE), // 22

    },

    {
        // FRAME_IMPATIENT = 9

        0 + 2,
        0,

        SP(___XXXX_, ________, HAIR, HAIR), // 00
        SP(__XXXXXX, X_______, HAIR, HAIR), // 01
        SP(_XXXXXXX, X_______, HAIR, HAIR), // 02
        SP(_XXXXXX_, ________, SKIN, HAIR), // 03
        SP(_XXXXXX_, ________, SKIN, SKIN), // 04
        SP(_XX_X_X_, ________, SKIN, SKIN), // 05
        SP(_XXXXXX_, ________, SKIN, SKIN), // 06
        SP(_XXXXX__, ________, SKIN, SKIN), // 05
        SP(__XX_X__, ________, SKIN, SKIN), // 08
        SP(___XXXX_, ________, SKIN, TOP2), // 09
        SP(__XXXXXX, ________, TOP1, TOP1), // 10
        SP(__XXXXXX, X_______, TOP2, TOP2), // 11
        SP(_XXXXXXX, XX______, TOP1, TOP1), // 12
        SP(XXXXXXXX, XX______, TOP2, TOP2), // 13
        SP(XX_XXXX_, XX______, TOP1, SKIN), // 14
        SP(XX_XXXXX, ________, TOP2, TOP2), // 15
        SP(___XXXXX, ________, BELT, BELT), // 16
        SP(XX_XXXXX, ________, PANT, PANT), // 15
        SP(XX_XX_XX, ________, PANT, PANT), // 18
        SP(__XXX_XX, XX______, PANT, BOOT), // 19
        SP(__XX__XX, XX______, BOOT, BOOT), // 20
        SP(__XXX___, ________, BOOT, SOLE), // 21
        SP(__XXX___, ________, SOLE, SOLE), // 22

    },

    // {
    //     // FRAME_IMPATIENT2 = 10

    //     0 + 2,
    //     0,

    //     SP(___XXXX_, ________, HAIR, HAIR), // 00
    //     SP(__XXXXXX, ________, HAIR, HAIR), // 01
    //     SP(_XXXXXXX, X_______, HAIR, HAIR), // 02
    //     SP(_XXXXXX_, ________, SKIN, HAIR), // 03
    //     SP(_XXXXXX_, ________, SKIN, SKIN), // 04
    //     SP(_XX_X_X_, ________, SKIN, SKIN), // 05
    //     SP(_XXXXXX_, ________, SKIN, SKIN), // 06
    //     SP(_XXXXX__, ________, SKIN, SKIN), // 05
    //     SP(__XX_X__, ________, SKIN, SKIN), // 08
    //     SP(___XXXX_, ________, SKIN, SKIN), // 09
    //     SP(__XXXXXX, ________, TOP1, TOP1), // 10
    //     SP(__XXXXXX, X_______, TOP2, TOP2), // 11
    //     SP(_XXXXXXX, XX______, TOP1, TOP1), // 12
    //     SP(XXXXXXXX, XX______, TOP2, TOP2), // 13
    //     SP(XX_XXXX_, XX______, TOP1, SKIN), // 14
    //     SP(XX_XXXX_, ________, TOP2, TOP2), // 15
    //     SP(___XXXXX, ________, BELT, BELT), // 16
    //     SP(XX_XXXXX, ________, PANT, PANT), // 15
    //     SP(XX_XX_XX, XX______, PANT, BOOT), // 18
    //     SP(__XXX_XX, XX______, BOOT, BOOT), // 19
    //     SP(__XX____, ________, BOOT, BOOT), // 20
    //     SP(__XXX___, ________, BOOT, BOOT), // 21
    //     SP(__XXX___, ________, SOLE, SOLE), // 22
    // },

    // {
    //     // FRAME_LOOK1 = 11

    //     3,
    //     0,

    //     SP(____XXX_, ________, HAIR, HAIR), // 00
    //     SP(___XXXX_, ________, HAIR, HAIR), // 01
    //     SP(__XXXXXX, X_______, HAIR, HAIR), // 02
    //     SP(__XXXXXX, XX______, SKIN, HAIR), // 03
    //     SP(__XXXXXX, ________, SKIN, HAIR), // 04
    //     SP(__X_X_XX, ________, SKIN, HAIR), // 05
    //     SP(__XXXXXX, ________, SKIN, SKIN), // 06
    //     SP(__XXXXXX, ________, SKIN, SKIN), // 05
    //     SP(___X_XX_, ________, SKIN, SKIN), // 08
    //     SP(___XXXX_, ________, SKIN, SKIN), // 09
    //     SP(__XXXXXX, ________, TOP1, TOP1), // 10
    //     SP(_XXXXXXX, X_______, TOP2, TOP2), // 11
    //     SP(_XXXXXXX, XX______, TOP1, TOP1), // 12
    //     SP(_XX_XXXX, XX______, TOP2, TOP2), // 13
    //     SP(_XXXXXX_, XX______, TOP1, TOP1), // 14
    //     SP(___XXXX_, XX______, TOP2, SKIN), // 15
    //     SP(_XXXXXX_, ________, BELT, BELT), // 16
    //     SP(___XXXX_, ________, PANT, PANT), // 15
    //     SP(___XXXX_, ________, PANT, PANT), // 18
    //     SP(___XXXXX, ________, PANT, PANT), // 19
    //     SP(___XX_XX, ________, BOOT, BOOT), // 20
    //     SP(___XX_XX, X_______, BOOT, BOOT), // 21
    //     SP(___XX_XX, X_______, SOLE, SOLE), // 22

    // },

    // {
    //     // FRAME_LOOK2 = 12

    //     2,
    //     0,

    //     SP(___XXXXX, ________, HAIR, HAIR), // 00
    //     SP(__XXXXXX, X_______, HAIR, HAIR), // 01
    //     SP(_XXXXXXX, X_______, HAIR, HAIR), // 02
    //     SP(___XXXXX, X_______, SKIN, HAIR), // 03
    //     SP(__XXXXXX, X_______, SKIN, HAIR), // 04
    //     SP(__X_X_XX, X_______, SKIN, HAIR), // 05
    //     SP(__XXXXXX, X_______, SKIN, HAIR), // 06
    //     SP(___XXXXX, ________, SKIN, SKIN), // 05
    //     SP(_____XXX, ________, SKIN, SKIN), // 08
    //     SP(____XXXX, ________, SKIN, SKIN), // 09
    //     SP(___XXXXX, XX______, TOP1, TOP1), // 10
    //     SP(__XXXXXX, XXX_____, TOP2, TOP2), // 11
    //     SP(__XXXXXX, XXX_____, TOP1, TOP1), // 12
    //     SP(__XX_XXX, _XX_____, TOP2, TOP2), // 13
    //     SP(__XXXXXX, __XX____, TOP1, TOP1), // 14
    //     SP(____XXXX, __XX____, TOP2, SKIN), // 15
    //     SP(__XXXXXX, ________, BELT, BELT), // 16
    //     SP(____XXXX, ________, PANT, PANT), // 15
    //     SP(____XXXX, ________, PANT, PANT), // 18
    //     SP(____XXXX, ________, PANT, PANT), // 19
    //     SP(____XX_X, X_______, BOOT, BOOT), // 20
    //     SP(____XX_X, X_______, BOOT, BOOT), // 21
    //     SP(___XXX_X, X_______, SOLE, SOLE), // 22
    // },

    {
        // FRAME_SHADES = 13

        2 + 0,
        0,

        SP(____XXXX, ________, HAIR, HAIR), // 00
        SP(___XXXXX, X_______, HAIR, HAIR), // 01
        SP(__XXXXXX, XX______, HAIR, HAIR), // 02
        SP(__XXXXXX, ________, SKIN, HAIR), // 03
        SP(__XXXXXX, ________, SKIN, HAIR), // 04
        SP(________, ________, SKIN, HAIR), // 05
        SP(__XX_X_X, ________, SKIN, HAIR), // 06
        SP(__XXXXX_, ________, SKIN, SKIN), // 05
        SP(___XX_X_, ________, SKIN, SKIN), // 08
        SP(___XXXX_, ________, SKIN, SKIN), // 09
        SP(__XXXXXX, X_______, TOP1, TOP1), // 10
        SP(_XXXXXXX, XX______, TOP2, TOP2), // 11
        SP(_XXXXXXX, XX______, TOP1, TOP1), // 12
        SP(XX_XXXX_, XX______, TOP2, TOP2), // 13
        SP(XX_XXXX_, _XX_____, TOP1, TOP1), // 14
        SP(___XXXX_, _XX_____, TOP2, SKIN), // 15
        SP(XX_XXXX_, ________, BELT, BELT), // 16
        SP(XX_XXXXX, ________, PANT, PANT), // 15
        SP(___XX_XX, ________, PANT, PANT), // 18
        SP(__XXX_XX, ________, PANT, PANT), // 19
        SP(__XX__XX, ________, BOOT, BOOT), // 20
        SP(__XXX_XX, X_______, BOOT, BOOT), // 21
        SP(__XXX_XX, X_______, SOLE, SOLE), // 22

    },

    {
        // FRAME_SHADES_ARM = 14

        2 + 0,
        0,

        SP(____XXX_, ________, HAIR, HAIR), // 00
        SP(___XXXXX, ________, HAIR, HAIR), // 01
        SP(__XXXXXX, ________, HAIR, HAIR), // 02
        SP(__XXXXXX, X_______, HAIR, HAIR), // 03
        SP(__XXXXXX, X_______, SKIN, HAIR), // 04
        SP(________, XX______, SKIN, SKIN), // 05
        SP(__XX_X_X, XX______, SKIN, SKIN), // 06
        SP(__XXXXX_, XX______, SKIN, TOP2), // 05
        SP(___XX_X_, XX______, SKIN, TOP1), // 08
        SP(___XXXX_, XX______, SKIN, TOP2), // 09
        SP(__XXXXXX, XX______, TOP1, TOP1), // 10
        SP(_XXXXXXX, X_______, TOP2, TOP2), // 11
        SP(_XXXXXXX, ________, TOP1, TOP1), // 12
        SP(XX_XXXX_, ________, TOP2, TOP2), // 13
        SP(XX_XXXX_, ________, TOP1, TOP1), // 14
        SP(___XXXX_, ________, TOP2, TOP2), // 15
        SP(XX_XXXX_, ________, BELT, BELT), // 16
        SP(XX_XXXXX, ________, PANT, PANT), // 15
        SP(___XX_XX, ________, PANT, PANT), // 18
        SP(__XXX_XX, ________, PANT, PANT), // 19
        SP(__XX__XX, ________, BOOT, BOOT), // 20
        SP(__XXX_XX, X_______, BOOT, BOOT), // 21
        SP(__XXX_XX, X_______, SOLE, SOLE), // 22

    },

        {
        // FRAME_LOOK1 = 11

        2,
        0,

        SP(____XXX_, ________, HAIR, HAIR), // 00
        SP(___XXXX_, ________, HAIR, HAIR), // 01
        SP(__XXXXXX, X_______, HAIR, HAIR), // 02
        SP(__XXXXXX, XX______, SKIN, HAIR), // 03
        SP(__XXXXXX, ________, SKIN, HAIR), // 04
        SP(__X_X_XX, ________, SKIN, HAIR), // 05
        SP(__XXXXXX, ________, SKIN, SKIN), // 06
        SP(__XXXXXX, ________, SKIN, SKIN), // 05
        SP(___X_XX_, ________, SKIN, SKIN), // 08
        SP(___XXXX_, ________, SKIN, SKIN), // 09
        SP(__XXXXXX, ________, TOP1, TOP1), // 10
        SP(_XXXXXXX, X_______, TOP2, TOP2), // 11
        SP(_XXXXXXX, XX______, TOP1, TOP1), // 12
        SP(_XX_XXXX, XX______, TOP2, TOP2), // 13
        SP(_XXXXXX_, XX______, TOP1, TOP1), // 14
        SP(___XXXX_, XX______, TOP2, SKIN), // 15
        SP(_XXXXXX_, ________, BELT, BELT), // 16
        SP(___XXXX_, ________, PANT, PANT), // 15
        SP(___XXXX_, ________, PANT, PANT), // 18
        SP(___XXXXX, ________, PANT, PANT), // 19
        SP(___XX_XX, ________, BOOT, BOOT), // 20
        SP(___XX_XX, X_______, BOOT, BOOT), // 21
        SP(___XX_XX, X_______, SOLE, SOLE), // 22

    },

    {
        // FRAME_LOOK2 = 12

        2,
        0,

        SP(___XXXXX, ________, HAIR, HAIR), // 00
        SP(__XXXXXX, X_______, HAIR, HAIR), // 01
        SP(_XXXXXXX, X_______, HAIR, HAIR), // 02
        SP(___XXXXX, X_______, SKIN, HAIR), // 03
        SP(__XXXXXX, X_______, SKIN, HAIR), // 04
        SP(__X_X_XX, X_______, SKIN, HAIR), // 05
        SP(__XXXXXX, X_______, SKIN, HAIR), // 06
        SP(___XXXXX, ________, SKIN, SKIN), // 05
        SP(_____XXX, ________, SKIN, SKIN), // 08
        SP(____XXXX, ________, SKIN, SKIN), // 09
        SP(___XXXXX, XX______, TOP1, TOP1), // 10
        SP(__XXXXXX, XXX_____, TOP2, TOP2), // 11
        SP(__XXXXXX, XXX_____, TOP1, TOP1), // 12
        SP(__XX_XXX, _XX_____, TOP2, TOP2), // 13
        SP(__XXXXXX, __XX____, TOP1, TOP1), // 14
        SP(____XXXX, __XX____, TOP2, SKIN), // 15
        SP(__XXXXXX, ________, BELT, BELT), // 16
        SP(____XXXX, ________, PANT, PANT), // 15
        SP(____XXXX, ________, PANT, PANT), // 18
        SP(____XXXX, ________, PANT, PANT), // 19
        SP(____XX_X, X_______, BOOT, BOOT), // 20
        SP(____XX_X, X_______, BOOT, BOOT), // 21
        SP(___XXX_X, X_______, SOLE, SOLE), // 22
    },

    {
        // FRAME_BLINK= 15

        2 + 0,
        0,

        SP(____XXX_, ________, HAIR, HAIR), // 00
        SP(___XXXXX, ________, HAIR, HAIR), // 01
        SP(__XXXXXX, X_______, HAIR, HAIR), // 02
        SP(__XXXXXX, XX______, SKIN, HAIR), // 03
        SP(__XXXXXX, ________, SKIN, HAIR), // 04
        SP(__XXXXXX, ________, SKIN, HAIR), // 05
        SP(__XXXXXX, ________, SKIN, SKIN), // 06
        SP(__XXXXX_, ________, SKIN, SKIN), // 05
        SP(___X____, ________, SKIN, SKIN), // 08
        SP(___XXXX_, ________, SKIN, SKIN), // 09
        SP(__XXXXXX, ________, TOP1, TOP1), // 10
        SP(_XXXXXXX, X_______, TOP2, TOP2), // 11
        SP(_XXXXXXX, X_______, TOP1, TOP1), // 12
        SP(XX_XXXX_, XX______, TOP2, TOP2), // 13
        SP(XX_XXXX_, XX______, TOP1, TOP1), // 14
        SP(___XXXX_, XX______, TOP2, SKIN), // 15
        SP(XX_XXXX_, ________, BELT, BELT), // 16
        SP(XX_XXXXX, ________, PANT, PANT), // 15
        SP(___XX_XX, ________, PANT, PANT), // 18
        SP(__XXX_XX, ________, PANT, PANT), // 19
        SP(__XX__XX, ________, BOOT, BOOT), // 20
        SP(__XXX_XX, X_______, BOOT, BOOT), // 21
        SP(__XXX_XX, X_______, SOLE, SOLE), // 22

    },

    {
        // FRAME_WALK1 = 16

        3,
        -2,

        SP(_____XXX, X_______, HAIR, HAIR), // 00
        SP(____XXXX, XX______, HAIR, HAIR), // 01
        SP(___XXXXX, XX______, HAIR, HAIR), // 02
        SP(___XXXXX, XXX_____, SKIN, HAIR), // 03
        SP(___XXXXX, X_______, SKIN, HAIR), // 04
        SP(___XX_X_, X_______, SKIN, HAIR), // 05
        SP(___XXXXX, X_______, SKIN, HAIR), // 06
        SP(___XXXXX, ________, SKIN, SKIN), // 05
        SP(____XX_X, ________, SKIN, SKIN), // 08
        SP(___XXXX_, ________, SKIN, SKIN), // 09
        SP(__XXXXXX, X_______, TOP1, TOP1), // 10
        SP(_XXXXXXX, XX______, TOP2, TOP2), // 11
        SP(XXXXXXXX, XX______, TOP1, TOP1), // 12
        SP(XX_XXXX_, XX______, TOP2, TOP2), // 13
        SP(XX_XXXX_, _XX_____, TOP1, TOP1), // 14
        SP(___XXXXX, _XX_____, TOP2, SKIN), // 15
        SP(XX_XXXXX, ________, BELT, BELT), // 16
        SP(__XXXXXX, ________, PANT, PANT), // 15
        SP(__XXX_XX, X_______, PANT, PANT), // 18
        SP(_XXX___X, XX______, PANT, PANT), // 19
        SP(XXX_____, XX______, BOOT, BOOT), // 20
        SP(XX______, XX______, BOOT, BOOT), // 21
        SP(XXX_____, XXX_____, SOLE, SOLE), // 22

    },

    {
        // FRAME_WALK2 = 15

        3,
        0,

        SP(_____XXX, X_______, HAIR, HAIR), // 00
        SP(____XXXX, XX______, HAIR, HAIR), // 01
        SP(___XXXXX, XX______, HAIR, HAIR), // 02
        SP(___XXXXX, XXX_____, SKIN, HAIR), // 03
        SP(___XXXXX, X_______, SKIN, HAIR), // 04
        SP(___XX_X_, X_______, SKIN, HAIR), // 05
        SP(___XXXXX, X_______, SKIN, HAIR), // 06
        SP(___XXXXX, ________, SKIN, SKIN), // 05
        SP(____XX_X, ________, SKIN, SKIN), // 08
        SP(___XXXX_, ________, SKIN, SKIN), // 09
        SP(__XXXXXX, ________, TOP1, TOP1), // 10
        SP(__XXXXXX, ________, TOP2, TOP2), // 11
        SP(_XXXXXXX, X_______, TOP2, TOP2), // 12
        SP(__XXXXXX, X_______, TOP1, TOP1), // 13
        SP(___XXXXX, X_______, TOP2, SKIN), // 14
        SP(__X_XXX_, ________, BELT, BELT), // 15
        SP(__XXXXXX, ________, PANT, PANT), // 16
        SP(__XXXXXX, ________, PANT, PANT), // 15
        SP(__XXX_XX, X_______, PANT, PANT), // 18
        SP(_XXX___X, XX______, BOOT, PANT), // 19
        SP(XX______, XX______, BOOT, BOOT), // 20
        SP(XXX_____, XX______, SOLE, BOOT), // 21
        SP(________, XXX_____, SOLE, SOLE), // 22

    },

    {
        // FRAME_WALK3 = 18

        3,
        1,

        SP(_____XXX, X_______, HAIR, HAIR), // 00
        SP(____XXXX, XX______, HAIR, HAIR), // 01
        SP(___XXXXX, XX______, HAIR, HAIR), // 02
        SP(___XXXXX, XXX_____, SKIN, HAIR), // 03
        SP(___XXXXX, X_______, SKIN, SKIN), // 04
        SP(___XX_X_, X_______, SKIN, SKIN), // 05
        SP(___XXXXX, X_______, SKIN, SKIN), // 06
        SP(___XXXXX, ________, SKIN, SKIN), // 05
        SP(____XX_X, ________, SKIN, SKIN), // 08
        SP(___XXXX_, ________, SKIN, SKIN), // 09
        SP(__XXXXXX, ________, TOP1, TOP1), // 10
        SP(__XXXXXX, X_______, TOP2, TOP2), // 11
        SP(_XXXXXXX, X_______, TOP1, TOP1), // 12
        SP(_XX_XXX_, X_______, TOP2, TOP1), // 13
        SP(_XX_XXX_, X_______, TOP1, TOP1), // 14
        SP(____XXXX, X_______, TOP2, SKIN), // 15
        SP(__XX_XX_, ________, BELT, BELT), // 16
        SP(____XXXX, ________, PANT, PANT), // 15
        SP(____XXXX, ________, PANT, PANT), // 18
        SP(__XX_XXX, ________, PANT, PANT), // 19
        SP(__XX_XXX, ________, BOOT, BOOT), // 20
        SP(_____XX_, ________, BOOT, BOOT), // 21
        SP(_____XXX, ________, SOLE, SOLE), // 22

    },

    {
        // FRAME_WALK4 = 19

        3,
        0,

        SP(_____XXX, X_______, HAIR, HAIR), // 00
        SP(____XXXX, XX______, HAIR, HAIR), // 01
        SP(___XXXXX, XX______, HAIR, HAIR), // 02
        SP(___XXXXX, XXX_____, HAIR, HAIR), // 03
        SP(___XXXXX, X_______, SKIN, SKIN), // 04
        SP(___XX_X_, X_______, SKIN, SKIN), // 05
        SP(___XXXXX, X_______, SKIN, SKIN), // 06
        SP(___XXXXX, ________, SKIN, SKIN), // 05
        SP(____XX_X, ________, SKIN, SKIN), // 08
        SP(___XXXX_, ________, SKIN, SKIN), // 09
        SP(__XXXXXX, X_______, TOP1, TOP1), // 10
        SP(_XXXXXXX, XX______, TOP2, TOP2), // 11
        SP(XXXXXXXX, XX______, TOP1, TOP1), // 12
        SP(XX_XXXX_, XX______, TOP2, TOP2), // 13
        SP(XX_XXXX_, _XX_____, TOP1, TOP1), // 14
        SP(___XXXXX, _XX_____, TOP2, SKIN), // 15
        SP(XX_XXXXX, ________, BELT, BELT), // 16
        SP(____XXXX, ________, PANT, PANT), // 15
        SP(____XXXX, ________, PANT, PANT), // 18
        SP(_____XXX, X_______, PANT, BOOT), // 19
        SP(____XXXX, X_______, BOOT, BOOT), // 20
        SP(_____XX_, ________, BOOT, BOOT), // 21
        SP(_____XXX, ________, SOLE, SOLE), // 22

    },

    {
        // FRAME_SNATCH_DOWN = 20

        2 + 0,
        0,

        SP(________, ________, ____, ____), // 00
        SP(________, ________, ____, ____), // 01
        SP(________, ________, ____, ____), // 02
        SP(________, ________, ____, ____), // 03
        SP(________, ________, ____, ____), // 04
        SP(________, ________, ____, ____), // 05
        SP(________, ________, ____, ____), // 06
        SP(____XXXX, XX______, HAIR, HAIR), // 05
        SP(___XXXXX, XXX_____, HAIR, HAIR), // 08
        SP(___XXXXX, X_______, SKIN, SKIN), // 09
        SP(___XXXXX, X_______, SKIN, SKIN), // 10
        SP(___XX_X_, X_______, SKIN, SKIN), // 11
        SP(___XXXXX, X_______, SKIN, SKIN), // 12
        SP(___XXXXX, ________, SKIN, SKIN), // 13
        SP(____XX_X, ________, SKIN, SKIN), // 14
        SP(__XXXXXX, X_______, TOP1, TOP1), // 15
        SP(__XXXXXX, X_______, TOP2, TOP2), // 16
        SP(__XXXXXX, X_______, TOP1, TOP1), // 15
        SP(__XX_XXX, X_______, TOP2, TOP2), // 18
        SP(___XX_XX, X_______, TOP1, TOP1), // 19
        SP(___XXXXX, X_______, TOP2, TOP2), // 20
        SP(__XXX_XX, X_______, BELT, SKIN), // 21
        SP(__XXX_XX, X_______, SOLE, SKIN), // 22

    },

    {
        // FRAME_SKELETON = 21

        0,
        0,

        SP(________, ________, ____, ____), // 00
        SP(________, ________, ____, ____), // 01
        SP(_____XXX, X_______, BONE, BONE), // 02
        SP(____XXXX, XX______, BONE, BONE), // 03
        SP(____X__X, __X_____, BONE, BONE), // 04
        SP(____X__X, __X_____, BONE, BONE), // 05
        SP(____XXXX, XX______, BONE, BONE), // 06
        SP(_____X_X, _X______, BONE, BONE), // 05
        SP(______X_, X_______, BONE, BONE), // 08
        SP(_______X, ________, BONE, BONE), // 09
        SP(____XXXX, XX______, BONE, BONE), // 10
        SP(___X___X, __X_____, BONE, BONE), // 11
        SP(__XX_XXX, X_XX____, BONE, BONE), // 12
        SP(__X____X, ___X____, BONE, BONE), // 13
        SP(_____XXX, XX______, BONE, BONE), // 14
        SP(__XX__XX, X_XX____, BONE, BONE), // 15
        SP(_X___X_X, _X__X___, BONE, BONE), // 16
        SP(_____X__, _X______, BONE, BONE), // 15
        SP(____XX__, _XX_____, BONE, BONE), // 18
        SP(_____X__, _X______, BONE, BONE), // 19
        SP(________, ________, BONE, BONE), // 20
        SP(____XXX_, XXX_____, BONE, BONE), // 21
        SP(________, ________, ____, ____), // 22

    },

    // {
    //     // FRAME_SKELETON2 = 22

    //     -1,
    //     0,

    //     SP(________, ________, ____, ____), // 00
    //     SP(________, ________, ____, ____), // 01
    //     SP(________, ________, ____, ____), // 02
    //     SP(________, ________, ____, ____), // 03
    //     SP(________, ________, ____, ____), // 04
    //     SP(________, ________, ____, ____), // 05
    //     SP(________, ________, ____, ____), // 06
    //     SP(________, ________, ____, ____), // 05
    //     SP(________, ________, ____, ____), // 08
    //     SP(________, ________, ____, ____), // 09
    //     SP(________, ________, ____, ____), // 10
    //     SP(________, XXXX____, BONE, BONE), // 11
    //     SP(_______X, XXXXX___, BONE, BONE), // 12
    //     SP(_______X, __X__X__, BONE, BONE), // 13
    //     SP(_______X, __X__X__, BONE, BONE), // 14
    //     SP(_______X, XXXXX___, BONE, BONE), // 15
    //     SP(________, X_X_X___, BONE, BONE), // 16
    //     SP(________, _X_X____, BONE, BONE), // 15
    //     SP(_____X__, XXXX____, BONE, BONE), // 18
    //     SP(___X___X, ________, BONE, BONE), // 19
    //     SP(_______X, XX______, BONE, BONE), // 20
    //     SP(___X__X_, ______X_, BONE, BONE), // 21
    //     SP(__XX__XX, X_XXX__X, BONE, BONE), // 22

    // },

    // {
    //     // FRAME_SKELETON3 = 24

    //     0,
    //     0,

    //     SP(________, ________, ____, ____), // 00
    //     SP(________, ________, ____, ____), // 01
    //     SP(________, ________, ____, ____), // 02
    //     SP(________, ________, ____, ____), // 03
    //     SP(________, ________, ____, ____), // 04
    //     SP(________, ________, ____, ____), // 05
    //     SP(_______X, XXX_____, BONE, BONE), // 06
    //     SP(______XX, XXXX____, BONE, BONE), // 05
    //     SP(______X_, _X__X___, BONE, BONE), // 08
    //     SP(______X_, _X__X___, BONE, BONE), // 09
    //     SP(______XX, XXXX____, BONE, BONE), // 10
    //     SP(_______X, _X_X____, BONE, BONE), // 11
    //     SP(________, X_X_____, BONE, BONE), // 12
    //     SP(__X_XXX_, XXX_____, BONE, BONE), // 13
    //     SP(_X____X_, ________, BONE, BONE), // 14
    //     SP(_X__XXXX, ___X____, BONE, BONE), // 15
    //     SP(______XX, __XX____, BONE, BONE), // 16
    //     SP(_X__X_X_, X_______, BONE, BONE), // 15
    //     SP(_X__X___, X___XX__, BONE, BONE), // 18
    //     SP(_X__X___, X___X___, BONE, BONE), // 19
    //     SP(X_______, ________, BONE, BONE), // 20
    //     SP(___XXX_X, XX______, BONE, BONE), // 21
    //     SP(________, ________, ____, ____), // 22

    // },

    {
        // FRAME_SKELETON4 = 25

        1,
        0,

        SP(________, ________, ____, ____), // 00
        SP(________, ________, ____, ____), // 01
        SP(____XXXX, ________, BONE, BONE), // 02
        SP(___XXXXX, X_______, BONE, BONE), // 03
        SP(__X__X__, X_______, BONE, BONE), // 04
        SP(__X__X__, X_______, BONE, BONE), // 05
        SP(__XXXXXX, X_______, BONE, BONE), // 06
        SP(___X_X_X, ________, BONE, BONE), // 05
        SP(____X_X_, ________, BONE, BONE), // 08
        SP(_______X, ________, BONE, BONE), // 09
        SP(___XXXXX, X_______, BONE, BONE), // 10
        SP(__X___X_, _X______, BONE, BONE), // 11
        SP(_XX_XXXX, _XX_____, BONE, BONE), // 12
        SP(_X____X_, __X_____, BONE, BONE), // 13
        SP(____XXXX, X_______, BONE, BONE), // 14
        SP(_XX__XXX, _XX_____, BONE, BONE), // 15
        SP(X___X_X_, X__X____, BONE, BONE), // 16
        SP(____X___, X_______, BONE, BONE), // 15
        SP(___XX___, XX______, BONE, BONE), // 18
        SP(____X___, X_______, BONE, BONE), // 19
        SP(________, ________, BONE, BONE), // 20
        SP(___XXX_X, XX______, BONE, BONE), // 21
        SP(________, ________, ____, ____), // 22

    },

    // {
    //     // FRAME_SKELETON5 = 26

    //     0,
    //     0,

    //     SP(________, ________, ____, ____), // 00
    //     SP(________, ________, ____, ____), // 01
    //     SP(________, ________, ____, ____), // 02
    //     SP(________, ________, ____, ____), // 03
    //     SP(________, ________, ____, ____), // 04
    //     SP(________, ________, ____, ____), // 05
    //     SP(________, ________, ____, ____), // 06
    //     SP(________, ________, ____, ____), // 05
    //     SP(________, ________, ____, ____), // 08
    //     SP(________, ________, ____, ____), // 09
    //     SP(________, ________, ____, ____), // 10
    //     SP(________, ________, ____, ____), // 11
    //     SP(________, ________, ____, ____), // 12
    //     SP(________, ________, ____, ____), // 13
    //     SP(________, ________, ____, ____), // 14
    //     SP(________, ________, ____, ____), // 15
    //     SP(________, ________, ____, ____), // 16
    //     SP(________, ________, ____, ____), // 15
    //     SP(________, ________, ____, ____), // 18
    //     SP(________, ________, ____, ____), // 19
    //     SP(_______X, XXXXX___, BONE, BONE), // 20
    //     SP(______X_, _X__XX__, BONE, BONE), // 21
    //     SP(______XX, _XX__X__, BONE, BONE), // 22

    // },

    {
        // FRAME_ARMSCROSSED = 25

        2 + 0,
        0,

        SP(____XXXX, ________, HAIR, HAIR), // 00
        SP(___XXXXX, ________, HAIR, HAIR), // 01
        SP(__XXXXXX, X_______, HAIR, HAIR), // 02
        SP(__XXXXXX, XX______, HAIR, HAIR), // 03
        SP(__XXXXXX, ________, SKIN, HAIR), // 04
        SP(__XX_X_X, ________, SKIN, HAIR), // 05
        SP(__XXXXXX, ________, SKIN, HAIR), // 06
        SP(__XXXXX_, ________, SKIN, SKIN), // 05
        SP(___XX_X_, ________, SKIN, SKIN), // 08
        SP(___XXXX_, ________, SKIN, SKIN), // 09
        SP(__XXXXXX, ________, TOP1, TOP1), // 10
        SP(_XXXXXXX, X_______, TOP2, TOP2), // 11
        SP(_XXX__XX, X_______, TOP1, TOP1), // 12
        SP(_XXXXXXX, X_______, TOP2, BELT), // 13
        SP(__XXXX_X, X_______, PANT, TOP1), // 14
        SP(______XX, ________, TOP2, TOP2), // 15
        SP(___XXXX_, ________, BELT, BELT), // 16
        SP(___XXXXX, ________, PANT, PANT), // 15
        SP(___XXXXX, ________, PANT, PANT), // 18
        SP(___XX_XX, ________, PANT, PANT), // 19
        SP(__XX__XX, ________, BOOT, BOOT), // 20
        SP(__XXX_XX, X_______, BOOT, BOOT), // 21
        SP(__XXX_XX, X_______, SOLE, SOLE), // 22

    },

    // #if ENABLE_SHAKE

    // {   // FRAME_SHAKE= 28

    //     -1+0, 0,

    //     ________,________, ____, ____,     //00
    //     _______X,XXX_____, HAIR, HAIR,     //01
    //     ______XX,XXX_____, HAIR, HAIR,     //02
    //     _____XXX,XXXX____, HAIR, HAIR,     //03
    //     _____XXX,XXXXXX__, HAIR, HAIR,     //04
    //     _____XXX,XXXX____, SKIN, SKIN,     //05
    //     _____XX_,X_X_____, SKIN, SKIN,     //06
    //     _____XXX,XXX_____, SKIN, SKIN,     //05
    //     _____XXX,XX______, SKIN, SKIN,     //08
    //     __XX__XX,_X__XX__, SKIN, SKIN,     //09
    //     ___X__XX,_X__X___, SKIN, SKIN,     //10
    //     ____XXXX,XXXXX___, TOP1, TOP1,     //11
    //     ___XXXXX,XXXXX___, TOP2, TOP2,     //12
    //     ____XXXX,XXXX____, TOP1, TOP1,     //13
    //     ______XX,XX______, TOP2, TOP2,     //14
    //     ______XX,XX______, TOP1, TOP1,     //15
    //     ______XX,XX______, TOP2, TOP2,     //16
    //     ______XX,XX______, BELT, BELT,     //15
    //     ______XX,XXX_____, PANT, PANT,     //18
    //     ______XX,_XX_____, PANT, BOOT,     //19
    //     _____XX_,_XX_____, BOOT, BOOT,     //20
    //     _____XXX,_XXX____, BOOT, SOLE,     //21
    //     _____XXX,________, SOLE, SOLE,     //22

    //     },

    // {   // FRAME_SHAKE = 29

    //     0+0, 0,

    //     ________,________, ____, ____,     //00
    //     _______X,XXX_____, HAIR, HAIR,     //01
    //     ______XX,XXX_____, HAIR, HAIR,     //02
    //     _____XXX,XXXX____, HAIR, HAIR,     //03
    //     _____XXX,XXXXXX__, HAIR, HAIR,     //04
    //     _____XXX,XXXX____, SKIN, SKIN,     //05
    //     _____XX_,X_X_____, SKIN, SKIN,     //06
    //     _____XXX,XXX_____, SKIN, SKIN,     //05
    //     _____XXX,XX______, SKIN, SKIN,     //08
    //     ______XX,_X___XX_, SKIN, SKIN,     //09
    //     ______XX,_X___X__, SKIN, SKIN,     //10
    //     ____XXXX,XXXXXX__, TOP1, TOP1,     //11
    //     ___XXXXX,XXXXXX__, TOP2, TOP2,     //12
    //     __XXX_XX,XXXXX___, TOP1, TOP1,     //13
    //     __XX__XX,XX______, TOP2, TOP2,     //14
    //     __XX__XX,XX______, TOP1, TOP1,     //15
    //     ______XX,XX______, TOP2, TOP2,     //16
    //     __XX__XX,XX______, BELT, BELT,     //15
    //     ______XX,XXX_____, PANT, PANT,     //18
    //     ______XX,_XX_____, PANT, BOOT,     //19
    //     _____XX_,_XX_____, BOOT, BOOT,     //20
    //     _____XXX,_XXX____, BOOT, BOOT,     //21
    //     _____XXX,________, SOLE, SOLE,     //22

    //     },

    // {   // FRAME_SHAKE= 30

    //     -1+0, 0,

    //     ________,________, ____, ____,     //00
    //     _______X,XXX_____, HAIR, HAIR,     //01
    //     ______XX,XXX_____, HAIR, HAIR,     //02
    //     _____XXX,XXXX____, HAIR, HAIR,     //03
    //     _____XXX,XXXXXX__, HAIR, HAIR,     //04
    //     _____XXX,XXXX____, SKIN, SKIN,     //05
    //     _____XX_,X_X_____, SKIN, SKIN,     //06
    //     _____XXX,XXX_____, SKIN, SKIN,     //05
    //     _____XXX,XX______, SKIN, SKIN,     //08
    //     _XX___XX,_X______, SKIN, SKIN,     //09
    //     __X___XX,_X______, SKIN, SKIN,     //10
    //     __XXXXXX,XXX_____, TOP1, TOP1,     //11
    //     __XXXXXX,XXXX____, TOP2, TOP2,     //12
    //     ___XXXXX,XXXXX___, TOP1, TOP1,     //13
    //     ______XX,XX_XX___, TOP2, TOP2,     //14
    //     ______XX,XX__XX__, TOP1, TOP1,     //15
    //     ______XX,XX______, TOP2, TOP2,     //16
    //     ______XX,XX__XX__, BELT, BELT,     //15
    //     ______XX,XXX_____, PANT, PANT,     //18
    //     ______XX,_XX_____, PANT, BOOT,     //19
    //     _____XX_,_XX_____, BOOT, BOOT,     //20
    //     _____XXX,_XXX____, BOOT, SOLE,     //21
    //     _____XXX,________, SOLE, SOLE,     //22

    //     },

    // #endif

    // {
    //     // FRAME_TALK= 31

    // 2 + 0,
    // 0,

    // SP(____XXXX, ________, HAIR, HAIR), // 00
    // SP(___XXXXX, ________, HAIR, HAIR), // 01
    // SP(__XXXXXX, X_______, HAIR, HAIR), // 02
    // SP(__XXXXXX, XX______, HAIR, HAIR), // 03
    // SP(__XXXXXX, ________, SKIN, HAIR), // 04
    // SP(__XX_X_X, ________, SKIN, HAIR), // 05
    // SP(__XXXXXX, ________, SKIN, HAIR), // 06
    // SP(__XXXXX_, ________, SKIN, SKIN), // 05
    // SP(___XX_X_, ________, SKIN, SKIN), // 08
    // SP(___XXXX_, ________, SKIN, SKIN), // 09
    // SP(__XXXXXX, ________, TOP1, TOP1), // 10
    // SP(_XXXXXXX, X_______, TOP2, TOP2), // 11
    // SP(_XXXXXXX, X_______, TOP1, TOP1), // 12
    // SP(XX_XXXX_, XX______, TOP2, TOP2), // 13
    // SP(XX_XXXX_, XX______, TOP1, TOP1), // 14
    // SP(___XXXX_, XX______, TOP2, SKIN), // 15
    // SP(XX_XXXX_, ________, BELT, BELT), // 16
    // SP(XX_XXXXX, ________, PANT, PANT), // 15
    // SP(___XX_XX, ________, PANT, PANT), // 18
    // SP(__XXX_XX, ________, PANT, PANT), // 19
    // SP(__XX__XX, ________, BOOT, BOOT), // 20
    // SP(__XXX_XX, X_______, BOOT, BOOT), // 21
    // SP(__XXX_XX, X_______, SOLE, SOLE), // 22

    // },

    // {   // FRAME_TALK= 31

    //     3+0, 0,

    //     ________,________, HAIR, ____ ),     //00
    //     ________,________, HAIR, ____ ),     //01
    //     ________,________, HAIR, HAIR ),     //02
    //     ________,________, HAIR, HAIR ),     //03
    //     ___XXXXX,________, SKIN, HAIR ),     //04
    //     __XXXXXX,X_______, HAIR, HAIR ),     //05
    //     __XXXXXX,________, HAIR, HAIR ),     //06
    //     __XXXXXX,________, HAIR, ____ ),     //05
    //     __XXXXX_,________, HAIR, ____ ),     //08
    //     ___XXXX_,________, HAIR, ____ ),     //09
    //     ___XXXXX,________, SKIN, TOP1 ),     //10
    //     __XXXXXX,X_______, TOP2, TOP2 ),     //11
    //     __XXXXXX,X_______, TOP1, TOP1 ),     //12
    //     _XXXXXXX,XX______, TOP2, TOP2 ),     //13
    //     XX_XXXX_,________, SKIN, SKIN ),     //14
    //     XX_XXXX_,________, SKIN, ____ ),     //15
    //     __XX_XXX,________, SKIN, BELT ),     //16
    //     __XX_XXX,________, SKIN, PANT ),     //15
    //     __X____X,________, SKIN, ____ ),     //18
    //     ___XX_XX,________, SKIN, ____ ),     //19
    //     ___XX_XX,________, SKIN, ____ ),     //20
    //     __XX__XX,X_______, BOOT, BOOT ),     //21
    //     __XXX_XX,X_______, SOLE, SOLE ),     //22

    //     },

    {
        // FRAME_WALKDOWN1

        2 + 0,
        0,

        SP(___XXXX_, ________, HAIR, HAIR), // 00
        SP(__XXXXXX, ________, HAIR, HAIR), // 01
        SP(__XXXXXX, X_______, HAIR, HAIR), // 02
        SP(__XXXXXX, ________, SKIN, HAIR), // 03
        SP(__XX_X_X, ________, SKIN, HAIR), // 04
        SP(__XXXXXX, ________, SKIN, HAIR), // 05
        SP(__XXXXXX, ________, SKIN, HAIR), // 06
        SP(___XXX__, ________, SKIN, SKIN), // 05
        SP(____XXX_, ________, SKIN, SKIN), // 08
        SP(__XXXXXX, X_______, TOP1, SKIN), // 09
        SP(_XXXXXXX, XX______, TOP2, TOP2), // 10
        SP(_XXXXXXX, XX______, TOP1, TOP1), // 11
        SP(XXXXXXXX, XXX_____, TOP2, TOP2), // 12
        SP(XX_XXXXX, _XX_____, TOP1, TOP2), // 13
        SP(___XXXXX, _XX_____, TOP2, TOP2), // 14
        SP(XX_XXXXX, ________, TOP2, SKIN), // 15
        SP(XX_XXXXX, ________, BELT, BELT), // 16
        SP(___XX_XX, ________, PANT, PANT), // 15
        SP(__XXX_XX, ________, PANT, PANT), // 18
        SP(__XXX_XX, ________, PANT, PANT), // 19
        SP(__XX__XX, ________, BOOT, BOOT), // 20
        SP(__XX__XX, ________, BOOT, BOOT), // 21
        SP(__XX__XX, ________, SOLE, SOLE), // 22

    },

    {
        // FRAME_WALKDOWN2

        2 + 0,
        0,

        SP(___XXXX_, ________, HAIR, HAIR), // 00
        SP(__XXXXXX, ________, HAIR, HAIR), // 01
        SP(__XXXXXX, X_______, HAIR, HAIR), // 02
        SP(__XXXXXX, ________, SKIN, HAIR), // 03
        SP(__XX_X_X, ________, SKIN, HAIR), // 04
        SP(__XXXXXX, ________, SKIN, HAIR), // 05
        SP(__XXXXXX, ________, SKIN, HAIR), // 06
        SP(___XXX__, ________, SKIN, SKIN), // 05
        SP(____XXX_, ________, SKIN, SKIN), // 08
        SP(__XXXXXX, ________, TOP1, SKIN), // 09
        SP(_XXXXXXX, X_______, TOP2, TOP2), // 10
        SP(XXXXXXXX, X_______, TOP1, TOP1), // 11
        SP(XXXXXXXX, XX______, TOP2, TOP2), // 12
        SP(___XXXXX, X_______, TOP1, TOP2), // 13
        SP(XX_XXXXX, X_______, TOP2, TOP2), // 14
        SP(XX_XXXX_, ________, TOP2, SKIN), // 15
        SP(___XXXXX, ________, BELT, BELT), // 16
        SP(___XX_XX, ________, PANT, PANT), // 15
        SP(___XX_XX, ________, PANT, PANT), // 18
        SP(__XXX_XX, ________, PANT, PANT), // 19
        SP(__XX____, ________, BOOT, BOOT), // 20
        SP(__XX____, ________, BOOT, BOOT), // 21
        SP(__XX____, ________, SOLE, SOLE), // 22

    },

    {
        // FRAME_WALKDOWN3

        2 + 0,
        0,

        SP(___XXXX_, ________, HAIR, HAIR), // 00
        SP(__XXXXXX, ________, HAIR, HAIR), // 01
        SP(__XXXXXX, X_______, HAIR, HAIR), // 02
        SP(__XXXXXX, ________, SKIN, HAIR), // 03
        SP(__XX_X_X, ________, SKIN, HAIR), // 04
        SP(__XXXXXX, ________, SKIN, HAIR), // 05
        SP(__XXXXXX, ________, SKIN, HAIR), // 06
        SP(___XXX__, ________, SKIN, SKIN), // 05
        SP(____XXX_, ________, SKIN, SKIN), // 08
        SP(__XXXXXX, ________, TOP1, SKIN), // 09
        SP(_XXXXXXX, X_______, TOP2, TOP2), // 10
        SP(_XXXXXXX, XX______, TOP1, TOP1), // 11
        SP(XXXXXXXX, XX______, TOP2, TOP2), // 12
        SP(_XXXXXX_, ________, TOP1, TOP2), // 13
        SP(_XXXXXX_, XX______, TOP2, TOP2), // 14
        SP(___XXXX_, XX______, TOP2, SKIN), // 15
        SP(__XXXXX_, ________, BELT, BELT), // 16
        SP(__XX_XX_, ________, PANT, PANT), // 15
        SP(__XX_XXX, ________, PANT, PANT), // 18
        SP(__XX__XX, ________, PANT, PANT), // 19
        SP(______XX, ________, BOOT, BOOT), // 20
        SP(______XX, ________, BOOT, BOOT), // 21
        SP(______XX, ________, SOLE, SOLE), // 22

    },

    {
        // FRAME_WALKUP1

        2 + 0,
        0,

        SP(____XXX_, ________, HAIR, HAIR), // 00
        SP(___XXXXX, ________, HAIR, HAIR), // 01
        SP(__XXXXXX, ________, HAIR, HAIR), // 02
        SP(__XXXXXX, ________, HAIR, HAIR), // 03
        SP(__XXXXXX, ________, HAIR, HAIR), // 04
        SP(__XXXXXX, ________, HAIR, HAIR), // 05
        SP(__XXXXXX, ________, HAIR, HAIR), // 06
        SP(__XXXXXX, ________, HAIR, SKIN), // 05
        SP(____XX__, ________, SKIN, SKIN), // 08
        SP(__XXXXXX, ________, TOP1, SKIN), // 09
        SP(_XXXXXXX, X_______, TOP2, TOP2), // 10
        SP(_XXXXXXX, X_______, TOP1, TOP2), // 11
        SP(XXXXXXXX, XX______, TOP2, TOP2), // 12
        SP(XX_XXXX_, XX______, TOP1, TOP2), // 13
        SP(___XXXX_, XX______, TOP2, TOP2), // 14
        SP(XX_XXXX_, ________, TOP1, SKIN), // 15
        SP(XX_XXXXX, ________, BELT, BELT), // 16
        SP(___XX_XX, ________, PANT, PANT), // 15
        SP(__XXX_XX, ________, PANT, PANT), // 18
        SP(__XXX_XX, ________, PANT, PANT), // 19
        SP(__XX__XX, ________, BOOT, BOOT), // 20
        SP(__XX__XX, ________, BOOT, BOOT), // 21
        SP(__XX__XX, ________, SOLE, SOLE), // 22

    },

    {
        // FRAME_WALKUP2

        2 + 0,
        0,

        SP(____XXX_, ________, HAIR, HAIR), // 00
        SP(___XXXXX, ________, HAIR, HAIR), // 01
        SP(__XXXXXX, ________, HAIR, HAIR), // 02
        SP(__XXXXXX, ________, HAIR, HAIR), // 03
        SP(__XXXXXX, ________, HAIR, HAIR), // 04
        SP(__XXXXXX, ________, HAIR, HAIR), // 05
        SP(__XXXXXX, ________, HAIR, HAIR), // 06
        SP(__XXXXX_, ________, HAIR, SKIN), // 05
        SP(____XX__, ________, SKIN, SKIN), // 08
        SP(__XXXXXX, ________, TOP1, SKIN), // 09
        SP(_XXXXXXX, X_______, TOP2, TOP2), // 10
        SP(XXXXXXXX, X_______, TOP1, TOP2), // 11
        SP(XXXXXXXX, XX______, TOP2, TOP2), // 12
        SP(___XXXXX, X_______, TOP1, TOP2), // 13
        SP(XX_XXXXX, X_______, TOP2, TOP2), // 14
        SP(XX_XXXX_, ________, TOP1, SKIN), // 15
        SP(___XXXXX, ________, BELT, BELT), // 16
        SP(___XX_XX, ________, PANT, PANT), // 15
        SP(___XX_XX, ________, PANT, PANT), // 18
        SP(__XXX_XX, ________, PANT, PANT), // 19
        SP(__XX____, ________, BOOT, BOOT), // 20
        SP(__XX____, ________, BOOT, BOOT), // 21
        SP(__XX____, ________, SOLE, SOLE), // 22

    },

    {
        // FRAME_WALKUP3

        2 + 0,
        0,

        SP(____XXX_, ________, HAIR, HAIR), // 00
        SP(___XXXXX, ________, HAIR, HAIR), // 01
        SP(__XXXXXX, ________, HAIR, HAIR), // 02
        SP(__XXXXXX, ________, HAIR, HAIR), // 03
        SP(__XXXXXX, ________, HAIR, HAIR), // 04
        SP(__XXXXXX, ________, HAIR, HAIR), // 05
        SP(__XXXXXX, ________, HAIR, HAIR), // 06
        SP(___XXXXX, ________, HAIR, SKIN), // 05
        SP(____XX__, ________, SKIN, SKIN), // 08
        SP(__XXXXXX, ________, TOP1, SKIN), // 09
        SP(_XXXXXXX, X_______, TOP2, TOP2), // 10
        SP(_XXXXXXX, XX______, TOP1, TOP2), // 11
        SP(XXXXXXXX, XX______, TOP2, TOP2), // 12
        SP(_XXXXXX_, ________, TOP1, TOP2), // 13
        SP(_XXXXXX_, XX______, TOP2, TOP2), // 14
        SP(___XXXX_, XX______, TOP1, SKIN), // 15
        SP(__XXXXX_, ________, BELT, BELT), // 16
        SP(__XX_XX_, ________, PANT, PANT), // 15
        SP(__XX_XXX, ________, PANT, PANT), // 18
        SP(__XX__XX, ________, PANT, PANT), // 19
        SP(______XX, ________, BOOT, BOOT), // 20
        SP(______XX, ________, BOOT, BOOT), // 21
        SP(______XX, ________, SOLE, SOLE), // 22

    },

};

// clang-format on

void adjustBoxPosition(int xOffset, int yOffset) {

	// first move PLAYER
	*boxLocation = pillFlagFrom == CH_BOX ? CH_BLANK : CH_PILL_1;
	bool lock = (pillFlagTo == CH_PILL1 || pillFlagTo == CH_PILL_1);

	// then box
	unsigned char *to = boxLocation + yOffset * _1ROW + xOffset;
	*to = lock ? CH_BOX_CORRECT : CH_BOX;

	if (lock) {

		startCharAnimation(TYPE_BOX_CORRECT, AnimateBase[TYPE_BOX_CORRECT]);
		ADDAUDIO(SFX_MAGIC2);

		pillCount--;
		ADDAUDIO(SFX_UNCOVER);

		if (!pillCount) {

			FLASH(0xD4, 10);
			if (Room < getRoomCount())
				Room++;
		}
	}

	int dir = ((yOffset != 0) << 1) | (xOffset > 0) | (yOffset > 0);

	pushUndo(manX, manY, dir, true);
	boxLocation = 0;

	manY += yOffset;
	manX += xOffset;
}

void startBoxMoving(int dir) {

	const struct {
		unsigned char a;
		unsigned char b;
		int x;
		int y;
	} boxMover[] = {
	    {CH_BOX_LTOR_RIGHT_1, CH_BOX_LTOR_LEFT_1, -1, 0}, // R
	    {CH_BOX_LTOR_LEFT_0, CH_BOX_LTOR_RIGHT_0, 1, 0},  // L
	    {CH_BOX_UTOD_BOTTOM_1, CH_BOX_UTOD_TOP_1, 0, -1}, // D
	    {CH_BOX_UTOD_TOP_0, CH_BOX_UTOD_BOTTOM_0, 0, 1},  // U
	};

	int y = boxMover[dir].y * _1ROW;
	int x = boxMover[dir].x;

	unsigned char *playerx = ADDRESS_OF(manY) + y + manX + x;
	unsigned char *from = playerx;
	unsigned char *to = from + y + x;

	if ((Attribute[CharToType[GET(*from)]] & ATT_PUSH) &&
	    (Attribute[CharToType[GET(*to)]] & ATT_BLANK)) {

		pushingMoves++;
		ADDAUDIO(SFX_DIRT);

		pillFlagTo = *to;

		boxLocation = from;
		pillFlagFrom = *from;

		if (Attribute[CharToType[pillFlagFrom]] & ATT_TARGETLIKE) {
			pillCount++;
			ADDAUDIO(SFX_BLIP);
		}

		// if (Attribute[CharToType[pillFlagTo]] & ATT_TARGETLIKE) {
		// 	pillCount--;
		// 	ADDAUDIO(SFX_UNCOVER);

		// 	if (!pillCount) {

		// 		FLASH(0xD4, 10);
		// 		if (Room < getRoomCount())
		// 			Room++;
		// 	}
		// }

		*from = boxMover[dir].a;
		startDefaultCharAnimation(CharToType[*from]);
		*to = boxMover[dir].b;
		startDefaultCharAnimation(CharToType[*to]);
	}

	else {
		startAnimation(animationList[ANIM_PLAYER], ID_Stand);
		playerAnim.animation--; // hack
	}
}

bool vectorJoystick(Animation *animate) {

	bool handled = false;
	if (animate == animationList[ANIM_PLAYER]) {

		frameAdjustX = 0;
		frameAdjustY = 0;

		swcha = SWCHA;
		inpt4 = INPT4;

		static const enum AnimationIdent WalkAnim[4] = {
		    ID_Walk,
		    ID_Walk,
		    ID_WalkUp,
		    ID_WalkDown,
		};

		static const enum AnimationIdent PushAnim[4] = {
		    ID_PushLeft,
		    ID_PushRight,
		    ID_PushUp,
		    ID_PushDown,
		};

		if (scoreCycle != SCORELINE_UNDO)
			for (int dir = 0; dir < 4; dir++) {
				int joybit = joyDirectBit[dir] << 4;

				if (pillCount && !(swcha & joybit)) {

					static const signed char faceDirection[] = {
					    FACE_LEFT,
					    FACE_RIGHT,
					    0,
					    0,
					};

					if (faceDirection[dir] && manFaceDirection != faceDirection[dir])
						manFaceDirection = faceDirection[dir];

					unsigned char *me = ADDRESS_OF(manY) + manX;
					int offset = dirOffset[dir];
					unsigned char *thisOffset = me + offset;
					unsigned char destType = CharToType[(*thisOffset) & 0x7F];

					if (Attribute[destType] & ATT_BLANK) {
						//					moves++;
						startAnimation(animate, WalkAnim[dir]);
						handled = true;
					}

					else if (Attribute[destType] & ATT_PUSH) {
						startAnimation(animate, PushAnim[dir]);
						handled = true;
					}

					break;
				}
			}

		if (!handled) {
			if (pillCount)
				startAnimation(animate, ID_Stand);
			else if (playerAnimationID != ID_Shades)
				startAnimation(animationList[ANIM_PLAYER], ID_Shades);
		}
	}

	return handled;
}

int absolute(int x) { return x < 0 ? -x : x; }

void processAnimation(Animation *animate) {

	while (!animate->count)
		switch (*animate->animation) {

		case ACTION_FLIP:
			manFaceDirection = -manFaceDirection;
			animate->animation++;
			break;

		case ACTION_NEXTSQUARE: {

			int x = *++animate->animation * (manFaceDirection < 0 ? -1 : 1);
			int y = *++animate->animation;

			int dir = 0;
			if (x > 0)
				dir = 1;
			if (y < 0)
				dir = 2;
			if (y > 0)
				dir = 3;

			pushUndo(manX, manY, dir, false);

			manX += x;
			manY += y;

			animate->animation++;
		} break;

		case ACTION_LOOP:

			animate->animation = animate->loop;
			break;

		case ACTION_STOP:

			vectorJoystick(animate);
			break;

		case ACTION_POSITION: {

			frameAdjustX = *++animate->animation;
			frameAdjustY = *++animate->animation;

			static int lastx = 0;
			static int lasty = 0;
			static int deltaDelay = 0;

			int x = (manX * PIXELS_PER_CHAR + 2 + ((manFaceDirection * frameAdjustX) >> 2));
			int y = ((manY * (CHAR_HEIGHT / 3) + 6 - ((frameAdjustY * (0X100 / 3)) >> 8)));

			// snail trail...

			int deltax = absolute(lastx - x);
			int deltay = absolute(lasty - y);

			if (!(deltax + deltay))
				deltaDelay = 0;

			if (deltaDelay > 4 && (deltax > 1 || deltay > 2)) {
				lastx = x;
				lasty = y;
				addLocalPixel(x, y, 3, 40);
			}

			deltaDelay++;

			animate->animation++;

		} break;

		case ACTION_MOVE: {

			int x = *++animate->animation;
			int y = *++animate->animation;
			adjustBoxPosition(x, y);
			animate->animation++;

		} break;

		case ACTION_PUSH:
			startBoxMoving(*++animate->animation);
			animate->animation++;
			break;

		case ACTION_JOYSTICK:
			getJoystick();

			animate->animation++;
			break;

		case ACTION_AUTOFRAME:
			animate->frameServer = animationList[*++animate->animation];
			animate->frameServer->count = 0;
			processAnimation(animate->frameServer);
			animate->animation++;
			break;

		case ACTION_DELAY:
			animate->count = *(animate->animation + 1);
			break;

		default:
			animate->frame = *animate->animation;
			animate->count = *(animate->animation + 1);
			break;
		}

	if (animate->frameServer)
		animate->frame = animate->frameServer->frame;

	//	actualScore = animationList[ANIM_PLAYER]->count;
}

// see ANIM_* definitions in player.h

Animation *animationList[] = {&playerAnim, &autoWalk, &autoWalkUp, &autoWalkDown, &autoPush};

void initAnimations() {

	startAnimation(animationList[ANIM_PLAYER], ID_Stand);
	startAnimation(animationList[ANIM_AUTO_WALK], AUTO_Walk);
	startAnimation(animationList[ANIM_AUTO_WALKUP], AUTO_WalkUp);
	startAnimation(animationList[ANIM_AUTO_WALKDOWN], AUTO_WalkDown);
	startAnimation(animationList[ANIM_AUTO_PUSH], AUTO_Push);
	//    startAnimation(animationList[ANIM_AUTO_PUSH], AUTOPush);
}

void updateAnimations() {

	for (unsigned int i = 0; i < sizeof(animationList) / sizeof(struct Animation *); i++) {
		if (animationList[i]->count != 255) {

			if (!animationList[i]->count) {
				animationList[i]->animation += 2;
				processAnimation(animationList[i]);
			}

			animationList[i]->count--;
		}
	}
}

void hackStartAnimation(Animation *anim, enum AnimationIdent id) { startAnimation(anim, id); }

void startAnimation(Animation *anim, enum AnimationIdent id) {

	anim->frameServer = 0;
	anim->animation = anim->loop = AnimationVector[id];
	anim->count = 0;
	anim->frame = FRAME_BLANK;

	processAnimation(anim);
}

// ROF