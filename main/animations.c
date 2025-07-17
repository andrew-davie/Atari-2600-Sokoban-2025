
#include "animations.h"
#include "attribute.h"
#include "random.h"

// clang-format off

const unsigned char *Animate[TYPE_MAX];
char AnimCount[TYPE_MAX];


static const unsigned char Animate_BoxCorrect[] = {

    CH_BOX_CORRECT,3,
    CH_PILL1,3,
    CH_BOX, 6,
    CH_PILL1,3,
    CH_BOX, 6,
    CH_PILL1,3,
//    CH_BOX,
    ANIM_HALT,
};

static const unsigned char Animate_BoxLocked[] = {

    CH_BOX_LOCKED,60,
    CH_BOX_LOCKED2,12,

    CH_BOX_LOCKED,6,
    CH_BOX_LOCKED2,6,

    ANIM_LOOP,
};

static const unsigned char AnimPill[] = {

    CH_PILL_3, 120,
    CH_PILL_1, 6,
    CH_PILL_2, 6,
    CH_PILL_3, 6,
    ANIM_LOOP,

    CH_PILL_2, 99,
    CH_PILL_2, 12,
    CH_PILL_1, 12,
    CH_PILL_2, 9,
    CH_PILL_1, 9,
    CH_PILL_2, 6,
    CH_PILL_1, 6,
    CH_PILL_2, 3,
    CH_PILL_1, 3,
    ANIM_LOOP
};


#define BOX_SPEED 3
#define BOX_SPEED_VERTICAL 2


static const unsigned char AnimBox_UtoD_TOP[] = {

//    CH_BOX_UTOD_TOP_0, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_1, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_2, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_3, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_4, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_5, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_6, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_7, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_8, BOX_SPEED_VERTICAL,

    CH_BLANK, ANIM_HALT,
};

static const unsigned char AnimBox_UtoD_BOT[] = {

//    CH_BOX_UTOD_BOTTOM_0, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_1, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_2, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_3, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_4, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_5, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_6, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_7, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_8, BOX_SPEED_VERTICAL,

    CH_BOX, ANIM_HALT,
};


static const unsigned char AnimBox_DtoU_TOP[] = {


//    CH_BOX_UTOD_TOP_0, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_8, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_7, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_6, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_5, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_4, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_3, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_2, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_TOP_1, BOX_SPEED_VERTICAL,

    CH_BOX, ANIM_HALT,
};

static const unsigned char AnimBox_DtoU_BOT[] = {

//    CH_BOX_UTOD_BOTTOM_0, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_8, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_7, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_6, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_5, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_4, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_3, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_2, BOX_SPEED_VERTICAL,
    CH_BOX_UTOD_BOTTOM_1, BOX_SPEED_VERTICAL,

    CH_BLANK, ANIM_HALT,
};

static const unsigned char AnimBox_LtoR_LHS[] = {

 //   CH_BOX_LTOR_LEFT_0, BOX_SPEED,
    CH_BOX_LTOR_LEFT_1, BOX_SPEED,
    CH_BOX_LTOR_LEFT_2, BOX_SPEED,
    CH_BOX_LTOR_LEFT_3, BOX_SPEED,
    CH_BOX_LTOR_LEFT_4, BOX_SPEED,

    CH_BLANK,
    ANIM_HALT,
};

static const unsigned char AnimBox_LtoR_RHS[] = {

    //    CH_BOX_LTOR_RIGHT_0, BOX_SPEED,
    CH_BOX_LTOR_RIGHT_1, BOX_SPEED,
    CH_BOX_LTOR_RIGHT_2, BOX_SPEED,
    CH_BOX_LTOR_RIGHT_3, BOX_SPEED,
    CH_BOX_LTOR_RIGHT_4, BOX_SPEED,

    CH_BOX,
    ANIM_HALT,
};

static const unsigned char AnimBox_RtoL_LHS[] = {

    CH_BOX_LTOR_LEFT_4, BOX_SPEED,
    CH_BOX_LTOR_LEFT_3, BOX_SPEED,
    CH_BOX_LTOR_LEFT_2, BOX_SPEED,
    CH_BOX_LTOR_LEFT_1, BOX_SPEED,
//    CH_BOX_LTOR_LEFT_0, BOX_SPEED,

    CH_BOX,
    ANIM_HALT,
};

static const unsigned char AnimBox_RtoL_RHS[] = {

    // sliding R to L
    CH_BOX_LTOR_RIGHT_4, BOX_SPEED,
    CH_BOX_LTOR_RIGHT_3, BOX_SPEED,
    CH_BOX_LTOR_RIGHT_2, BOX_SPEED,
    CH_BOX_LTOR_RIGHT_1, BOX_SPEED,
//    CH_BOX_LTOR_RIGHT_0, BOX_SPEED,

    CH_BLANK,
    ANIM_HALT,
};

const unsigned char AnimMan[] = {

    CH_MAN, ANIM_HALT,


    // @startup
    ANIM_LOOP,
};

// clang-format on

const unsigned char *const AnimateBase[] = {

    // indexed by object TYPE (def: ObjectType in attribute.h)
    // =0 if object does not auto-animate
    // NOTE: ALL OBJECTS OF THIS TYPE WILL HAVE THE SAME SHAPE ONSCREEN!

    0,                  // 00 TYPE_SPACE
    0,                  // 01 TYPE_BRICKWALL
    0,                  // 02 TYPE_STEELWALL
    0,                  // 03 TYPE_BOULDER
    0,                  // 04 TYPE_MAN_PRE
    AnimMan,            // 05 TYPE_MAN
    AnimPill,           // 06 TYPE_PILL1
    AnimPill,           // 07 TYPE_PILL2
    0,                  // 08 TYPE_BOULDER_SHAKE
    0,                  // 09 TYPE_BOULDER_GENERATOR
    0,                  // 10 TYPE_BOULDER_FALLING
    AnimBox_LtoR_LHS,   // 11 TYPE_BOX_LTOR_LHS
    AnimBox_LtoR_RHS,   // 12 TYPE_BOX_LTOR_RHS
    AnimBox_RtoL_LHS,   // 13 TYPE_BOX_RTOL_LHS
    AnimBox_RtoL_RHS,   // 14 TYPE_BOX_RTOL_RHS
    0,                  // 15 TYPE_ABOUTTOFALL
    AnimBox_UtoD_TOP,   // 16 TYPE_BOX_UTOD_TOP
    AnimBox_UtoD_BOT,   // 17 TYPE_BOX_UTOD_BOT
    AnimBox_DtoU_TOP,   // 18 TYPE_BOX_DTOU_TOP
    AnimBox_DtoU_BOT,   // 19 TYPE_BOX_DTOU_BOT
    AnimBox_LtoR_LHS,   // 20 TYPE_BOX_LTOR_LEFT
    AnimBox_LtoR_RHS,   // 21 TYPE_BOX_LTOR_RIGHT
    AnimBox_RtoL_LHS,   // 22 TYPE_BOX_RTOL_LEFT
    AnimBox_RtoL_RHS,   // 23 TYPE_BOX_RTOL_RIGHT
    Animate_BoxLocked,  // 24 TYPE_BOX_LOCKED
    Animate_BoxCorrect, // 25 TYPE_BOX_CORRECT
};

void initCharAnimations() {

	for (int type = 0; type < TYPE_MAX; type++)
		startCharAnimation(type, AnimateBase[type]);
}

void startCharAnimation(int type, const unsigned char *idx) {

	int count = 0;
	if (idx) {

		if (*idx == ANIM_LOOP)
			idx = AnimateBase[type];

		Animate[type] = idx++;

		count = *idx;
		if (count == ANIM_RNDSPEED)
			count = (getRandom32() & 0xFF) | 15;
	}
	AnimCount[type] = count;
}

void startDefaultCharAnimation(int type) { startCharAnimation(type, AnimateBase[type]); }

void processCharAnimations() {

	for (int type = 0; type < TYPE_MAX; type++)
		if (AnimCount[type] && AnimateBase[type])
			if (!--AnimCount[type])
				startCharAnimation(type, Animate[type] + 2);
}

// EOF