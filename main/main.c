
#include <limits.h>
#include <stdbool.h>

#include "defines_cdfj.h"
#include "defines_from_dasm_for_c.h"

#include "defines.h"
#include "main.h"
#include "menu.h"
#include "undo.h"

#include "animations.h"
#include "atarivox.h"
#include "drawscreen.h"

#include "attribute.h"

#if ENABLE_SWIPE
#include "swipe.h"
#endif

#include "colour.h"
#include "deadlock.h"
#include "drawplayer.h"
#include "drawscreen.h"
#include "man.h"
#include "menu.h"
#include "player.h"
#include "random.h"
#include "rooms.h"
#include "score.h"
#include "scroll.h"
#include "sound.h"

////////////////////////////////////////////////////////////////////////////////
// CONFIGURABLE UX

#define DEAD_RESTART 200 /* # frames to hold trigger after loss of life, to start next life */
// #define MAX_PILLS 100 /* # random pebbles to put in dirt */

// Delays on RESET and SELECT are to allow non-ambiguous press of SELECT+RESET
// for suicide

#define RESET_DELAY 20 /* # frames to hold RESET before it is detected */

////////////////////////////////////////////////////////////////////////////////

#define MAX_CREATURE_TIME 2000 /*1000*/

int boardRow;
int boardCol;
bool triggerNextLife;

int pushingMoves;
int moves;

int showRoomCounter;

// int trig;

unsigned char *boxLocation;

bool lifeInit;
// bool rageQuit;
unsigned char *me;
// int millingTime; // negative = expired
int time60ths;

int notifyVisible;
int notifyCol;
int notifyY;
char *notifyString;

bool waitRelease;
BOUNDARY boundary; // TODO : incomplete implementation

#if ENABLE_FIREWORKS
FIREWORKS fireworks[SPLATS];
#endif

unsigned char mm_tv_type; // 0 = NTSC, 1 = PAL, 2 = PAL-60, 3 = SECAM... start @
                          // NTSC always

unsigned int sparkleTimer;

int exitMode;
enum PHASE exitPhase;

unsigned int availableIdleTime;

int gameFrame;

enum DisplayMode displayMode;
enum DisplayMode lastDisplayMode;
unsigned int triggerPressCounter;
static unsigned int triggerOffCounter;

int selectResetDelay;
int resetDelay;

int currentPalette;

int Room = 0;

bool roomCompleted;

#if ENABLE_SHAKE
int shakeX, shakeY;
int shakeTime;
#endif

bool drawNotification();
bool showNotification(char *text, int y, int colour);

static const int isActive[] = {

    ATT_PHASE1,
    ATT_PHASE1 | ATT_PHASE2,
    ATT_PHASE1 | ATT_PHASE4,
    ATT_PHASE1 | ATT_PHASE2,
};

int activated;
int selectorCounter;

unsigned char creature;
unsigned int type;

// COMPILE-TIME REVERSE BITS IN BYTE
#define RVS(a)                                                                                     \
	(((((a) >> 0) & 1) << 7) | ((((a) >> 1) & 1) << 6) | ((((a) >> 2) & 1) << 5) |                 \
	 ((((a) >> 3) & 1) << 4) | ((((a) >> 4) & 1) << 3) | ((((a) >> 5) & 1) << 2) |                 \
	 ((((a) >> 6) & 1) << 1) | ((((a) >> 7) & 1) << 0))

#define P0(a) RVS(a)
#define P1(a) P0(a), P0(a + 1)
#define P2(a) P1(a), P1(a + 2)
#define P3(a) P2(a), P2(a + 4)
#define P4(a) P3(a), P3(a + 8)
#define P5(a) P4(a), P4(a + 16)
#define P6(a) P5(a), P5(a + 32)
#define P7(a) P6(a), P6(a + 64)
#define P8(a) P7(a), P7(a + 128)

// Want to call RVS(n) for 0-255 values. The weird #defines above allows a
// single-call It's effectively a recursive power-of-two call of the base RVS
// macro

const unsigned char BitRev[] = {
    P8(0),
};

enum SCHEDULE gameSchedule;

// Function Prototypes

void GameOverscan();
void GameVerticalBlank();
void initNextLife();
void SystemReset();
void setupBoard();
void processCharAnimations();
void triggerStuff();
void drawComplete();

static void (*const runFunc[])() = {

    SystemReset,       // 0 _FN_SYSTEM_RESET
    MenuOverscan,      // 1 _FN_MENU_OS
    MenuVerticalBlank, // 2 _FN_MENU_VB
    SchedulerMenu,     // 3 _FN_MENU_IDLE
    initNextLife,      // 4 _FN_INIT_GAME_KERNEL
    GameOverscan,      // 5 _FN_GAME_OS
    GameVerticalBlank, // 6 _FN_GAME_VB
    GameIdle,          // 7 _FN_GAME_IDLE
};

int main() { // <-- 6507/ARM interfaced here!

	(*runFunc[RUN_FUNC])();
	return 0;
}

void SystemReset() {

	actualScore = APBDIV; // indicate to STELLA that we want timing values

	initRandom();
	initAudio();
	startMusic();
	initMenuDatastreams();

	swipePhase = SWIPE_GROW;

	for (int i = 0; i <= 34; i++)
		QINC[i] = 0x100; // data stream increments -> 1.0

	initKernel(KERNEL_COPYRIGHT);

	// rageQuit = false;
	ARENA_COLOUR = 1;
}

void setJumpVectors(int midKernel, int exitKernel) {

	unsigned short int *p = &RAM_SINT[_BUF_JUMP1 / 2];
	for (int i = 0; i < _ARENA_SCANLINES; i++)
		*p++ = midKernel;
	*p = exitKernel;
}

void initNewGame() {

	initScore();

	swipeType = SWIPE_STAR;
	lifeInit = true;
}

void initNextLife() {

	setJumpVectors(_NORMAL_KERNEL, _EXIT_KERNEL);

#if ENABLE_SHAKE
	shakeTime = 0;
#endif

	exitPhase = PHASE_NONE;
	roomCompleted = false;
	undoing = 0;
	boxLocation = 0;
	deadlock = 0;
	lastDeadlock = 0;

	exitMode = 0;
	sparkleTimer = 0;
	gameFrame = 0;
	triggerPressCounter = 0;
	triggerOffCounter = 0;
	notifyVisible = 0;

	resetDelay = 0;
	selectResetDelay = 0;

	lastDisplayMode = DISPLAY_NONE;

	selectorCounter = 0;

	initMan();
	initAnimations();
	initSprites();

#if ENABLE_SWIPE
	initStarSwipe();
#endif

#if ENABLE_FIREWORKS

	for (int i = 0; i < SPLATS; i++)
		fireworks[i].age = 0;
#endif

	guaranteedViewTime = 1;
	initCharAnimations();

	gameSchedule = SCHEDULE_UNPACK_Room;
}

void scheduleUnpackRoom() {

	if (KERNEL != KERNEL_GAME)
		return;

	setupBoard();

	unsigned char *b = ADDRESS_OF(0);
	for (int i = 0; i < __BOARD_SIZE; i++)
		b[i] = 0;

	// Decode the room data
	// First dummy-unpack to get dimensions of room, then center and unpack

	boundary.x = 0;
	boundary.y = 0;

	showRoomCounter = 100;
	unpackRoom(&boundary, true, Room);

	boundary.x = (__BOARD_WIDTH - boundary.width) >> 1;
	boundary.y = (__BOARD_DEPTH - boundary.height) >> 1;

	unpackRoom(&boundary, false, Room);

#if ENABLE_SWIPE

	int x, y;

	clearMask(0);
	getPlayerScreenPosition(manX, manY, &x, &y, displayMode);

	// convert sprite to PF coordinates
	y = (y * (0x100 / 3)) >> 8; // de-trixelise (x 1/3)
	x = x >> 2;

	setSwipe(x + 2, y + 4, 0, 100, SWIPE_GROW);
#endif

	gameSchedule = SCHEDULE_PROCESSBOARD;

	startCharAnimation(TYPE_PILL1, AnimateBase[TYPE_PILL1] + 9); // initial all-pill startup flash
}

void GameIdle() {

	static void (*const scheduleFunc[])() = {

	    processBoardSquares, // 0 SCHEDULE_PROCESSBOARD
	    scheduleUnpackRoom,  // 1 SCHEDULE_UNPACK_Room
	};

	(*scheduleFunc[gameSchedule])();
}

void drawWord(const char *string, int y, int colour) {

	static int wc = 0;

	if (!roller)
		if (++wc >= 8 << 2)
			wc = 1 << 2;

	unsigned char ch;

	const char *width = string;
	while (*width && *width != (char)NEW_LINE)
		width++;

	int x = (8 + width - string) >> 1;

	while ((ch = *string++) != NEW_LINE && ch) {

		if (ch != ' ') {

			if (ch >= 'A')
				ch = LETTER(ch);
			else if (ch >= '0')
				ch = ch - '0';

			drawBigDigit(ch, x, y + EXTERNAL(__sinoid)[(x + (frame >> 2)) & 15] - 3, 7);
			drawBigDigit(ch, x, y + EXTERNAL(__sinoid)[(x + (frame >> 2)) & 15] + 3, 0);
			drawBigDigit(ch, x, y + EXTERNAL(__sinoid)[(x + (frame >> 2)) & 15],
			             wc >> 2 /*colour*/);
		}
		x--;
	}
}

#define BUTTONTIME_UNDO_MODE 30
#define BUTTONTIME_EXIT_BOUNDARY 100

void checkExitWarning() {

	if (waitRelease) {

		if (triggerOffCounter > BUTTONTIME_EXIT_BOUNDARY) {

			//		if ((triggerPressCounter > BUTTONTIME_EXIT_BOUNDARY)) {
			if (triggerOffCounter & 24) {
				drawWord("EXIT", 50, 3);
				//			drawWord("ROOM", 75, 3);
			}

			if (triggerOffCounter > BUTTONTIME_EXIT_BOUNDARY + 50) {

				// rageQuit = true;
				initKernel(KERNEL_MENU);
			}
		}

	} else {

		if (triggerPressCounter > BUTTONTIME_UNDO_MODE) {

			if (!waitRelease) {

				if (scoreCycle != SCORELINE_UNDO) {
					scoreCycle = SCORELINE_UNDO;
					displayMode = DISPLAY_NORMAL;
					startAnimation(animationList[ANIM_PLAYER], ID_Undo);

				} else {

					scoreCycle = SCORELINE_TIME;
					hackStartAnimation(animationList[ANIM_PLAYER], ID_Stand);
				}

				waitRelease = true;
				triggerPressCounter = 0;
			}
		}
	}
}

void drawOverscanThings() {

	extern int lastSpriteY;
	// zap sprite buffers (expensive!)
	unsigned char *p = (unsigned char *)RAM + _BUF_GRP0A;
	for (int i = lastSpriteY; i < lastSpriteY + 24 && i < _ARENA_SCANLINES; i++) {
		p[i] = 0;
		*(p + _ARENA_SCANLINES + i) = 0;
	}

#if ENABLE_SHAKE

	if (shakeTime) {
		shakeTime--;

		shakeX = (rangeRandom(3) - 1) << SHIFT_SCROLLX;
		shakeY = (rangeRandom(7) - 3) << SHIFT_SCROLLY;
	} else
		shakeY = shakeX = 0;

#endif

	if (exitMode || !swipeComplete)
		displayMode = DISPLAY_NORMAL;

	if (displayMode == DISPLAY_OVERVIEW) {

		drawOverviewScreen(0, 11); // 11);

		if (lastDisplayMode == displayMode)
			drawPlayerSmallSprite();
	}

	else {

		if (lastDisplayMode == DISPLAY_OVERVIEW) {
			unsigned char *p = RAM + _BUF_PF0_LEFT + DIGIT_SIZE;
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < _ARENA_SCANLINES * 6; j += _ARENA_SCANLINES)
					*(p + i + j) = 0;
		}

		doSparkles();

		if (displayMode == DISPLAY_HALF) {

			drawHalfScreen(0);

			if (lastDisplayMode == displayMode)
				drawHalfSprite();
		}

		else {
			drawScreen();
			drawPlayerSprite();
		}
	}

	highlightUndo();

	lastDisplayMode = displayMode;
}

void initGameDataStreams() {

	struct ptrs {
		unsigned char dataStream;
		unsigned short buffer;
	};

	static const struct ptrs streamInits[] = {

	    {_DS_PF0_LEFT, _BUF_PF0_LEFT},
	    {_DS_PF1_LEFT, _BUF_PF1_LEFT},
	    {_DS_PF2_LEFT, _BUF_PF2_LEFT},
	    {_DS_PF0_RIGHT, _BUF_PF0_RIGHT},
	    {_DS_PF1_RIGHT, _BUF_PF1_RIGHT},
	    {_DS_PF2_RIGHT, _BUF_PF2_RIGHT},
	    {_DS_AUDV0, _BUF_AUDV},
	    {_DS_AUDC0, _BUF_AUDC},
	    {_DS_AUDF0, _BUF_AUDF},
	    {_DS_COLUPF, _BUF_COLUPF},
	    {_DS_COLUBK, _BUF_COLUBK},
	    {_DS_COLUP0, _BUF_COLUP0},
	    {_DS_COLUP1, _BUF_COLUP1},
	    {_DS_GRP0a, _BUF_GRP0A},
	    {_DS_GRP1a, _BUF_GRP1A},
#if __ENABLE_ATARIVOX
	    {_DS_SPEECH, _BUF_SPEECH},
#endif
	    {0x21, _BUF_JUMP1},
	};

	for (unsigned int i = 0; i < sizeof(streamInits) / sizeof(struct ptrs); i++)
		setPointer(streamInits[i].dataStream, streamInits[i].buffer);

#if __ENABLE_ATARIVOX
	// QINC[_DS_SPEECH] = 0;
#endif
}

void bigStuff(int amount) {

	unsigned char decimalString[6];
	*drawDecimal2(decimalString, 0, 0, amount) = 0xFF;

	int digitPos = amount >= 10 ? amount >= 100 ? amount >= 1000 ? 7 : 6 : 5 : 4;

	for (int digit = 0; decimalString[digit] <= 9; digit++, digitPos -= 2) {
		int y = 70 + EXTERNAL(__sinoid)[(digit + (frame >> 2)) & 15];
		doubleSizeScore(digitPos, y - 3, decimalString[digit], 7);
		doubleSizeScore(digitPos, y + 3, decimalString[digit], 0);
		doubleSizeScore(digitPos, y + 0, decimalString[digit], 6);
	}
}

void checkButtonRelease() {
	if (waitRelease) {
		if ((INPT4 & 0x80) && (SWCHA == 0xFF))
			waitRelease = false;
	}
}

#if ENABLE_SWIPE
void swipeToPlayer() {

	if (swipeComplete) {

		int xpos = manX + 1;
		int ypos = (manY + 1) * (CHAR_HEIGHT / 6);

		switch (displayMode) {
		case DISPLAY_NORMAL:
			ypos = (manY + 1) * CHAR_HEIGHT / 3 - (scrollY[displayMode] >> SHIFT_SCROLLY) + 2;
			xpos = manX * 4 - (scrollX[displayMode] >> SHIFT_SCROLLX) + 1;
			break;
			// case DISPLAY_HALF:
			// 	ypos =
			// 	    (manY + 1) * CHAR_HEIGHT / 4 - (scrollY[displayMode] >>
			// (SHIFT_SCROLLY + 1)); 	xpos = manX * 2 - (scrollX[displayMode] >>
			// (SHIFT_SCROLLX + 1)) + 1; break;

			// case DISPLAY_OVERVIEW:
			//     xpos = manX + 1;
			//     ypos = (manY + 1) * (CHAR_HEIGHT / 6);
			//     break;

		default:
			break;
		}

		exitPhase = PHASE_END;
		swipeType = SWIPE_CIRCLE;
		setSwipe(xpos, ypos, 378 << 8, -16384, SWIPE_SEARCH);
		ADDAUDIO(SFX_WHOOSH2);
	}
}
#endif

void handleRoomCompletion() {

	if (exitMode) {

		setScoreCycle(SCORELINE_SCORE);

		switch (exitPhase) {

		case PHASE_TIME: {
			exitPhase = PHASE_GEMS;
			break;
		}

		case PHASE_GEMS: {

			setScoreCycle(SCORELINE_TIME);

			exitPhase = PHASE_SWIPE;

			break;
		}

		case PHASE_SWIPE:

#if ENABLE_SWIPE
			swipeToPlayer();
#endif
			exitPhase = PHASE_END;
			break;

		case PHASE_END:

#if ENABLE_SWIPE
			if (swipeComplete) {
#else
			if (!--exitMode) {
#endif

				roomCompleted = true;
				exitMode = 0;
			}
			break;

		default:
			actualScore++;
			break;
		}

		//        actualScore = exitPhase;
	}
}

void GameOverscan() { // ~32000

	T1TC = 0;
	T1TCR = 1;

	frame++; // for wobbly words

	availableIdleTime = 89040;

	initGameDataStreams();
	setPalette();

	if (lifeInit) {

		initNextLife();

		availableIdleTime = 0;
		lifeInit = false;
		return;
	}

	if (gameSchedule == SCHEDULE_UNPACK_Room)
		return;

	drawOverscanThings();

	triggerStuff();
	handleSelectReset();

#if ENABLE_SWIPE
	swipe(3000);
#endif
}

bool handleSelectReset() {

	bool halt = false;

	if (roomCompleted) {

		halt = true;
		roomCompleted = false;

		initKernel(KERNEL_STATS);
		return true;
	}

	// * after death, allows button press to look around, and subsequently exit

	checkButtonRelease();
	if (!waitRelease) {

		if (!(SWCHB & 0b11) // select+reset
		    || (SWCHA == 0xFF && !(INPT4 & 0x80))) {
			selectResetDelay++;
		}

		else {

			selectResetDelay = 0;

			if (!GAME_RESET_PRESSED)
				resetDelay = 0;

			else if (++resetDelay == RESET_DELAY) {

				// rageQuit = true;

				initKernel(KERNEL_MENU);
				//                ADDAUDIO(SFX_BLIP);
				halt = true;
			}
		}
	}

	return halt;
}

void triggerStuff() {

#define DOUBLE_TAP 8
#define TOOLONG 30

	if (scoreCycle == SCORELINE_UNDO && SWCHA != 0xFF) {
		scoreCycle = SCORELINE_TIME; // exit undo mode
		hackStartAnimation(animationList[ANIM_PLAYER], ID_Stand);
		ARENA_COLOUR = 1;
		return;
	}

	if ((!pillCount && !(INPT4 & 0x80)) || (triggerNextLife
#if ENABLE_SWIPE
	                                        && swipeComplete)
#endif
	) {
		initKernel(KERNEL_MENU);
		triggerNextLife = false;
		return;
	}

	if (
#if ENABLE_SWIPE
	    swipeComplete &&
#endif
	    !waitRelease && !(INPT4 & 0x80)) { // JOY0_FIRE)

		//		FLASH(0xD2, 2);
		//		actualScore = INPT4;

		if (!triggerNextLife) {

			if (triggerOffCounter < DOUBLE_TAP && triggerPressCounter < TOOLONG &&
			    triggerOffCounter) {

				if (scoreCycle != SCORELINE_UNDO) {
					if (!exitMode) {

						// Handle double-click view mode switching

						//            ADDAUDIO(SFX_BLIP);

						switch (displayMode) {
						case DISPLAY_NORMAL:
						case DISPLAY_HALF: {
							displayMode = DISPLAY_OVERVIEW;
							// trig = 0;
							break;
						}
						default:
							displayMode = DISPLAY_NORMAL;
							break;
						}

						triggerPressCounter = 0;
						waitRelease = true;
					}
				}

			} else {

				triggerPressCounter++;
				triggerOffCounter = 0;
			}
		}

		checkExitWarning();

		// Long-press after death will restart
		if (!triggerNextLife /*&& manDead*/ && triggerPressCounter >= DEAD_RESTART) {

			FLASH(0x44, 10);
//			FLASH(0xD2, 2);
#if ENABLE_SWIPE
			//            exitPhase = PHASE_SWIPE;
			//            exitMode = 1;
			// swipeToPlayer();
#endif

			triggerNextLife = true;
		}

	} else {

		if (triggerOffCounter >= DOUBLE_TAP && triggerPressCounter) {
			if (scoreCycle != SCORELINE_UNDO) {
				if (!triggerNextLife && !exitMode) {

					if (triggerPressCounter < TOOLONG) {
						if (displayMode == DISPLAY_NORMAL)
							displayMode = DISPLAY_HALF;
						else
							displayMode = DISPLAY_NORMAL;
					}
				}

			} else {
				undoing = UNDO_SPEED;
			}
			triggerPressCounter = 0;
			triggerOffCounter = 0;
		}
		triggerOffCounter++;
	}

	if (undoing && !--undoing && undoLastMove())
		undoing = UNDO_SPEED;
}

void drawComplete() { // 32k

	//	for (int y = -1; y < 2; y += 2)

	showNotification("LEGEND", 42, 200);

	// drawWord("LEGEND", 42, 0);

	// drawWord("LEGEND", 39, 5);

	// 	static const char gameOver[] = "OK";

	// 	int digitPos = 7;
	// 	int y = 136;

	// 	for (int digit = 0; gameOver[digit]; digit++) {
	// 		int dig = gameOver[digit] - 'A' + 3 - 29;
	// 		int sin = y + EXTERNAL(__sinoid)[((frame >> 3) + digit) & 15];

	// 		if (part)
	// 			doubleSizeScore(digitPos, sin, dig, 3);
	// 		else
	// 			doubleSizeScore(digitPos, sin + 6, dig, 0);

	// 		digitPos -= 2;
	// 	}
}

const char *deadlockText = "DEADLOCK";

void GameVerticalBlank() { // ~7500

	T1TC = 0;
	T1TCR = 1;

	time60ths++;
	availableIdleTime = 115000;

	if (KERNEL != KERNEL_GAME)
		return;

	playAudio();

#if __ENABLE_ATARIVOX
	processSpeech();
#endif

	if (sparkleTimer)
		--sparkleTimer;

	if (gameSchedule != SCHEDULE_UNPACK_Room) {

		if (displayMode == DISPLAY_OVERVIEW) {
			drawOverviewScreen(11, 22);
		}

		else {

			handleRoomCompletion();

			if (time60ths && !manDead && !exitMode) {
				if ((time60ths & 0xFF) == 0xFF)
					time60ths -= 0xC4; // magic!  - (-256+60)
			}

			if (displayMode == DISPLAY_NORMAL) {
				if (showRoomCounter) {
					drawWord("ROOM", 40, 6);
					bigStuff(Room);
					showRoomCounter--;
				}

			}

			else {
				drawHalfScreen(1);
			}

			drawScore();

			// if (deadlock /*&& displayMode != DISPLAY_OVERVIEW*/) {

			// 	showNotification((char *)deadlockText, 30, 6);
			// 	// deadlockCounter--;
			// 	// drawWord("DEADLOCK", 30, 6);
			// }
		}
	}

	if (!pillCount) {

		if (!sparkleTimer)
			ADDAUDIO(SFX_EXTRA);

		sparkleTimer += 10;
		displayMode = DISPLAY_NORMAL;

		if (sparkleTimer < 1500)
			drawComplete();
	}

	Scroll();

#if ENABLE_FIREWORKS
	for (int i = 0; i < SPLATS; i++) {
		if (fireworks[i].age) {

			fireworks[i].x += fireworks[i].dX;
			fireworks[i].y += fireworks[i].dY;

			if (displayMode == DISPLAY_NORMAL) {
				int x = (fireworks[i].x >> 8) - (scrollX[DISPLAY_NORMAL] >> SHIFT_SCROLLX);
				int y = (fireworks[i].y >> 8) - (scrollY[DISPLAY_NORMAL] >> SHIFT_SCROLLY);

				drawBit(x, y, fireworks[i].colour);
			}
			fireworks[i].age--;
		}
	}
#endif

	doFlash();
	updateAnimations();
	processCharAnimations();

	checkExitWarning();
	drawNotification();

#if ENABLE_SWIPE

	swipe(20000);

#endif

#if ENABLE_SWIPE

	applySwipeMask();

#endif
}

#if ENABLE_FIREWORKS
int addFirework(int x, int y) {

	int screenX, screenY;
	getPlayerScreenPosition(x, y, &screenX, &screenY, DISPLAY_NORMAL);

	for (int i = 0; i < SPLATS; i++)
		if (!fireworks[i].age) {
			fireworks[i].x = (x * PIXELS_PER_CHAR + 2) << 8;
			fireworks[i].y = (y * (CHAR_HEIGHT / 3) + 4) << 8;
			fireworks[i].dX = ((rangeRandom(SPLAT_RANGE) - SPLAT_RANGE / 2) + SPLAT_MIN) >> 1;
			fireworks[i].dY = ((rangeRandom(SPLAT_RANGE) - SPLAT_RANGE / 2) + SPLAT_MIN);
			fireworks[i].age = 10 + rangeRandom(SPLAT_LIFESPAN);
			fireworks[i].colour = 7;
			return i;
		}
	return -1;
}

int addLocalPixel(int x, int y, int colour, int age) {

	for (int i = 0; i < SPLATS; i++)
		if (!fireworks[i].age) {
			fireworks[i].x = x << 8;
			fireworks[i].y = y << 8;
			fireworks[i].dX = 0;
			fireworks[i].dY = 0;
			fireworks[i].age = age;
			fireworks[i].colour = colour;
			return i;
		}
	return -1;
}

#endif

void setupBoard() {

	rndX = getRandom32();

	gameFrame++;

	activated = isActive[++selectorCounter & 3];

	boardCol = -1; // boundary.x - 1;
	boardRow = 0;  // boundary.y;

	me = RAM + _BOARD - 1;
}

void pulse(unsigned char *cell, int baseChar) {

	rndX = getRandom32(); // (rndX >> 31) | (rndX << 1);

	unsigned char dType = ((rndX & 0xFF) * 4) >> 8;
	*cell = baseChar + dType;
}

bool drawNotification() {
	if (notifyVisible) {
		notifyVisible--;
		if (displayMode != DISPLAY_OVERVIEW)
			drawWord(notifyString, notifyY, notifyCol);
	}
}

bool showNotification(char *text, int y, int colour) {
	//	if (text != notifyString) {
	notifyString = text;
	notifyVisible = 100;
	notifyY = y;
	notifyCol = colour;
	//	}
}

bool processType() {

	switch (type) {

	case TYPE_BOX_UNDO:
		if (!*(Animate[TYPE_BOX_UNDO] + 1))
			*me = CH_BOX;
		break;

	case TYPE_BOX_UNDO_CORRECT:
		if (!*(Animate[TYPE_BOX_UNDO_CORRECT] + 1)) {
			*me = CH_BOX_LOCKED;
		}

		//		for (int i = 0; i < 3; i++)
		addFirework(boardCol, boardRow);
		break;

	case TYPE_BOX_CORRECT: {

		if (!*(Animate[TYPE_BOX_CORRECT] + 1)) {
			*me = CH_BOX_LOCKED;
		}

		//		for (int i = 0; i < 3; i++)
		addFirework(boardCol, boardRow);

		// if (selectResetDelay > DEAD_RESTART_COUCH ||
		//     (selectResetDelay > DEAD_RESTART && GAME_RESET_PRESSED &&
		//     GAME_SELECT_PRESSED) ||
		//     ((!time60ths /*&& !invincible*/) /*&& !terminalDelay*/)) {

		// 	startAnimation(animationList[ANIM_PLAYER], ID_Die);
		// 	//                            *me = CH_BLANK;
		// }
	} break;

	default:
		return false;
	}

	return true;
}

void processCreature() {

	switch (creature) {
	default:
		break;
	}

	// if (boxLocation) {

	// 	int ch = ~FLAG_THISFRAME & *boxLocation;
	// 	int frame = *Animate[CharToType[ch]];

	// 	switch (ch) {
	// 	case CH_BOX_LTOR_LEFT_0:
	// 		if (frame == CH_BLANK) {
	// 			*boxLocation = CH_BLANK;
	// 			*(boxLocation + 1) = CH_BOX;
	// 			boxLocation = 0;
	// 		}
	// 		break;

	// 	case CH_BOX_LTOR_RIGHT_1:
	// 		if (frame == CH_BLANK) {
	// 			*boxLocation = CH_BLANK;
	// 			*(boxLocation - 1) = CH_BOX;
	// 			boxLocation = 0;
	// 		}
	// 		break;

	// 	case CH_BOX_UTOD_TOP_0:
	// 		if (frame == CH_BLANK) {
	// 			*boxLocation = CH_BLANK;
	// 			*(boxLocation + _1ROW) = CH_BOX;
	// 			boxLocation = 0;
	// 		}
	// 		break;

	// 	case CH_BOX_UTOD_BOTTOM_1:
	// 		if (frame == CH_BLANK) {
	// 			*boxLocation = CH_BLANK;
	// 			*(boxLocation - _1ROW) = CH_BOX;
	// 			boxLocation = 0;
	// 		}
	// 		break;
	// 	}
	// }
}

char dl[] = "?? STUCK";

void processBoardSquares() {

	while (T1TC + MAX_CREATURE_TIME < availableIdleTime) {

		rndX = (rndX >> 1) | (rndX << 31);

		if (++boardCol >= __BOARD_WIDTH) {
			if (++boardRow >= __BOARD_DEPTH) {

				setupBoard();

				if (!exitMode) {
					moveMan();
				}

				if (selectResetDelay > DEAD_RESTART_COUCH ||
				    (selectResetDelay > DEAD_RESTART && GAME_RESET_PRESSED &&
				     GAME_SELECT_PRESSED) ||
				    ((!time60ths /*&& !invincible*/) /*&& !terminalDelay*/)) {

					startAnimation(animationList[ANIM_PLAYER], ID_Die);
					//                            *me = CH_BLANK;
				}

				if (deadlock > lastDeadlock) {

					if (scoreCycle != SCORELINE_UNDO)
						ADDAUDIO(SFX_WHOOSH2);

					int dd = deadlock;
					int d10 = 0;
					while (dd >= 10) {
						d10++;
						dd -= 10;
					}

					//					int d10 = (deadlock * (256 / 10)) >> 8;
					int d = dd; // deadlock - d10 * 10;

					dl[0] = d10 ? '0' + d10 : ' ';
					dl[1] = '0' + d;

					showNotification(dl, 30, 7);
				}
				lastDeadlock = deadlock;
				deadlock = 0;

				if (!deadlock && deadlockCounter == 1)
					deadlockCounter = 0;

				return;
			}

			boardCol = 0;
		}

		//        int start = T1TC;

		me++;

		creature = *me;

		if (!(creature & FLAG_THISFRAME)) {

			type = CharToType[creature];

			if (Attribute[type] & activated) {

				extern bool isBox(unsigned char *me);
				if (isBox(me))
					checkDeadlocks();

				if (!processType())
					processCreature();
			}
		}

		// Clear any "scanned this frame" objects on the previous line
		// note: we need to also do the last row ... or do we? if it's steel wall,
		// no
		if (boardRow > 1) {
			unsigned char *prev = me - _1ROW;
			*prev &= ~FLAG_THISFRAME;
		}
	}
}

// EOF