#include "attribute.h"

const unsigned char CharToType[CH_MAX] = {

    // see ChName@attribute.h for corresponding character name/number
    // Attribute[] array (see below) must correspond to these entries

    TYPE_SPACE,            // 00 CH_BLANK
    TYPE_BRICKWALL,        // 01 CH_BRICKWALL
    TYPE_STEELWALL,        // 02 CH_STEELWALL
    TYPE_PILL1,            // 03 CH_PILL1
    TYPE_PILL1,            // 04 CH_PILL2
    TYPE_BOX,              // 05 CH_BOX
    TYPE_MAN,              // 06 CH_MAN
    TYPE_BOX_UTOD_TOP,     // 07 CH_BOX_UTOD_TOP_0
    TYPE_BOX_DTOU_TOP,     // 08 CH_BOX_UTOD_TOP_1
    TYPE_BOX_UTOD_TOP,     // 09 CH_BOX_UTOD_TOP_2
    TYPE_BOX_UTOD_TOP,     // 10 CH_BOX_UTOD_TOP_3
    TYPE_BOX_UTOD_TOP,     // 11 CH_BOX_UTOD_TOP_4
    TYPE_BOX_UTOD_TOP,     // 12 CH_BOX_UTOD_TOP_5
    TYPE_BOX_UTOD_TOP,     // 13 CH_BOX_UTOD_TOP_6
    TYPE_BOX_UTOD_TOP,     // 14 CH_BOX_UTOD_TOP_7
    TYPE_BOX_UTOD_TOP,     // 15 CH_BOX_UTOD_TOP_8
    TYPE_BOX_UTOD_BOT,     // 16 CH_BOX_UTOD_BOTTOM_0
    TYPE_BOX_DTOU_BOT,     // 17 CH_BOX_UTOD_BOTTOM_1
    TYPE_BOX_UTOD_BOT,     // 18 CH_BOX_UTOD_BOTTOM_2
    TYPE_BOX_UTOD_BOT,     // 19 CH_BOX_UTOD_BOTTOM_3
    TYPE_BOX_UTOD_BOT,     // 20 CH_BOX_UTOD_BOTTOM_4
    TYPE_BOX_UTOD_BOT,     // 21 CH_BOX_UTOD_BOTTOM_5
    TYPE_BOX_UTOD_BOT,     // 22 CH_BOX_UTOD_BOTTOM_6
    TYPE_BOX_UTOD_BOT,     // 23 CH_BOX_UTOD_BOTTOM_7
    TYPE_BOX_UTOD_BOT,     // 24 CH_BOX_UTOD_BOTTOM_8
    TYPE_BOX_LTOR_LHS,     // 25 CH_BOX_LTOR_LEFT_0
    TYPE_BOX_RTOL_LHS,     // 26 CH_BOX_LTOR_LEFT_1
    TYPE_BOX_LTOR_LHS,     // 27 CH_BOX_LTOR_LEFT_2
    TYPE_BOX_LTOR_LHS,     // 28 CH_BOX_LTOR_LEFT_3
    TYPE_BOX_LTOR_LHS,     // 29 CH_BOX_LTOR_LEFT_4
    TYPE_BOX_LTOR_RHS,     // 30 CH_BOX_LTOR_RIGHT_0
    TYPE_BOX_RTOL_RHS,     // 31 CH_BOX_LTOR_RIGHT_1
    TYPE_BOX_LTOR_RHS,     // 32 CH_BOX_LTOR_RIGHT_2
    TYPE_BOX_LTOR_RHS,     // 33 CH_BOX_LTOR_RIGHT_3
    TYPE_BOX_LTOR_RHS,     // 34 CH_BOX_LTOR_RIGHT_4
    TYPE_PILL1,            // 35 CH_PILL_1
    TYPE_PILL1,            // 36 CH_PILL_2
    TYPE_PILL1,            // 37 CH_PILL_3
    TYPE_PILL1,            // 38 CH_PILL_4
    TYPE_PILL1,            // 39 CH_PILL_5
    TYPE_PILL1,            // 40 CH_PILL_6
    TYPE_PILL1,            // 41 CH_PILL_7
    TYPE_BOX_LOCKED,       // 42 CH_BOX_LOCKED
    TYPE_BOX_LOCKED,       // 43 CH_BOX_LOCKED2
    TYPE_BOX_CORRECT,      // 44 CH_BOX_CORRECT
    TYPE_BOX_DEADLOCKED,   // 45 CH_BOX_DEADLOCKED
    TYPE_BOX_DEADLOCKED,   // 46 CH_BOX_PADLOCK
    TYPE_BOX_UNDO,         // 47 CH_BOX_UNDO
    TYPE_BOX_UNDO_CORRECT, // 48 CH_BOX_UNDO_CORRECT
    TYPE_BOX,              // 49 CH_BOX_ZOOM1
    TYPE_BOX,              // 50 CH_BOX_ZOOM2
    TYPE_BOX,              // 51 CH_BOX_ZOOM3
    TYPE_BOX,              // 52 CH_BOX_ZOOM4
    TYPE_BOX,              // 53 CH_BOX_ZOOM5

};

const short Attribute[TYPE_MAX] = {

// 1. get character from board (ch)
// 2. convert to type via CharToType[ch]
// 3. Lookup Attribute[type] to get attribute bit mask for that type
// 4. If desired bit is set, then that character is involved in the action the bit represents

// see mnemonic definitions for bits in attribute.h
// There can be up to 16 attributes defined (for short type; may be increased to 32/int)

#define _ 0

    // clang-format off

//                                                    s
//                                           n       k
//                                          a   e   c
//                                         M   l   o
//                                        /   b   l   e           t
//                                   e   k   a   d   v   k       e
//                                  s   n   h   a   i   n       g
//                                 a   a   s   a   t   a   x   r
//                                h   l   u   e   c   l   o   a
//                               P   B   P   D   A   B   B   T
//                               |   |   |   |   |   |   |   |
//                              PHA BLK PSH DED ACT SPC BOX TRG

     _ | _ | _ | _ | _ | _ | _ | _ |BLK| _ | _ | _ |SPC| _ | _ ,  // 00 TYPE_SPACE
     _ | _ | _ | _ | _ | _ | _ | _ | _ | _ |DED| _ | _ | _ | _ ,  // 01 TYPE_BRICKWALL
     _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ ,  // 02 TYPE_STEELWALL
     _ | _ | _ | _ | _ | _ | _ |PH1| _ |PSH|DED|ACT| _ |BOX| _ ,  // 03 TYPE_BOX
     _ | _ | _ | _ | _ | _ | _ |PH1|BLK| _ | _ | _ | _ | _ | _ ,  // 04 TYPE_MAN
     _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ |SPC| _ |TRG,  // 05 TYPE_PILL1
     _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ |SPC| _ |TRG,  // 06 TYPE_PILL2
     _ | _ | _ | _ | _ | _ | _ |PH1| _ | _ | _ | _ | _ | _ | _ ,  // 07 TYPE_BOX_LTOR_LHS
     _ | _ | _ | _ | _ | _ | _ |PH1| _ | _ | _ | _ | _ | _ | _ ,  // 08 TYPE_BOX_LTOR_RHS
     _ | _ | _ | _ | _ | _ | _ |PH1| _ | _ | _ | _ | _ | _ | _ ,  // 09 TYPE_BOX_RTOL_LHS
     _ | _ | _ | _ | _ | _ | _ |PH1| _ | _ | _ | _ | _ | _ | _ ,  // 10 TYPE_BOX_RTOL_RHS
     _ | _ | _ | _ | _ | _ | _ |PH1| _ |PSH| _ | _ | _ | _ | _ ,  // 11 TYPE_BOX_UTOD_TOP
     _ | _ | _ | _ | _ | _ | _ |PH1| _ |PSH| _ | _ | _ | _ | _ ,  // 12 TYPE_BOX_UTOD_BOT
     _ | _ | _ | _ | _ | _ | _ |PH1| _ |PSH| _ | _ | _ | _ | _ ,  // 13 TYPE_BOX_DTOU_TOP
     _ | _ | _ | _ | _ | _ | _ |PH1| _ |PSH| _ | _ | _ | _ | _ ,  // 14 TYPE_BOX_DTOU_BOT
     _ | _ | _ | _ | _ | _ | _ |PH1| _ |PSH| _ | _ | _ | _ | _ ,  // 15 TYPE_BOX_LTOR_LEFT
     _ | _ | _ | _ | _ | _ | _ |PH1| _ |PSH| _ | _ | _ | _ | _ ,  // 16 TYPE_BOX_LTOR_RIGHT
     _ | _ | _ | _ | _ | _ | _ |PH1| _ |PSH| _ | _ | _ | _ | _ ,  // 17 TYPE_BOX_RTOL_LEFT
     _ | _ | _ | _ | _ | _ | _ |PH1| _ |PSH| _ | _ | _ | _ | _ ,  // 18 TYPE_BOX_RTOL_RIGHT
     _ | _ | _ | _ | _ | _ | _ |PH1| _ |PSH|DED| _ | _ |BOX|TRG,  // 19 TYPE_BOX_LOCKED
     _ | _ | _ | _ | _ | _ | _ |PH1| _ |PSH|DED|ACT| _ |BOX|TRG,  // 20 TYPE_BOX_CORRECT
     _ | _ | _ | _ | _ | _ | _ |PH1| _ |PSH|DED|ACT| _ |BOX| _ ,  // 21 TYPE_BOX_DEADLOCKED
     _ | _ | _ | _ | _ | _ | _ |PH1| _ |PSH| _ |ACT| _ |BOX| _ ,  // 22 TYPE_BOX_UNDO
     _ | _ | _ | _ | _ | _ | _ |PH1| _ |PSH| _ |ACT| _ |BOX|TRG,  // 23 TYPE_BOX_UNDOCORRECT

};

// EOF
