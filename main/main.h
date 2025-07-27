#ifndef __MAIN_H
#define __MAIN_H

// public repository test

#include "defines_from_dasm_for_c.h"
#include <stdbool.h>

#define ENABLE_SOUND 1            /* */
#define ENABLE_SHAKE 1            /* */
#define ENABLE_IDLE_ANIMATION 1   /* */
#define ENABLE_SECAM 0            /* 88 bytes */
#define ENABLE_60MHZ_AUTODETECT 1 /* 16 bytes */
#define ENABLE_ANIMATING_MAN 0    /* 244 bytes but man disappears */
#define ENABLE_SWIPE 1            /* 1108 bytes */
#define ENABLE_SERIAL_NUMBER 1    /* 232 bytes(with 'halfsize' routine) */
#define ENABLE_FIREWORKS 1        /* 244 bytes - a pixel-draw used for a fireworks starburst */
#define ENABLE_UNDO_DOTGUIDE 0    /* places a snail trail on boxes when undoing */
#define ENABLE_PLAYERCELL 1

// Characterset dimensions for each view...
#define CHAR_HEIGHT 27
#define CHAR_HEIGHT_HALF 18
#define CHAR_HEIGHT_OVERVIEW 9

#define SPRITE_DEPTH 23

// range: slowest --> 7 6 5 4 3 <-- fastest

#define PIXELS_PER_CHAR 5
#define PIXELS_PER_CHAR_HALF 3
#define PIXELS_PER_PF 4

#define HALFWAYX 20
#define HALFWAYY 28
#define TRILINES (CHAR_HEIGHT / 3)

#define SCORE_SCANLINES 21
#define SCANLINES (_ARENA_SCANLINES - SCORE_SCANLINES)

#define _1ROW 40

#define CR "\xFF"
#define ADDRESS_OF(row) RAM + _BOARD + (row) * __BOARD_WIDTH

void setJumpVectors(int midKernel, int exitKernel);
void InitializeNewGame();
void roomUnpack(int roomNumber, int center);
void clearBoard();

void drawWord(const char *string, int y, int colour);

enum DisplayMode { DISPLAY_NORMAL, DISPLAY_HALF, DISPLAY_OVERVIEW, DISPLAY_NONE };

extern enum DisplayMode displayMode, lastDisplayMode;

extern const unsigned char BitRev[];
extern int boardRow, boardCol;

// struct Animation {
// 	signed char index;
// 	signed char count;
// };

#define setPointer(fetcher, offset) QPTR[fetcher] = (offset) << 20;

#define FLAG_THISFRAME 0x80

#define DEAD_RESTART_COUCH 200
#define EXTERNAL(i) ((const unsigned char *)(long)(i))

// extern const unsigned char iCC_skull[];
extern const unsigned char joyDirectBit[4];
extern unsigned char mm_tv_type;

extern int Room;

#if ENABLE_SHAKE
extern int shakeTime;
extern int shakeX, shakeY;
#endif

extern unsigned char enablePALPalette;
extern unsigned char *boxLocation;

enum SCHEDULE {
	// SCHEDULE_START,
	SCHEDULE_PROCESSBOARD,
	SCHEDULE_UNPACK_Room,
};

extern enum SCHEDULE gameSchedule;
extern int gameSpeed;
extern int gameFrame;

#if ENABLE_FIREWORKS
#define SPLATS 20
#define SPLAT_LIFESPAN 40
#define SPLAT_MIN 0x10
#define SPLAT_RANGE 0x90
#define SPLAT_SPEED (SPLAT_MIN + SPLAT_RANGE)
typedef struct {
	// unsigned char row, column;
	short int x;  // 8.8 fixed point
	short int y;  // 8.8 fixed point
	short int dX; // 8.8 fixed point
	short int dY; // 8.8 fixed point
	unsigned char age;
	char colour;
} FIREWORKS;

extern FIREWORKS fireworks[SPLATS];

int addFirework(int x, int y);
int addLocalPixel(int x, int y, int colour, int age);
#endif

enum KERNEL_TYPE {
	KERNEL_COPYRIGHT,
	KERNEL_MENU,
	KERNEL_GAME,
	KERNEL_STATS,
};

enum PHASE { PHASE_NONE = 0, PHASE_TIME = 2, PHASE_GEMS = 3, PHASE_SWIPE = 4, PHASE_END = 5 };
extern enum PHASE exitPhase;

extern int frame;

extern int pushingMoves;
extern int moves;
extern int lives;
extern int time60ths;
extern bool waitRelease;
// extern bool rageQuit;
extern unsigned int sparkleTimer;
extern int perfectBonus;

extern int exitMode;

extern int selectResetDelay;
extern unsigned char *me;

extern int boardCol;
extern bool triggerNextLife;

extern unsigned int triggerPressCounter;
extern bool enableICC;

extern bool roomCompleted;

enum FaceDirection {
	FACE_LEFT = -1,
	FACE_RIGHT = 1,
	FACE_UP = -1,
	FACE_DOWN = 1,
};

extern int currentPalette;
extern unsigned int availableIdleTime;
extern int showRoomCounter;
extern int theRoomNumber;

void GameIdle();
void processBoardSquares();
void reanimateDiamond(unsigned char *me);
bool handleSelectReset();
void initNewGame();
void checkButtonRelease();
void requestKernel(int kernel);
void bigStuff(int amount);

int halfSize(int x, int y, int letter, bool render);

#define GET(a) (((unsigned char)((a) << 1)) >> 1)

#define NEW_LINE 0xFF
#define END_STRING NEW_LINE, NEW_LINE

#endif
