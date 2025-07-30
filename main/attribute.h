#ifndef __ATTRIBUTE_H
#define __ATTRIBUTE_H

#include "main.h"

extern const short Attribute[];
extern const unsigned char CharToType[];

enum ObjectType {

	// update Attribute[] in attribute.c if changing this
	// update AnimateBase[] in animations.C

	TYPE_SPACE,            // 00
	TYPE_BRICKWALL,        // 01
	TYPE_STEELWALL,        // 02
	TYPE_BOX,              // 03
	TYPE_MAN,              // 04
	TYPE_PILL1,            // 05
	TYPE_PILL2,            // 06
	TYPE_BOX_LTOR_LHS,     // 07
	TYPE_BOX_LTOR_RHS,     // 08
	TYPE_BOX_RTOL_LHS,     // 09
	TYPE_BOX_RTOL_RHS,     // 10
	TYPE_BOX_UTOD_TOP,     // 11
	TYPE_BOX_UTOD_BOT,     // 12
	TYPE_BOX_DTOU_TOP,     // 13
	TYPE_BOX_DTOU_BOT,     // 14
	TYPE_BOX_LTOR_LEFT,    // 15
	TYPE_BOX_LTOR_RIGHT,   // 16
	TYPE_BOX_RTOL_LEFT,    // 17
	TYPE_BOX_RTOL_RIGHT,   // 18
	TYPE_BOX_LOCKED,       // 19
	TYPE_BOX_CORRECT,      // 20
	TYPE_BOX_DEADLOCKED,   // 21
	TYPE_BOX_UNDO,         // 22
	TYPE_BOX_UNDO_CORRECT, // 23

	TYPE_MAX
};

enum ChName {

	// see charSet @ characterset.c
	// see charToType @ attribute.c

	CH_BLANK,             // 00
	CH_BRICKWALL,         // 01
	CH_STEELWALL,         // 02
	CH_PILL1,             // 03
	CH_PILL2,             // 04
	CH_BOX,               // 05
	CH_MAN,               // 06
	CH_BOX_UTOD_TOP_0,    // 07
	CH_BOX_UTOD_TOP_1,    // 08
	CH_BOX_UTOD_TOP_2,    // 09
	CH_BOX_UTOD_TOP_3,    // 10
	CH_BOX_UTOD_TOP_4,    // 11
	CH_BOX_UTOD_TOP_5,    // 12
	CH_BOX_UTOD_TOP_6,    // 13
	CH_BOX_UTOD_TOP_7,    // 14
	CH_BOX_UTOD_TOP_8,    // 15
	CH_BOX_UTOD_BOTTOM_0, // 16
	CH_BOX_UTOD_BOTTOM_1, // 17
	CH_BOX_UTOD_BOTTOM_2, // 18
	CH_BOX_UTOD_BOTTOM_3, // 19
	CH_BOX_UTOD_BOTTOM_4, // 20
	CH_BOX_UTOD_BOTTOM_5, // 21
	CH_BOX_UTOD_BOTTOM_6, // 22
	CH_BOX_UTOD_BOTTOM_7, // 23
	CH_BOX_UTOD_BOTTOM_8, // 24
	CH_BOX_LTOR_LEFT_0,   // 25
	CH_BOX_LTOR_LEFT_1,   // 26
	CH_BOX_LTOR_LEFT_2,   // 27
	CH_BOX_LTOR_LEFT_3,   // 28
	CH_BOX_LTOR_LEFT_4,   // 29
	CH_BOX_LTOR_RIGHT_0,  // 30
	CH_BOX_LTOR_RIGHT_1,  // 31
	CH_BOX_LTOR_RIGHT_2,  // 32
	CH_BOX_LTOR_RIGHT_3,  // 33
	CH_BOX_LTOR_RIGHT_4,  // 34
	CH_PILL_1,            // 35
	CH_PILL_2,            // 36
	CH_PILL_3,            // 37
	CH_PILL_4,            // 38
	CH_PILL_5,            // 39
	CH_PILL_6,            // 40
	CH_PILL_7,            // 41
	CH_BOX_LOCKED,        // 42
	CH_BOX_LOCKED2,       // 43
	CH_BOX_CORRECT,       // 44
	CH_BOX_DEADLOCK,      // 45
	CH_BOX_PADLOCK,       // 46
	CH_BOX_UNDO,          // 47
	CH_BOX_UNDO_CORRECT,  // 48
	CH_BOX_ZOOM1,         // 49
	CH_BOX_ZOOM2,         // 50
	CH_BOX_ZOOM3,         // 51
	CH_BOX_ZOOM4,         // 52
	CH_BOX_ZOOM5,         // 53

	CH_MAX
};

// clang-format off

enum ATTRIBUTE_BITS {

    // used in Attribute[] to flag object's action/reaction to situations

    ATT_BOX                     = 0b0000000000000001, // BOX
    ATT_BLANK                   = 0b0000000000000010, // SPC
    ATT_ACTIVE                  = 0b0000000000000100, // ACT
    ATT_DEADLOCK                = 0b0000000000001000, // DED
    ATT_EXIT                    = 0b0000000000010000, // XIT
    ATT_PUSH                    = 0b0000000000100000, // PSH
    ATT_PHASE1                  = 0b0000000001000000, // PH1
    ATT_PHASE2                  = 0b0000000010000000, // PH2
    ATT_PHASE4                  = 0b0000000100000000, // PH4
    ATT_TARGETLIKE              = 0b0000001000000000, // TRG
    ATT_MANYBLANK               = 0b0000010000000000, // BLK
};

// clang-format on

enum AttributeAlias {

	BOX = (ATT_BOX),        // box or box on target
	SPC = (ATT_BLANK),      // a literal blank area
	ACT = (ATT_ACTIVE),     // object has logic/requires processing
	DED = (ATT_DEADLOCK),   // contributes to a deadlock
	XIT = (ATT_EXIT),       //  this is an exit
	PSH = (ATT_PUSH),       // can be pushed
	BLK = (ATT_MANYBLANK),  //  a blank or man
	PH1 = (ATT_PHASE1),     // processed in phase 1
	PH2 = (ATT_PHASE2),     // processed in phase 2
	PH4 = (ATT_PHASE4),     // processed in phase 4
	TRG = (ATT_TARGETLIKE), // object contains a target

};

#endif