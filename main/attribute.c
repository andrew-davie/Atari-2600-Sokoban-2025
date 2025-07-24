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
};

const int Attribute[TYPE_MAX] = {

// 1. get character from board (ch)
// 2. convert to type via CharToType[ch]
// 3. Lookup Attribute[type] to get attribute bit mask for that type
// 4. If desired bit is set, then that character is involved in the action the bit represents

// see mnemonic definitions for bits in attribute.h
// There can be up to 32 attributes defined

#define _ 0

    // clang-format off

// Note: Sokoban - most of these attributes are actually unused/bogus - just a holdover
// from previous game(s).

//                                     e           k                               e
//                                    s           n                   e           k
//                                   i           a               e   l           i
//                                  o       k   l               l   b           l
//         g               e       n       c   B               b   a           -
//        n               k       k       o   h   e           a   d           t
//       i   e           i       c       l   s   v       k   e   o           e
//      l   s   t   p   l   h   o   t   d   a   i       n   m   l       1   g
//     l   a   r   i   n   s   R   i   a   u   t       a   r   p   x   l   r
//    a   h   i   r   a   u   o   x   e   q   c       l   e   x   o   o   a
//   F   P   D   D   M   P   N   E   D   S   A       B   P   E   B   R   T
//   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
//  FAL PHA DRT DRP RKF PSH QUI XIT DED SQB ACT ___ SPC PER XPD ___ ROL TRG

     _ | _ | _ | _ |RKF| _ |QUI| _ | _ | _ | _ | _ |SPC|PER|XPD| _ | _ | _ ,  // 00 TYPE_SPACE
     _ | _ | _ |DRP| _ | _ | _ | _ |DED| _ | _ | _ | _ | _ |XPD| _ |ROL| _ ,  // 01 TYPE_BRICKWALL
     _ | _ | _ |DRP| _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ ,  // 02 TYPE_STEELWALL
     _ |PH1| _ | _ | _ |PSH| _ | _ |DED| _ |ACT| _ | _ | _ |XPD|BOX|ROL| _ ,  // 03 TYPE_BOX
     _ |PH1| _ | _ |RKF| _ |QUI| _ | _ |SQB| _ | _ | _ | _ |XPD| _ | _ | _ ,  // 04 TYPE_MAN
     _ | _ |DRT|DRP| _ | _ | _ | _ | _ | _ | _ | _ |SPC|PER|XPD| _ | _ |TRG,  // 05 TYPE_PILL1
     _ | _ |DRT|DRP| _ | _ | _ | _ | _ | _ | _ | _ |SPC|PER|XPD| _ | _ |TRG,  // 06 TYPE_PILL2
     _ |PH1| _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ |XPD| _ | _ | _ ,  // 07 TYPE_BOX_LTOR_LHS
     _ |PH1| _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ |XPD| _ | _ | _ ,  // 08 TYPE_BOX_LTOR_RHS
     _ |PH1| _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ ,  // 09 TYPE_BOX_RTOL_LHS
     _ |PH1| _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ ,  // 10 TYPE_BOX_RTOL_RHS
     _ |PH1| _ | _ | _ |PSH| _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ ,  // 11 TYPE_BOX_UTOD_TOP
     _ |PH1| _ | _ | _ |PSH| _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ ,  // 12 TYPE_BOX_UTOD_BOT
     _ |PH1| _ | _ | _ |PSH| _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ ,  // 13 TYPE_BOX_DTOU_TOP
     _ |PH1| _ | _ | _ |PSH| _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ ,  // 14 TYPE_BOX_DTOU_BOT
     _ |PH1| _ | _ | _ |PSH| _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ ,  // 15 TYPE_BOX_LTOR_LEFT
     _ |PH1| _ | _ | _ |PSH| _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ ,  // 16 TYPE_BOX_LTOR_RIGHT
     _ |PH1| _ | _ | _ |PSH| _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ ,  // 17 TYPE_BOX_RTOL_LEFT
     _ |PH1| _ | _ | _ |PSH| _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ | _ ,  // 18 TYPE_BOX_RTOL_RIGHT
     _ |PH1| _ | _ | _ |PSH| _ | _ |DED| _ | _ | _ | _ | _ |XPD|BOX|ROL|TRG,  // 19 TYPE_BOX_LOCKED
     _ |PH1| _ | _ | _ |PSH| _ | _ |DED| _ |ACT| _ | _ | _ |XPD|BOX|ROL|TRG,  // 20 TYPE_BOX_CORRECT
     _ |PH1| _ | _ | _ |PSH| _ | _ |DED| _ |ACT| _ | _ | _ |XPD|BOX|ROL| _ ,  // 21 TYPE_BOX_DEADLOCKED
     _ |PH1| _ | _ | _ |PSH| _ | _ | _ | _ |ACT| _ | _ | _ |XPD|BOX|ROL| _ ,  // 22 TYPE_BOX_UNDO
     _ |PH1| _ | _ | _ |PSH| _ | _ | _ | _ |ACT| _ | _ | _ |XPD|BOX|ROL|TRG,  // 23 TYPE_BOX_UNDOCORRECT


//  FAL PHA DRT DRP RKF PSH QUI XIT DED SQB ACT  _  SPC PER XPD BOX ROL TRG
//   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |   |
//   F   P   D   D   M   P   N   E   D   S   A       B   P   E   B   R   T
//    a   h   i   r   a   u   o   x   e   q   c       l   e   x   o   o   a
//     l   a   r   i   n   s   R   i   a   u   t       a   r   p   x   l   r
//      l   s   t   p   l   h   o   t   d   a   i       n   m   l       1   g
//       i   e           i       c       l   s   v       k   e   o           e
//        n               k       k       o   h   e           a   a           t
//         g               e       n       c   B               b   b           -
//                                  o       k   l               l   l           l
//                                   i           a               e   e           i
//                                    s           n                               k
//                                     e           k                               e

};

// EOF
