// clang-format off

#include "defines.h"
#include "defines_cdfj.h"


#include "characterset.h"

#include "attribute.h"
#include "drawscreen.h"
#include "main.h"
#include "scroll.h"
#include "player.h"
#include "random.h"

// clang-format on

static const unsigned char char_parallaxBlank[CHAR_HEIGHT];

// clang-format off

static const unsigned char char_box_deadlocked[CHAR_HEIGHT] = {

    0b00011111,     /*0*/\
    0b00011111, \
    0b00011111, \
    0b00010001,     /*1*/\
    0b00000000, \
    0b00011111, \
    0b00010000,     /*2*/\
    0b00000000, \
    0b00011111, \
    0b00010001,     /*3*/\
    0b00000000, \
    0b00011111, \
    0b00010000,     /*4*/\
    0b00000000, \
    0b00011111, \
    0b00010001,     /*5*/\
    0b00000000, \
    0b00011111, \
    0b00010000,     /*6*/\
    0b00000000, \
    0b00011111, \
    0b00010001,     /*7*/\
    0b00000000, \
    0b00011111, \
    0b00011111,     /*8*/\
    0b00000000, \
    0b00011111

};

static const unsigned char smallchar_box_deadlocked[CHAR_HEIGHT_HALF] = {

    0b11111111,
    0b11111111,
    0b11111111,
    0b00000011,
    0b00000000,
    0b11111100,
    0b00100111,
    0b00000000,
    0b11111100,
    0b00000000,
    0b00000000,
    0b11111100,
    0b00100100,
    0b00000000,
    0b11111100,
    0b00000000,
    0b00000000,
    0b11111100,

};

static const unsigned char smallchar_box_deadlocked2[CHAR_HEIGHT_HALF] = {

    0b00000000,
    0b00000000,
    0b00000000,
    0b11111111,
    0b11111111,
    0b11111111,
    0b00000011,
    0b00000000,
    0b11111100,
    0b00000100,
    0b00000000,
    0b11111100,
    0b00000000,
    0b00000000,
    0b11111100,
    0b00000000,
    0b00000000,
    0b00000000,

};
static const unsigned char smallchar_box_deadlocked3[CHAR_HEIGHT_HALF] = {

    0b00000000,
    0b00000000,
    0b00000000,
    0b11011011,
    0b11011011,
    0b11011011,
    0b00000011,
    0b00000000,
    0b11011000,
    0b00000000,
    0b00000000,
    0b11011000,
    0b00000000,
    0b00000000,
    0b11011000,
    0b00000000,
    0b00000000,
    0b00000000,

};

static const unsigned char smallchar_box_deadlocked4[CHAR_HEIGHT_HALF] = {

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b01001011,
    0b01001011,
    0b01001011,
    0b00000000,
    0b00000000,
    0b01001000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,

};

static const unsigned char char_box_zoom1[CHAR_HEIGHT] = {

    0b00000000,
    0b00000000,
    0b00000000,

    0b00011111,     /*0*/\
    0b00011111, \
    0b00011111, \
    0b00010000,     /*1*/\
    0b00000001, \
    0b00011111, \
    0b00010001,     /*2*/\
    0b00000000, \
    0b00011111, \
    0b00010000,     /*3*/\
    0b00000001, \
    0b00011111, \
    0b00010001,     /*6*/\
    0b00000000, \
    0b00011111, \
    0b00010000,     /*7*/\
    0b00000001, \
    0b00011111, \
    0b00011111,     /*8*/\
    0b00000000, \
    0b00011111,

    0b00000000,
    0b00000000,
    0b00000000,

};


static const unsigned char char_box_zoom2[CHAR_HEIGHT] = {

    0b00000000,
    0b00000000,
    0b00000000,

    0b00001110,     /*0*/\
    0b00001110, \
    0b00001110, \
    0b00001010,     /*1*/\
    0b00000010, \
    0b00001100, \
    0b00001010,     /*2*/\
    0b00000000, \
    0b00001110, \
    0b00001010,     /*3*/\
    0b00000010, \
    0b00001100, \
    0b00001010,     /*6*/\
    0b00000000, \
    0b00001110, \
    0b00001010,     /*7*/\
    0b00000010, \
    0b00001100, \
    0b00001110,     /*8*/\
    0b00000000, \
    0b00001110,

    0b00000000,
    0b00000000,
    0b00000000,

};

static const unsigned char char_box_zoom3[CHAR_HEIGHT] = {

    0b00000000,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,

    0b00001110,     /*0*/\
    0b00001110, \
    0b00001110, \
    0b00001000,     /*1*/\
    0b00000010, \
    0b00001110, \
    0b00001010,     /*6*/\
    0b00000000, \
    0b00001110, \
    0b00001000,     /*7*/\
    0b00000010, \
    0b00001110, \
    0b00001110,     /*8*/\
    0b00000000, \
    0b00001110,

    0b00000000,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,

};

static const unsigned char char_box_zoom4[CHAR_HEIGHT] = {

    0b00000000,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,


    0b00000100,     /*0*/\
    0b00000100, \
    0b00000100, \
    0b00000100,     /*7*/\
    0b00000000, \
    0b00000100, \
    0b00000100,     /*8*/\
    0b00000000, \
    0b00000100,

    0b00000000,
    0b00000000,
    0b00000000,


    0b00000000,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,

};


static const unsigned char char_box_zoom5[CHAR_HEIGHT] = {

    0b00000000,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,

    0b00000100,     /*0*/\
    0b00000100, \
    0b00000100, \

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,


    0b00000000,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,

};



static const unsigned char char_box_locked[CHAR_HEIGHT] = {

    0b00011111,     /*0*/
    0b00011111,
    0b00011111,
    0b00011111,     /*1*/
    0b00000000,
    0b00010000,
    0b00011011,     /*2*/
    0b00000100,
    0b00010101,
    0b00011011,     /*3*/
    0b00000100,
    0b00010100,
    0b00010001,     /*4*/
    0b00001110,
    0b00011111,
    0b00011011,     /*5*/
    0b00000100,
    0b00010100,
    0b00011011,     /*6*/
    0b00000100,
    0b00110101,
    0b00011111,     /*7*/
    0b00000000,
    0b00010000,
    0b00011111,     /*8*/
    0b00000000,
    0b00011111
};

// static const unsigned char char_box_locked2b[CHAR_HEIGHT] = {

//     0b00000000,     /*0*/
//     0b00000000,
//     0b00011111,
//     0b00000000,     /*1*/
//     0b00000000,
//     0b00011111,
//     0b00000000,     /*2*/
//     0b00000000,
//     0b00011111,
//     0b00000000,     /*3*/
//     0b00000000,
//     0b00011111,
//     0b00000000,     /*4*/
//     0b00000000,
//     0b00011111,
//     0b00000000,     /*5*/
//     0b00000000,
//     0b00011111,
//     0b00000000,     /*6*/
//     0b00000000,
//     0b00011111,
//     0b00000000,     /*7*/
//     0b00000000,
//     0b00011111,
//     0b00011111,     /*8*/
//     0b00000000,
//     0b00011111
// };




static const unsigned char smallchar_box[CHAR_HEIGHT_HALF] = {

    0b11111111,
    0b11111110,
    0b11111110,
    0b11111111,
    0b00000000,
    0b00000000,
    0b11111111,
    0b00000000,
    0b00100110,
    0b11111100,
    0b00000000,
    0b00000000,
    0b11111100,
    0b00000000,
    0b00100100,
    0b11111100,
    0b00000000,
    0b00000000,

};

static const unsigned char smallchar_box_UtoD_bottom_5[CHAR_HEIGHT_HALF] = {

    0b11111100,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,


};
static const unsigned char smallchar_box_UtoD_bottom_4[CHAR_HEIGHT_HALF] = {

    0b11111111,
    0b00000000,
    0b00100100,
    0b11111100,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,


};
static const unsigned char smallchar_box_UtoD_bottom_3[CHAR_HEIGHT_HALF] = {

    0b11111111,
    0b00000000,
    0b00000000,
    0b11111111,
    0b00000000,
    0b00100100,
    0b11111100,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,


};

static const unsigned char smallchar_box_UtoD_bottom_2[CHAR_HEIGHT_HALF] = {

    0b11111111,
    0b00000000,
    0b00100100,
    0b11111111,
    0b00000000,
    0b00000000,
    0b11111100,
    0b00000000,
    0b00100100,
    0b11111100,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,


};

static const unsigned char smallchar_box_UtoD_bottom_1[CHAR_HEIGHT_HALF] = {

    0b11111111,
    0b00000000,
    0b00000000,
    0b11111111,
    0b00000000,
    0b00100100,
    0b11111111,
    0b00000000,
    0b00000000,
    0b11111100,
    0b00000000,
    0b00100100,
    0b11111100,
    0b00000000,
    0b00000000,

    0b00000000,
    0b00000000,
    0b00000000,


};




static const unsigned char smallchar_box_UtoD_top_5[CHAR_HEIGHT_HALF] = {

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,

    0b11111100,
    0b11111100,
    0b11111100,
};

static const unsigned char smallchar_box_UtoD_top_4[CHAR_HEIGHT_HALF] = {

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000000,
    0b00000011,
    0b00000011,
    0b00000011,
    0b00000000,
    0b00000000,
    0b00000000,

    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b00000000,
    0b00000000,

};


static const unsigned char smallchar_box_UtoD_top_3[CHAR_HEIGHT_HALF] = {

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000011,
    0b00000011,
    0b00000011,
    0b00000011,
    0b00000000,
    0b00000000,

    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b00000000,
    0b00000000,
    0b11111100,
    0b00000000,
    0b00100100,

};

static const unsigned char smallchar_box_UtoD_top_1[CHAR_HEIGHT_HALF] = {

    0b00000000,
    0b00000000,
    0b00000000,

    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b00000000,
    0b00000000,
    0b11111100,
    0b00000000,
    0b00100100,
    0b11111100,
    0b00000000,
    0b00000000,
    0b11111100,
    0b00000000,
    0b00100100,

};


static const unsigned char smallchar_box_UtoD_top_2[CHAR_HEIGHT_HALF] = {

    0b00000000,
    0b00000000,
    0b00000000,
    0b00000011,
    0b00000011,
    0b00000011,

    0b11111111,
    0b11111100,
    0b11111100,
    0b11111100,
    0b00000000,
    0b00000000,
    0b11111100,
    0b00000000,
    0b00100100,
    0b11111100,
    0b00000000,
    0b00000000,

};

static const unsigned char smallchar_box_LtoR_left_1[CHAR_HEIGHT_HALF] = {

    0b01101111,
    0b01101111,
    0b01101111,
    0b01101111,
    0b00000000,
    0b00000000,
    0b01101111,
    0b00000000,
    0b00000000,
    0b01101100,
    0b00000000,
    0b00000000,
    0b01101100,
    0b00000000,
    0b00000000,
    0b01101100,
    0b00000000,
    0b00000000,

};

static const unsigned char smallchar_box_LtoR_right_1[CHAR_HEIGHT_HALF] = {

    0b10010011,
    0b10010011,
    0b10010011,
    0b10010011,
    0b00000000,
    0b00000000,
    0b10010011,
    0b00000000,
    0b10010000,
    0b10010000,
    0b00000000,
    0b00000000,
    0b10010000,
    0b00000000,
    0b10010000,
    0b10010000,
    0b00000000,
    0b00000000,

};

static const unsigned char smallchar_box_LtoR_left_2[CHAR_HEIGHT_HALF] = {

    0b00100111,
    0b00100111,
    0b00100111,
    0b00100111,
    0b00000000,
    0b00000000,
    0b00100111,
    0b00000000,
    0b00000000,
    0b00100100,
    0b00000000,
    0b00000000,
    0b00100100,
    0b00000000,
    0b00000000,
    0b00100100,
    0b00000000,
    0b00000000,

};


static const unsigned char smallchar_box_LtoR_right_2[CHAR_HEIGHT_HALF] = {

    0b11011011,
    0b11011011,
    0b11011011,
    0b11011011,
    0b00000000,
    0b00000000,
    0b11011011,
    0b00000000,
    0b01001000,
    0b11011000,
    0b00000000,
    0b00000000,
    0b11011000,
    0b00000000,
    0b01001000,
    0b11011000,
    0b00000000,
    0b00000000,

};




static const unsigned char smallchar_box_correct[CHAR_HEIGHT_HALF] = {

    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111111,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,
    0b11111100,

};



static const unsigned char smallchar_box_locked[CHAR_HEIGHT_HALF] = {

    0b11111111,
    0b11111111,
    0b11111111,
    0b11111100,
    0b00000011,
    0b00000000,
    0b10110111,
    0b01001000,
    0b00100100,
    0b10110100,
    0b01001000,
    0b00000000,
    0b11111100,
    0b00000000,
    0b00100100,
    0b11111100,
    0b00000000,
    0b00000000,

};

static const unsigned char smallchar_box_locked2[CHAR_HEIGHT_HALF] = {

    0b11111111,
    0b11111111,
    0b11111111,
    0b11111100,
    0b00000011,
    0b00000011,
    0b10110111,
    0b01001000,
    0b01101100,
    0b10110100,
    0b01001000,
    0b01001000,
    0b11111100,
    0b00000000,
    0b00100100,
    0b11111100,
    0b00000000,
    0b00000000,
};


static const unsigned char char_box_correct[CHAR_HEIGHT] = {

    0b11111111,     /*0*/
    0b11111111,
    0b11111111,
    0b11111111,     /*1*/
    0b11111111,
    0b11111111,
    0b11111111,     /*2*/
    0b11111111,
    0b11111111,
    0b11111111,     /*3*/
    0b11111111,
    0b11111111,
    0b11111111,     /*4*/
    0b11111111,
    0b11111111,
    0b11111111,     /*5*/
    0b11111111,
    0b11111111,
    0b00011111,     /*6*/
    0b00011111,
    0b00011111,
    0b00011111,     /*7*/
    0b00011111,
    0b00011111,
    0b00011111,     /*8*/
    0b00011111,
    0b00011111,
};

static unsigned char char_box_locked2[CHAR_HEIGHT] = {

    0b11111,     /*0*/
    0b11111,
    0b11111,
    0b11111,     /*1*/
    0b00000,
    0b10000,
    0b11011,     /*2*/
    0b00100,
    0b10001,
    0b11011,     /*3*/
    0b00100,
    0b10000,
    0b10001,     /*4*/
    0b01110,
    0b10001,
    0b11011,     /*5*/
    0b00100,
    0b10000,
    0b11011,     /*6*/
    0b00100,
    0b10001,
    0b11111,     /*7*/
    0b00000,
    0b10000,
    0b11111,     /*8*/
    0b00000,
    0b11111
};


#define LINE0 0x00, 0x00, 0x00
#define BLANK3 LINE0
#define BLANK6 LINE0, LINE0
#define BLANK9 LINE0, LINE0, LINE0
#define BLANK12 LINE0, LINE0, LINE0, LINE0
#define BLANK15 LINE0, LINE0, LINE0, LINE0, LINE0
#define BLANK18 LINE0, LINE0, LINE0, LINE0, LINE0, LINE0
#define BLANK21 LINE0, LINE0, LINE0, LINE0, LINE0, LINE0, LINE0
#define BLANK24 LINE0, LINE0, LINE0, LINE0, LINE0, LINE0, LINE0, LINE0
#define BLANK27 LINE0, LINE0, LINE0, LINE0, LINE0, LINE0, LINE0, LINE0, LINE0

// The only required shape definition
// All 9 shifted versions of the shape are created by the macros!

#define BOX_BYTES                                                                                  \
    0b00011111,     /*0*/\
    0b00011111, \
    0b00011111, \
    0b00011111,     /*1*/\
    0b00000000, \
    0b00010001, \
    0b00011111,     /*2*/\
    0b00000000, \
    0b00010000, \
    0b00011111,     /*3*/\
    0b00000000, \
    0b00010001, \
    0b00011111,     /*4*/\
    0b00000000, \
    0b00010000, \
    0b00011111,     /*5*/\
    0b00000000, \
    0b00010001, \
    0b00011111,     /*6*/\
    0b00000000, \
    0b00010000, \
    0b00011111,     /*7*/\
    0b00000000, \
    0b00010001, \
    0b00011111,     /*8*/\
    0b00000000, \
    0b00011111

#if 0
    0b11111111,     /*0*/\
    0b00011111, \
    0b11111111, \
    0b11111111,     /*1*/\
    0b00000001, \
    0b00010001, \
    0b11111111,     /*2*/\
    0b00000001, \
    0b00110001, \
    0b11111111,     /*3*/\
    0b00000001, \
    0b00010001, \
    0b11111111,     /*4*/\
    0b00000001, \
    0b00110001, \
    0b11111111,     /*5*/\
    0b00000001, \
    0b00010001, \
    0b00011111,     /*6*/\
    0b00000001, \
    0b00010001, \
    0b00011111,     /*7*/\
    0b00000001, \
    0b00010001, \
    0b00011111,     /*8*/\
    0b00000000, \
    0b00011111
#endif

// Shift 0 (no blank lines on top)
#define CHAR_DEFINE_0(name, \
 b0,b1,b2, b3,b4,b5, b6,b7,b8, \
 b9,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26) \
static const unsigned char name##_top_0[27] = { \
     b0,b1,b2, b3,b4,b5, b6,b7,b8, \
 b9,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26 \
 }; \
static const unsigned char name##_bottom_0[27] = { BLANK27 };

// Shift 1 line (3 bytes blank on top)
#define CHAR_DEFINE_1(name, \
 b0,b1,b2, b3,b4,b5, b6,b7,b8, \
 b9,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26) \
static const unsigned char name##_top_1[27] = { \
  BLANK3, \
  b0,b1,b2, b3,b4,b5, b6,b7,b8, \
  b9,b10,b11, b12,b13,b14, b15,b16,b17, \
  b18,b19,b20, b21,b22,b23 \
}; \
static const unsigned char name##_bottom_1[27] = { \
  b24,b25,b26, \
  BLANK24 \
};

// Shift 2 lines (6 bytes blank on top)
#define CHAR_DEFINE_2(name, \
 b0,b1,b2, b3,b4,b5, b6,b7,b8, \
 b9,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26) \
static const unsigned char name##_top_2[27] = { \
  BLANK6, \
  b0,b1,b2, b3,b4,b5, b6,b7,b8, \
  b9,b10,b11, b12,b13,b14, \
  b15,b16,b17, b18,b19, b20 \
}; \
static const unsigned char name##_bottom_2[27] = { \
  b21,b22,b23, b24,b25,b26, \
  BLANK21 \
};

// Shift 3 lines (9 bytes blank on top)
#define CHAR_DEFINE_3(name, \
 b0,b1,b2, b3,b4,b5, b6,b7,b8, \
 b9,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26) \
static const unsigned char name##_top_3[27] = { \
  BLANK9, \
  b0,b1,b2, b3,b4,b5, b6,b7,b8, \
  b9,b10,b11, b12,b13,b14, b15,b16,b17, \
}; \
static const unsigned char name##_bottom_3[27] = { \
  b18,b19,b20, b21,b22,b23, b24,b25,b26, \
  BLANK18 \
};

// Shift 4 lines (12 bytes blank on top)
#define CHAR_DEFINE_4(name, \
 b0,b1,b2, b3,b4,b5, b6,b7,b8, \
 b9,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26) \
static const unsigned char name##_top_4[27] = { \
  BLANK12, \
  b0,b1,b2, b3,b4,b5, \
  b6,b7,b8, b9,b10,b11, b12,b13,b14, \
}; \
static const unsigned char name##_bottom_4[27] = { \
  b15,b16,b17, b18,b19,b20, b21,b22,b23, \
  b24,b25,b26, \
  BLANK15 \
};

// Shift 5 lines (15 bytes blank on top)
#define CHAR_DEFINE_5(name, \
 b0,b1,b2, b3,b4,b5, b6,b7,b8, \
 b9,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26) \
static const unsigned char name##_top_5[27] = { \
  BLANK15, \
  b0,b1,b2, \
  b3,b4,b5, b6,b7,b8, b9,b10,b11, \
}; \
static const unsigned char name##_bottom_5[27] = { \
  b12,b13,b14, b15,b16,b17, b18,b19,b20, \
  b21,b22,b23, b24,b25,b26, \
  BLANK12 \
};

// Shift 6 lines (18 bytes blank on top)
#define CHAR_DEFINE_6(name, \
 b0,b1,b2, b3,b4,b5, b6,b7,b8, \
 b9,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26) \
static const unsigned char name##_top_6[27] = { \
  BLANK18, \
  b0,b1,b2, \
  b3,b4,b5, b6,b7,b8 \
}; \
static const unsigned char name##_bottom_6[27] = { \
  b9,b10,b11, \
  b12,b13,b14, b15,b16,b17, b18,b19,b20, \
  b21,b22,b23, b24,b25,b26, \
  BLANK9 \
};

// Shift 7 lines (21 bytes blank on top)
#define CHAR_DEFINE_7(name, \
 b0,b1,b2, b3,b4,b5, b6,b7,b8, \
 b9,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26) \
static const unsigned char name##_top_7[27] = { \
  BLANK21, \
  b0,b1,b2, b3,b4,b5 \
}; \
static const unsigned char name##_bottom_7[27] = { \
  b6,b7,b8, b9,b10,b11, \
  b12,b13,b14, b15,b16,b17, b18,b19,b20, \
  b21,b22,b23, b24,b25,b26, \
  BLANK6 \
};

// Shift 8 lines (24 bytes blank on top)
#define CHAR_DEFINE_8(name, \
 b0,b1,b2, b3,b4,b5, b6,b7,b8, \
 b9,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26) \
static const unsigned char name##_top_8[27] = { \
  BLANK24, \
  b0,b1,b2 \
}; \
static const unsigned char name##_bottom_8[27] = { \
  b3,b4,b5, b6,b7,b8, b9,b10,b11, \
  b12,b13,b14, b15,b16,b17, b18,b19,b20, \
  b21,b22,b23, b24,b25,b26, \
  BLANK3 \
};

#define CONCAT(a, b) a##b
#define EXPAND(x) x
#define SELECT_CHAR_DEFINE(n) CONCAT(CHAR_DEFINE_, n)
#define SELECT_CHAR_CALL(n, name, bytes) EXPAND(SELECT_CHAR_DEFINE(n)(name, bytes))

SELECT_CHAR_CALL(0, char_box_UtoD, BOX_BYTES)
SELECT_CHAR_CALL(1, char_box_UtoD, BOX_BYTES)
SELECT_CHAR_CALL(2, char_box_UtoD, BOX_BYTES)
SELECT_CHAR_CALL(3, char_box_UtoD, BOX_BYTES)
SELECT_CHAR_CALL(4, char_box_UtoD, BOX_BYTES)
SELECT_CHAR_CALL(5, char_box_UtoD, BOX_BYTES)
SELECT_CHAR_CALL(6, char_box_UtoD, BOX_BYTES)
SELECT_CHAR_CALL(7, char_box_UtoD, BOX_BYTES)
SELECT_CHAR_CALL(8, char_box_UtoD, BOX_BYTES)


// Shift 0
#define HCHAR_DEFINE_0(name, \
 b0,b1,b2, b3,b4,b5, b6,b7,b8, \
 b9,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26) \
static const unsigned char name##_left_0[27] = { \
    b0,b1,b2, b3,b4,b5, b6,b7,b8, \
 b9,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26 \
 }; \
static const unsigned char name##_right_0[27] = { BLANK27 };

// Shift 1
#define HCHAR_DEFINE_1(name, \
 b00,b01,b02, b03,b04,b05, b06,b07,b08, \
 b09,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26) \
static const unsigned char name##_left_1[27] = { \
    b00 >> 1, b01 >> 1, b02 >> 1, \
    b03 >> 1, b04 >> 1, b05 >> 1, \
    b06 >> 1, b07 >> 1, b08 >> 1, \
    b09 >> 1, b10 >> 1, b11 >> 1, \
    b12 >> 1, b13 >> 1, b14 >> 1, \
    b15 >> 1, b16 >> 1, b17 >> 1, \
    b18 >> 1, b19 >> 1, b20 >> 1, \
    b21 >> 1, b22 >> 1, b23 >> 1, \
    b24 >> 1, b25 >> 1, b26 >> 1 \
 }; \
static const unsigned char name##_right_1[27] = { \
    (1 & b00) << 4, (1 & b01) << 4, (1 & b02) << 4, \
    (1 & b03) << 4, (1 & b04) << 4, (1 & b05) << 4, \
    (1 & b06) << 4, (1 & b07) << 4, (1 & b08) << 4, \
    (1 & b09) << 4, (1 & b10) << 4, (1 & b11) << 4, \
    (1 & b12) << 4, (1 & b13) << 4, (1 & b14) << 4, \
    (1 & b15) << 4, (1 & b16) << 4, (1 & b17) << 4, \
    (1 & b18) << 4, (1 & b19) << 4, (1 & b20) << 4, \
    (1 & b21) << 4, (1 & b22) << 4, (1 & b23) << 4, \
    (1 & b24) << 4, (1 & b25) << 4, (1 & b26) << 4 \
};

// Shift 2
#define HCHAR_DEFINE_2(name, \
 b00,b01,b02, b03,b04,b05, b06,b07,b08, \
 b09,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26) \
static const unsigned char name##_left_2[27] = { \
    b00 >> 2, b01 >> 2, b02 >> 2, \
    b03 >> 2, b04 >> 2, b05 >> 2, \
    b06 >> 2, b07 >> 2, b08 >> 2, \
    b09 >> 2, b10 >> 2, b11 >> 2, \
    b12 >> 2, b13 >> 2, b14 >> 2, \
    b15 >> 2, b16 >> 2, b17 >> 2, \
    b18 >> 2, b19 >> 2, b20 >> 2, \
    b21 >> 2, b22 >> 2, b23 >> 2, \
    b24 >> 2, b25 >> 2, b26 >> 2 \
 }; \
static const unsigned char name##_right_2[27] = { \
    (3 & b00) << 3, (3 & b01) << 3, (3 & b02) << 3, \
    (3 & b03) << 3, (3 & b04) << 3, (3 & b05) << 3, \
    (3 & b06) << 3, (3 & b07) << 3, (3 & b08) << 3, \
    (3 & b09) << 3, (3 & b10) << 3, (3 & b11) << 3, \
    (3 & b12) << 3, (3 & b13) << 3, (3 & b14) << 3, \
    (3 & b15) << 3, (3 & b16) << 3, (3 & b17) << 3, \
    (3 & b18) << 3, (3 & b19) << 3, (3 & b20) << 3, \
    (3 & b21) << 3, (3 & b22) << 3, (3 & b23) << 3, \
    (3 & b24) << 3, (3 & b25) << 3, (3 & b26) << 3 \
};

// Shift 3
#define HCHAR_DEFINE_3(name, \
 b00,b01,b02, b03,b04,b05, b06,b07,b08, \
 b09,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26) \
static const unsigned char name##_left_3[27] = { \
    b00 >> 3, b01 >> 3, b02 >> 3, \
    b03 >> 3, b04 >> 3, b05 >> 3, \
    b06 >> 3, b07 >> 3, b08 >> 3, \
    b09 >> 3, b10 >> 3, b11 >> 3, \
    b12 >> 3, b13 >> 3, b14 >> 3, \
    b15 >> 3, b16 >> 3, b17 >> 3, \
    b18 >> 3, b19 >> 3, b20 >> 3, \
    b21 >> 3, b22 >> 3, b23 >> 3, \
    b24 >> 3, b25 >> 3, b26 >> 3 \
 }; \
static const unsigned char name##_right_3[27] = { \
    (7 & b00) << 2, (7 & b01) << 2, (7 & b02) << 2, \
    (7 & b03) << 2, (7 & b04) << 2, (7 & b05) << 2, \
    (7 & b06) << 2, (7 & b07) << 2, (7 & b08) << 2, \
    (7 & b09) << 2, (7 & b10) << 2, (7 & b11) << 2, \
    (7 & b12) << 2, (7 & b13) << 2, (7 & b14) << 2, \
    (7 & b15) << 2, (7 & b16) << 2, (7 & b17) << 2, \
    (7 & b18) << 2, (7 & b19) << 2, (7 & b20) << 2, \
    (7 & b21) << 2, (7 & b22) << 2, (7 & b23) << 2, \
    (7 & b24) << 2, (7 & b25) << 2, (7 & b26) << 2 \
};

// Shift 4
#define HCHAR_DEFINE_4(name, \
 b00,b01,b02, b03,b04,b05, b06,b07,b08, \
 b09,b10,b11, b12,b13,b14, b15,b16,b17, \
 b18,b19,b20, b21,b22,b23, b24,b25,b26) \
static const unsigned char name##_left_4[27] = { \
    b00 >> 4, b01 >> 4, b02 >> 4, \
    b03 >> 4, b04 >> 4, b05 >> 4, \
    b06 >> 4, b07 >> 4, b08 >> 4, \
    b09 >> 4, b10 >> 4, b11 >> 4, \
    b12 >> 4, b13 >> 4, b14 >> 4, \
    b15 >> 4, b16 >> 4, b17 >> 4, \
    b18 >> 4, b19 >> 4, b20 >> 4, \
    b21 >> 4, b22 >> 4, b23 >> 4, \
    b24 >> 4, b25 >> 4, b26 >> 4 \
 }; \
static const unsigned char name##_right_4[27] = { \
    (15 & b00) << 1, (15 & b01) << 1, (15 & b02) << 1, \
    (15 & b03) << 1, (15 & b04) << 1, (15 & b05) << 1, \
    (15 & b06) << 1, (15 & b07) << 1, (15 & b08) << 1, \
    (15 & b09) << 1, (15 & b10) << 1, (15 & b11) << 1, \
    (15 & b12) << 1, (15 & b13) << 1, (15 & b14) << 1, \
    (15 & b15) << 1, (15 & b16) << 1, (15 & b17) << 1, \
    (15 & b18) << 1, (15 & b19) << 1, (15 & b20) << 1, \
    (15 & b21) << 1, (15 & b22) << 1, (15 & b23) << 1, \
    (15 & b24) << 1, (15 & b25) << 1, (15 & b26) << 1 \
};



#define SELECT_HCHAR_DEFINE(n) CONCAT(HCHAR_DEFINE_, n)
#define SELECT_HCHAR_CALL(n, name, bytes) EXPAND(SELECT_HCHAR_DEFINE(n)(name, bytes))

SELECT_HCHAR_CALL(0, char_box_LtoR, BOX_BYTES)
SELECT_HCHAR_CALL(1, char_box_LtoR, BOX_BYTES)
SELECT_HCHAR_CALL(2, char_box_LtoR, BOX_BYTES)
SELECT_HCHAR_CALL(3, char_box_LtoR, BOX_BYTES)
SELECT_HCHAR_CALL(4, char_box_LtoR, BOX_BYTES)


#define VM(n) \
 ((n) | ((n) <<1) | ((n) << 2) | ((n) << 3) | ((n) << 4) | 0xC0)

#define PILL(mask) \
static const unsigned char char_pill##mask[] = { \
 (0b00000000) & VM((mask & 4) >> 2),    \
 (0b00000000) & VM((mask & 2) >> 1),       \
 (0b00000000) & VM((mask & 1) >> 0),       \
 (0b00000000) & VM((mask & 4) >> 2),   \
 (0b00000000) & VM((mask & 2) >> 1),       \
 (0b00000000) & VM((mask & 1) >> 0),       \
 (0b01000100) & VM((mask & 4) >> 2),   \
 (0b01000100) & VM((mask & 2) >> 1),       \
 (0b01000100) & VM((mask & 1) >> 0),       \
 (0b01001110) & VM((mask & 4) >> 2),    \
 (0b01001110) & VM((mask & 2) >> 1),       \
 (0b01001110) & VM((mask & 1) >> 0),       \
 (0b00001110) & VM((mask & 4) >> 2),        \
 (0b00001110) & VM((mask & 2) >> 1),       \
 (0b00001110) & VM((mask & 1) >> 0),       \
 (0b00001110) & VM((mask & 4) >> 2),        \
 (0b00001110) & VM((mask & 2) >> 1),       \
 (0b00001110) & VM((mask & 1) >> 0),       \
 (0b00000100) & VM((mask & 4) >> 2),        \
 (0b00000100) & VM((mask & 2) >> 1),       \
 (0b00000100) & VM((mask & 1) >> 0),       \
 (0b00000000) & VM((mask & 4) >> 2),        \
 (0b00000000) & VM((mask & 2) >> 1),       \
 (0b00000000) & VM((mask & 1) >> 0),       \
 (0b00000000) & VM((mask & 4) >> 2),        \
 (0b00000000) & VM((mask & 2) >> 1),       \
 (0b00000000) & VM((mask & 1) >> 0), \
};

PILL(7);
PILL(6);
PILL(5);
PILL(4);
PILL(3);
//PILL(2);
PILL(1);


#define SMALLPILL(mask) \
static const unsigned char smallchar_pill##mask[] = { \
 (0b00000000) & VM((mask & 4) >> 2),    \
 (0b00000000) & VM((mask & 2) >> 1),       \
 (0b00000000) & VM((mask & 1) >> 0),       \
 (0b00000000) & VM((mask & 4) >> 2),   \
 (0b00000011) & VM((mask & 2) >> 1),       \
 (0b00000011) & VM((mask & 1) >> 0),       \
 \
 (0b00000000) & VM((mask & 4) >> 2),   \
 (0b01001000) & VM((mask & 2) >> 1),       \
 (0b01001000) & VM((mask & 1) >> 0),       \
 (0b00000000) & VM((mask & 4) >> 2),    \
 (0b01001000) & VM((mask & 2) >> 1),       \
 (0b01001000) & VM((mask & 1) >> 0),       \
 \
 (0b00000000) & VM((mask & 4) >> 2),        \
 (0b00000000) & VM((mask & 2) >> 1),       \
 (0b00000000) & VM((mask & 1) >> 0),       \
 (0b00000000) & VM((mask & 4) >> 2),        \
 (0b00000000) & VM((mask & 2) >> 1),       \
 (0b00000000) & VM((mask & 1) >> 0),       \
};

SMALLPILL(7);
// SMALLPILL(6);
// SMALLPILL(5);
// SMALLPILL(4);
// SMALLPILL(3);
// SMALLPILL(2);
// SMALLPILL(1);


static const unsigned char char_pill2[] = {
 0b00000,
 0b00000,
 0b00000,
 0b00000,
 0b00000,
 0b01110,
 0b00100,
 0b00100,
 0b11111,
 0b01110,
 0b01110,
 0b11111,
 0b01110,
 0b01110,
 0b11111,
 0b01110,
 0b01110,
 0b11111,
 0b00100,
 0b00100,
 0b11111,
 0b00000,
 0b00000,
 0b01110,
 0b00000,
 0b00000,
 0b00000,
};

// clang-format on

static unsigned char char_PlayerCell[CHAR_HEIGHT];

void copyPlayerCell(int c) {

	const unsigned char *const p = charSet[c].big;
	for (int i = 0; i < CHAR_HEIGHT; i++)
		char_PlayerCell[i] = p[i];
}

void modifyPlayerCell() {

	for (int i = 0; i < 4; i++) {
		unsigned int pix = getRandom32();
		int trix = (pix >> 29) * 3;
		unsigned char p = ~(1 << (pix & 7));

		char_PlayerCell[trix] &= p;
		char_PlayerCell[trix + 1] &= p;
		char_PlayerCell[trix + 2] &= p;
	}
}

// #define __CHAR_BLANK playerBigSprite
// clang-format off
const unsigned char __CHAR_BLANK[CHAR_HEIGHT] = {
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,
    0, 0,0,
    0, 0, 0,
    0, 0, 0,
    0, 0, 0,

};
// clang-format off

#define C(x) (const unsigned char *const)x

const CSET charSet[CH_MAX] = {

    // see ChName @ attribute.h

    // LARGE CHARACTER uses low 5 bits of each byte (5 horizontal pixels)
    // organised as 9 x RGB triplets (forming a trixel)

    // MEDIUM+SMALL CHARACTER holds a pair (left/right) of medium characters (D7D6D5 and D4D3D2)
    // and a similar pair (left/right) of lores (1 pixel wide) characters (D1 and D0) consisting
    // of 6 x RGB triplets (for medium) and 3 x RGB triplets (for lores). The pairs allow the
    // screen to be drawn in alternating (checkerboard) pattern to improve the visuals of side-by-side
    // characters. For example bricks/walls can have better definition.


    // LARGE CHARACTER        MEDIUM+SMALL CHARACTER PAIRS

    { C(char_parallaxBlank),  C(char_parallaxBlank),       }, // 00 CH_BLANK,             
    { C(__CHAR_BRICKWALL),    C(__SMALLCHAR_BRICKWALL),    }, // 01 CH_BRICKWALL,         
    { C(__CHAR_STEELWALL),    C(__SMALLCHAR_STEELWALL),    }, // 02 CH_STEELWALL,         
    { char_pill1,             smallchar_pill7,             }, // 03 CH_PILL1,             
    { char_pill1,             smallchar_pill7,             }, // 04 CH_PILL2,             
    { char_box_UtoD_top_0,    smallchar_box,               }, // 05 CH_BOX,               
    { char_PlayerCell,        char_PlayerCell,             }, // 06 CH_MAN,               
    { char_box_UtoD_top_0,    smallchar_box,               }, // 07 CH_BOX_UTOD_TOP_0,    
    { char_box_UtoD_top_1,    smallchar_box_UtoD_top_1,    }, // 08 CH_BOX_UTOD_TOP_1,    
    { char_box_UtoD_top_2,    smallchar_box_UtoD_top_1,    }, // 09 CH_BOX_UTOD_TOP_2,    
    { char_box_UtoD_top_3,    smallchar_box_UtoD_top_2,    }, // 10 CH_BOX_UTOD_TOP_3,    
    { char_box_UtoD_top_4,    smallchar_box_UtoD_top_3,    }, // 11 CH_BOX_UTOD_TOP_4,    
    { char_box_UtoD_top_5,    smallchar_box_UtoD_top_3,    }, // 12 CH_BOX_UTOD_TOP_5,    
    { char_box_UtoD_top_6,    smallchar_box_UtoD_top_4,    }, // 13 CH_BOX_UTOD_TOP_6,    
    { char_box_UtoD_top_7,    smallchar_box_UtoD_top_4,    }, // 14 CH_BOX_UTOD_TOP_7,    
    { char_box_UtoD_top_8,    smallchar_box_UtoD_top_5,    }, // 15 CH_BOX_UTOD_TOP_8,    
    { char_box_UtoD_bottom_0, char_parallaxBlank,          }, // 16 CH_BOX_UTOD_BOTTOM_0, 
    { char_box_UtoD_bottom_1, smallchar_box_UtoD_bottom_5, }, // 17 CH_BOX_UTOD_BOTTOM_1, 
    { char_box_UtoD_bottom_2, smallchar_box_UtoD_bottom_5, }, // 18 CH_BOX_UTOD_BOTTOM_2, 
    { char_box_UtoD_bottom_3, smallchar_box_UtoD_bottom_4, }, // 19 CH_BOX_UTOD_BOTTOM_3, 
    { char_box_UtoD_bottom_4, smallchar_box_UtoD_bottom_3, }, // 20 CH_BOX_UTOD_BOTTOM_4, 
    { char_box_UtoD_bottom_5, smallchar_box_UtoD_bottom_3, }, // 21 CH_BOX_UTOD_BOTTOM_5, 
    { char_box_UtoD_bottom_6, smallchar_box_UtoD_bottom_2, }, // 22 CH_BOX_UTOD_BOTTOM_6, 
    { char_box_UtoD_bottom_7, smallchar_box_UtoD_bottom_2, }, // 23 CH_BOX_UTOD_BOTTOM_7, 
    { char_box_UtoD_bottom_8, smallchar_box_UtoD_bottom_1, }, // 24 CH_BOX_UTOD_BOTTOM_8, 
    { char_box_LtoR_left_0,   smallchar_box,               }, // 25 CH_BOX_LTOR_LEFT_0,   
    { char_box_LtoR_left_1,   smallchar_box_LtoR_left_1,   }, // 26 CH_BOX_LTOR_LEFT_1,   
    { char_box_LtoR_left_2,   smallchar_box_LtoR_left_1,   }, // 27 CH_BOX_LTOR_LEFT_2,   
    { char_box_LtoR_left_3,   smallchar_box_LtoR_left_2,   }, // 28 CH_BOX_LTOR_LEFT_3,   
    { char_box_LtoR_left_4,   smallchar_box_LtoR_left_2,   }, // 29 CH_BOX_LTOR_LEFT_4,   
    { char_box_LtoR_right_0,  char_parallaxBlank,          }, // 30 CH_BOX_LTOR_RIGHT_0,  
    { char_box_LtoR_right_1,  smallchar_box_LtoR_right_1,  }, // 31 CH_BOX_LTOR_RIGHT_1,  
    { char_box_LtoR_right_2,  smallchar_box_LtoR_right_1,  }, // 32 CH_BOX_LTOR_RIGHT_2,  
    { char_box_LtoR_right_3,  smallchar_box_LtoR_right_2,  }, // 33 CH_BOX_LTOR_RIGHT_3,  
    { char_box_LtoR_right_4,  smallchar_box_LtoR_right_2,  }, // 34 CH_BOX_LTOR_RIGHT_4,  
    { char_pill1,             smallchar_pill7,             }, // 35 CH_PILL_1,            
    { char_pill2,             smallchar_pill7,             }, // 36 CH_PILL_2,            
    { char_pill3,             smallchar_pill7,             }, // 37 CH_PILL_3,            
    { char_pill4,             smallchar_pill7,             }, // 38 CH_PILL_4,            
    { char_pill5,             smallchar_pill7,             }, // 39 CH_PILL_5,            
    { char_pill6,             smallchar_pill7,             }, // 40 CH_PILL_6,            
    { char_pill7,             smallchar_pill7,             }, // 41 CH_PILL_7,            
    { char_box_locked,        smallchar_box_locked,        }, // 42 CH_BOX_LOCKED,        
    { char_box_locked2,       smallchar_box_locked2,       }, // 43 CH_BOX_LOCKED2,       
    { char_box_correct,       smallchar_box_correct,       }, // 44 CH_BOX_CORRECT,       
    { char_box_deadlocked,    smallchar_box_deadlocked,    }, // 45 CH_BOX_DEADLOCK,      
    { char_box_UtoD_top_0,    smallchar_box,               }, // 46 CH_BOX_PADLOCK,       
    { char_box_UtoD_top_0,    smallchar_box,               }, // 47 CH_BOX_UNDO,          
    { char_box_UtoD_top_0,    smallchar_box,               }, // 48 CH_BOX_UNDO_CORRECT,  
    { char_box_zoom1,    smallchar_box_deadlocked,         }, // 49 CH_BOX_ZOOM1,         
    { char_box_zoom2,    smallchar_box_deadlocked2,        }, // 50 CH_BOX_ZOOM2,         
    { char_box_zoom3,    smallchar_box_deadlocked3,        }, // 51 CH_BOX_ZOOM3,         
    { char_box_zoom4,    smallchar_box_deadlocked4,        }, // 52 CH_BOX_ZOOM4,         
    { char_box_zoom5,    C(char_parallaxBlank),            }, // 53 CH_BOX_ZOOM5,         
};

// clang-format on

// EOF
