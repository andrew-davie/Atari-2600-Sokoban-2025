// clang-format off

#include <limits.h>

#include "attribute.h"
#include "defines.h"
#include "defines_cdfj.h"

#include "main.h"
#include "menu.h"

#include "animations.h"
#include "atarivox.h"
#include "bitpatterns.h"
#include "characterset.h"
#include "colour.h"
#include "drawscreen.h"
#include "joystick.h"
#include "player.h"
#include "random.h"
#include "man.h"
#include "score.h"
#include "sound.h"
#include "rooms.h"


#if ENABLE_SERIAL_NUMBER
int halfSize(int x, int y, int letter, bool render);
#endif

#define LUMSHIFT 3              /* speed of title pulsing -- 0 = fast, 1 is slower, etc. */


void initCopyrightScreen();
void resetMode();


static int sin = 0;

#include "../iCC_title.c"

const unsigned char iCC_title_colour[] = {

    0xCA, 0x28, 0x34,
};

// clang-format on

#if DEBUG_TIMER
unsigned int debugDelay = 0;
#endif

#define LETTER_HEIGHT 10

// #define OPTION_SYSTEM 2

void doubleSize(int x, int y, int letter, unsigned char colour);

bool enableICC = true;
// int menuLineTVType;

static int mustWatchDelay;
static int pushFrame;
int frame;
static int pushCount;
static unsigned int menuLine;
static int RoomUnpackComplete;
unsigned int detectedPeriod;

int thumbnailSpeed;

unsigned int sline = 0;

void detectConsoleType() {

	switch (frame) {

	case 0:

		mm_tv_type = NTSC; // force NTSC frame for autodetect purposes

		T1TC = 0;
		T1TCR = 1;
		break;

	case DETECT_FRAME_COUNT: {

		detectedPeriod = T1TC;

		static const struct fmt {

			int frequency;
			unsigned char format;

		} mapTimeToFormat[] = {

#define NTSC_70MHZ (0xB240F6 * DETECT_FRAME_COUNT / 10)
#define PAL_70MHZ (0xB3E40D * DETECT_FRAME_COUNT / 10)

		    {
		        NTSC_70MHZ,
		        NTSC,
		    },

#if ENABLE_SECAM
#define SECAM_70MHZ (0xB294EA * DETECT_FRAME_COUNT / 10)
		    {
		        SECAM_70MHZ,
		        SECAM,
		    },
#endif
		    {
		        PAL_70MHZ,
		        PAL_60,
		    },

#if ENABLE_60MHZ_AUTODETECT

#define NTSC_60MHZ (0x98EB2F * DETECT_FRAME_COUNT / 10)
#define PAL_60MHZ (0x9A0EEF * DETECT_FRAME_COUNT / 10)
#define SECAM_60MHZ (((PAL_60MHZ - NTSC_60MHZ) / 2 + NTSC_60MHZ) * DETECT_FRAME_COUNT / 10)

		    {
		        NTSC_60MHZ,
		        NTSC,
		    },
		    {
		        SECAM_60MHZ,
		        SECAM,
		    },
		    {
		        PAL_60MHZ,
		        PAL_60,
		    },
#endif
		};

		int delta = INT_MAX;
		for (unsigned int i = 0; i < sizeof(mapTimeToFormat) / sizeof(struct fmt); i++) {

			int dist = detectedPeriod - mapTimeToFormat[i].frequency;
			if (dist < 0)
				dist = -dist;

			if (dist < delta) {
				delta = dist;
				mm_tv_type = mapTimeToFormat[i].format;
			}
		}

		// TV_TYPE = mm_tv_type;

		initCopyrightScreen();
		break;
	}

	default:
		break;
	}

	//    frame++;
}

void zeroBuffer(int *buffer, int size) {
	for (int i = 0; i < size; i++)
		*buffer++ = 0;
}

void initMenuDatastreams() {

	static const struct ptrs {

		unsigned char dataStream;
		unsigned short buffer;

	} streamInits[] = {

	    {_DS_PF1_LEFT, _BUF_MENU_PF1_LEFT},
	    {_DS_PF2_LEFT, _BUF_MENU_PF2_LEFT},
	    {_DS_PF1_RIGHT, _BUF_MENU_PF1_RIGHT},
	    {_DS_PF2_RIGHT, _BUF_MENU_PF2_RIGHT},
	    {_DS_AUDV0, _BUF_AUDV},
	    {_DS_AUDC0, _BUF_AUDC},
	    {_DS_AUDF0, _BUF_AUDF},
#if __ENABLE_ATARIVOX
	    {_DS_SPEECH, _BUF_SPEECH},
#endif
	    {_DS_COLUPF, _BUF_MENU_COLUPF},
	    {_DS_COLUP0, _BUF_MENU_COLUP0},
	    {_DS_GRP0a, _BUF_MENU_GRP0A},
	    {_DS_GRP1a, _BUF_MENU_GRP1A},
	    {_DS_GRP0b, _BUF_MENU_GRP0B},
	    {_DS_GRP1b, _BUF_MENU_GRP1B},
	    {_DS_GRP0c, _BUF_MENU_GRP0C},
	    {_DS_GRP1c, _BUF_MENU_GRP1C},
	    {0x21, _BUF_JUMP1},

	};

	for (unsigned int i = 0; i < sizeof(streamInits) / sizeof(struct ptrs); i++)
		setPointer(streamInits[i].dataStream, streamInits[i].buffer);

	// QINC[_DS_SPEECH] = 0;
}

#if ENABLE_ANIMATING_MAN
const unsigned char manPushing[][73] = {

    {
        72,  0,   0,
        0,   14,  14,
        0,   30,  30,
        0,   24,  28,
        4,   16,  90,
        74,  16,  92,
        76,  64,  89,
        89,  64,  125,
        125, 97,  113,
        113, 115, 115,
        115, 127, 126,
        127, 124, 124,
        124, 120, 104,
        120, 240, 240,
        240, 32,  32,
        32,  192, 0,
        0,   160, 0,
        0,   240, 0,
        0,   120, 0,
        0,   120, 0,
        0,   208, 0,
        0,   0,   144,
        0,   0,   144 + 64 + 8,
        0,   0,   144 + 64 + 8,
        0,
    },

    {
        72,  28,  28,  0,   60,  60,  0,   48,  56,          8,   32,  180,         148, 32,  184,
        152, 128, 176, 176, 128, 186, 186, 224, 242,         242, 242, 242,         242, 254, 254,
        254, 126, 124, 126, 124, 108, 124, 112, 112,         112, 112, 112,         112, 16,  16,
        16,  96,  0,   0,   80,  0,   0,   120, 0,           0,   120, 0,           0,   88,  0,
        0,   88,  0,   0,   0,   72,  0,   0,   72 + 32 + 4, 0,   0,   72 + 32 + 4, 0,
    },
};
#endif

char RGB[6] = {0x26};

void doDrawBitmap(const unsigned char *shape, int x, int y) {

	unsigned char *pf1L = RAM + _BUF_MENU_PF1_LEFT + y;
	unsigned char *pf2L = pf1L + _ARENA_SCANLINES;
	unsigned char *pf1R = pf2L + _ARENA_SCANLINES;
	unsigned char *pf2R = pf1R + _ARENA_SCANLINES;

	int size = shape[0];
	const unsigned char *bf = shape + 1;

	int baseRoll = roller;

	union g {
		int bGraphic;
		unsigned char g[4];
	} gfx;

	union masker {
		int mask;
		unsigned char mask2[4];
	} mask;

	for (int i = 0; i < size; i += 3) {

		mask.mask = ((bf[0] | bf[1] | bf[2]) << x) ^ 0xFFFFFFFF;

		for (int line = 0; line < 3; line++) {

			gfx.bGraphic = bf[baseRoll] << x;

			*pf1L = (*pf1L & mask.mask2[3]) | gfx.g[3];
			*pf1R = (*pf1R & BitRev[mask.mask2[0]]) | BitRev[gfx.g[0]];
			*pf2R = (*pf2R & mask.mask2[1]) | gfx.g[1];
			*pf2L = (*pf2L & BitRev[mask.mask2[2]]) | BitRev[gfx.g[2]];

			if (++baseRoll > 2)
				baseRoll = 0;

			pf1L++;
			pf1R++;
			pf2L++;
			pf2R++;
		}

		bf += 3;
	}
}

#if ENABLE_ANIMATING_MAN
void doMan() {

	if (--pushCount < 0 && !rangeRandom(256))
		pushCount = rangeRandom(32) | 32;

	pushFrame = pushCount < 0;

	int jiggle = 0;
	if (!pushFrame)
		jiggle = -((gameFrame >> 2) & 1) * 3;

	doDrawBitmap(manPushing[pushFrame], 23, 120 + jiggle);
}
#endif

// clang-format off

const unsigned char wordStartAt[] = {
    XXX___XX, ___XX__X, __X_____, ________, ________, ________,
    XXXX_XX_, X_XX_X_X, XXX_____, ________, ________, ________,
    X__X_X__, X_X__X_X, XXX_____, ________, ________, ________,
    XXXX_X__, X_X__X_X, __X_____, ________, ________, ________,
    XXX__XXX, X_XXXX_X, __X_____, ________, ________, ________,
    X__X__XX, ___XX__X, __X_____, ________, ________, ________,
};

const unsigned char wordDisplay[] = {
    XX___XXX, __XXX___,  ________, ________, ________, ________,
    ____XX__, _XX_____,  ________, ________, ________, ________,
    XX__X___, _X______,  ________, ________, ________, ________,
    _X__X___, _X______,  ________, ________, ________, ________,
    XXX_XXXX, _XXXX___,  ________, ________, ________, ________,
    XXX__XXX, X_XXXX__,  ________, ________, ________, ________,
};

// const unsigned char wordDifficulty[] = {

//     XXX__XXX, _XXXX_XX, XX_XXX__, XXX_X__X, _X__XXXX, XX___XX_,
//     X_XX_XXX, _XXX__XX, X__XXX_X, X___X__X, _X__XXXX, _XX_XX__,
//     X__XX_X_, _X____X_, ____X__X, ____X__X, _X____X_, __XXX___,
//     X__XX_X_, _XXX__XX, X___X__X, X___X__X, _X____X_, ___X____,
//     XXXXX_X_, _X____X_, ____X__X, XXX_XXXX, _XXXX_X_, ___X____,
//     XXXX_XXX, _X____X_, ___XXX__, XXX__XX_, _XXXX_X_, ___X____,
// };





#if ENABLE_EASTER_MYNAME

const unsigned char andrewDavie[] = {
    ______XX, _______X, ________, _______X, X_______, ___X____,
    _____X_X, _XX___XX, __XX_XX_, X__X___X, _X__XX_X, _X____XX,
    ____X__X, _XXX_X_X, _XX_X_X_, X__X___X, __X__X_X, _X_X_X_X,
    ____XXXX, _X_X_X_X, _X__XXX_, X__X___X, __XXXX_X, _X_X_XXX,
    ____X__X, _X_X_XXX, _X__X___, XXXX___X, XX_X_X_X, XX_X_X__,
    ____X__X, _X_X_XXX, _X___XX_, X__X___X, X__XXX__, X__X__XX,
};


// const unsigned char jurgenOster[] = {
//     ___XX_X_, X_______, ________, _____XX_, ______X_, ________,
//     ____X___, ___XX__X, X__X__XX, ____X__X, __XX_XXX, __X___XX,
//     ____X_X_, X_X___X_, X_X_X_X_, X___X__X, _X____X_, _X_X_X__,
//     ____X_X_, X_X____X, X_XXX_X_, X___X__X, __XX__X_, _XXX_X__,
//     ____X_X_, X_X_____, X_X___X_, X___X__X, ____X_X_, _X___X__,
//     __XX___X, X_X___XX, ___XX_X_, X____XX_, __XX___X, __XX_X__,
// };


#endif

// clang-format on

void drawPalette(const unsigned char *palette) {

	unsigned char *p = RAM + _BUF_MENU_COLUPF;

	unsigned char pal[3];

	int baseRoller = roller;
	for (int i = 0; i < 3; i++) {
		pal[i] = convertColour(palette[baseRoller]);
		if (++baseRoller > 2)
			baseRoller = 0;
	}

	for (int i = 0; i < _ARENA_SCANLINES; i += 3) {
		p[i] = pal[0];
		p[i + 1] = pal[1];
		p[i + 2] = pal[2];
	}
}

void drawCharacter(int x, int y, int ch) {

	ch -= '>';
	if (ch < 0)
		ch += 43;
	const unsigned char *p = EXTERNAL(__CHAR_A_TO_Z) + ch * 10;
	unsigned char *col = RAM + _BUF_MENU_GRP0A + _ARENA_SCANLINES * x + y;

	for (int i = 0; i < LETTER_HEIGHT; i++)
		*col++ = *p++;
}

void drawString(int x, int y, const char *text, int colour) {

	while (*text && x < 6)
		drawCharacter(x++, y, *text++);

	for (int i = 0; i < LETTER_HEIGHT; i++)
		RAM[_BUF_MENU_COLUP0 + i + y - 1] = convertColour(colour);
}

void drawSmallProxy(unsigned char colour, int y, const unsigned char *smallText) {

	for (int line = 0; line < 6; line++) {
		RAM[_BUF_MENU_COLUP0 + y + line - 1] = colour;
		for (int i = 0; i < 6; i++)
			RAM[_BUF_MENU_GRP0A + _ARENA_SCANLINES * i + y + line] |= *smallText++;
	}
}

void drawSmallString(int y, const unsigned char *smallText, int colour) {
	drawSmallProxy(convertColour(colour), y, smallText);
}

char showRoom[] = {"   "};

// const char TV[][6] = {

//     {"NTSC;;"},
//     {"PAL;;;"},
//     {"PAL60;"},
// #if ENABLE_SECAM
//     {"SECAM;"},
// #endif
// };

const char Level[][6] = {

    {"NORMAL"}, {"MEDIUM"}, {"HARD>>"}, {"EXPERT"}, {"SUPER>"},
};

const char displayOption[][6] = {
    {"OFF>>>"},
    {"ON>>>>"},
};

// clang-format on

const unsigned char *smallWord[] = {
    wordStartAt, wordDisplay
    //    wordTvType0,
};

void SchedulerMenu() {}

void setTitleMarqueeColours() {
	unsigned char *const p = RAM + _BUF_MENU_COLUPF;

	int baseRoller = roller + 1;
	for (int i = 0; i < 69; i++) {

		if (++baseRoller > 2)
			baseRoller -= 3;

		int offset = i < 48 ? 0 : 3;
		p[i] = RGB[baseRoller + offset];
	}
}

// int showAuthor;

void handleMenuScreen() {

	sline++;
	if (sline >= sizeof(smallWord) / sizeof(smallWord[0]))
		sline = 0;

	int y = sline * 24 + 96;

	if (gameFrame)
		--gameFrame;

	const char *dLine = 0;

	switch (sline) {

	case 0: {

		showRoom[0] = '0';
		showRoom[1] = '0';
		showRoom[2] = '0';

		int ct = Room;
		while (ct >= 100) {
			showRoom[0]++;
			ct -= 100;
		}

		while (ct >= 10) {
			showRoom[1]++;
			ct -= 10;
		}

		showRoom[2] = '0' + ct;

		dLine = showRoom;

	} break;

	case 1:
	case 2:
		dLine = displayOption[enableICC];
		break;
	}

	if (dLine) {

		drawSmallString(y, smallWord[sline], sline == menuLine ? 0xA : 0x9A);

		int colour =
		    sline == menuLine ? (gameFrame & 4) ? 0x8 : ((++sin << 4) & 0xF0) | 0x18 : 0xB8;
		drawString(0, y + 8, dLine, colour);
	}

#if ENABLE_SERIAL_NUMBER

	if (GAME_SELECT_PRESSED && GAME_RESET_PRESSED) {

		// for (int i = 179; i < 190; i++)
		//     *(RAM + _BUF_MENU_COLUP0 + i) = 0xA;

		char ch;
		int x = 0;

#include "date2.txt"

		while ((ch = *name++))
			x += halfSize(x, 180, ch, true);
	}
#endif
}

void initCopyrightScreen() {

	// const unsigned char *p = EXTERNAL(__COPYRIGHT_START);
	// unsigned char *r = RAM + _BUF_MENU_GRP0A;

	// const int rStart = (_ARENA_SCANLINES - __COPYRIGHT_ROWS) / 2;
	// const int rEnd = (_ARENA_SCANLINES + __COPYRIGHT_ROWS) / 2;

	// for (int col = 0; col < 6; col++) {
	// 	for (int row = rStart; row < rEnd; row++) {
	// 		RAM[_BUF_MENU_COLUP0 + row - 1] = convertColour(row < 53 ? 0x28
	// : 0x42); // multi OK 		r[row] = *p++;
	// 	}
	// 	r += _ARENA_SCANLINES;
	// }
}

void initKernel(int kernel) {

	// T1TC = 0;
	// T1TCR = 1; // tmp

	KERNEL = kernel;

	setJumpVectors(_MENU_KERNEL, _EXIT_MENU_KERNEL);

	killRepeatingAudio();

	gameSchedule = SCHEDULE_UNPACK_Room;

	// ARENA_COLOUR = 1;

	P0_X = 80; // sets the menu sprites position
	P1_X = 88;

	waitRelease = true;
	sound_max_volume = VOLUME_MAX;

	zeroBuffer((int *)(RAM + _BUF_MENU_COLUPF),
	           12 * _ARENA_SCANLINES / 4); // dubious
	chooseColourScheme();

	switch (kernel) {

	case KERNEL_COPYRIGHT:

		frame = 0;          // for auto-detect
		currentPalette = 0; // 4;

		mustWatchDelay = MUSTWATCH_COPYRIGHT;
		break;

	case KERNEL_MENU:

		// rndX = getRandom32();

		for (int i = 0; i < 6; i++)
			RGB[i] = 0x46;

		menuLine = 0;

		if (rageQuit) {

			SAY(__WORD_RAGEQUIT);

			sound_volume = VOLUME_NONPLAYING;
		}

		int delta = VOLUME_NONPLAYING - sound_volume;
		sound_volume += (delta > 0) - (delta < 0);

		// if (sound_volume < VOLUME_NONPLAYING)
		//     sound_volume++;
		// else if (sound_volume > VOLUME_NONPLAYING)
		//     sound_volume--;

		resetMode();

		mustWatchDelay = MUSTWATCH_MENU;

		break;

	case KERNEL_STATS: {

		// unsigned char *p = RAM + _BUF_MENU_PF1_LEFT;
		// for (int i = 0; i < 4 * _ARENA_SCANLINES; i++)
		//     p[i] = 0;

		mustWatchDelay = MUSTWATCH_STATS;
		RoomUnpackComplete = false;
		thumbnailSpeed = -10;

		// decode(Room);
		setBackgroundPalette(EXTERNAL(__COLOUR_POOL) + currentPalette);
		initCharAnimations();
		break;
	}

	case KERNEL_GAME: {

		extern bool lifeInit;
		lifeInit = true;

		sound_max_volume = VOLUME_PLAYING;

		break;
	}
	}
}

void MenuOverscan() {

	// T1TC = 0;
	// T1TCR = 1; // tmp

	initMenuDatastreams();
	rndX = getRandom32();

#if __ENABLE_ATARIVOX
	processSpeech();
#endif

	playAudio();

	zeroBuffer((int *)(RAM + _BUF_MENU_PF1_LEFT), _ARENA_SCANLINES);

	// zeroBuffer((int *)(RAM + _BUF_MENU_COLUPF), 12 * _ARENA_SCANLINES / 4);

	// #else
	//     zeroBuffer((int *)(RAM + _BUF_MENU_PF1_LEFT), _ARENA_SCANLINES);
	// #endif

	switch (KERNEL) {

	case KERNEL_COPYRIGHT:

		detectConsoleType();
		break;

	case KERNEL_MENU:

		handleMenuScreen();
		break;

	case KERNEL_STATS: // overscan
		break;

	default:
		break;
	}

	// if (T1TC > detectedPeriod)
	//     detectedPeriod = T1TC; // tmp   ~0x19500

	// detectedPeriod = T1TC; // tmp

	frame++;
}

int setBounds(int value, int max) {
	if (value < 0)
		value = max;
	if (value > max)
		value = 0;
	return value;
}

void resetMode() {

	gameFrame = 16;
	waitRelease = true;

	pushCount = (rndX & 31) | 32;
}

void drawICCScreen(const unsigned char *icc) {

	drawPalette(iCC_title_colour);

	unsigned char *pf1L = RAM + _BUF_MENU_PF1_LEFT;
	for (int line = 0; line < _ARENA_SCANLINES << 2; line += 3) {

		for (int i = 0; i < 3; i++) {

			*pf1L++ = icc[roller];

			if (++roller > 2)
				roller = 0;
		}
		icc += 3;
	}
}

void handleMenuVB() {

	if (!waitRelease && !(INPT4 & 0x80)) {

		ADDAUDIO(SFX_UNCOVERED);

		initNewGame();
		initKernel(KERNEL_STATS);
		return;
	}

	interleaveColour();

	if (!(INPT4 & 0x80) && !waitRelease) {
		rageQuit = false;
		waitRelease = true;
	}

	drawICCScreen(iCC_title);
	setTitleMarqueeColours();

#if ENABLE_ANIMATING_MAN
	doMan();
#endif

	static int lum = 0;
	static const unsigned int lumOffset[16] = {0, 2, 4, 6, 8, 8, 8, 8, 8, 8, 8, 8, 6, 4, 2, 0};

	for (int i = 0; i < 3; i++) {

		if (!lum && i < 3)
			RGB[i] = (RGB[i] & 0xF) | (getRandom32() << 4);

		RGB[i] = lumOffset[(lum >> LUMSHIFT)] | (RGB[i] & 0xF0);
	}

	if ((++lum >> LUMSHIFT) >= 16)
		lum = 0;

	int negJoy = (SWCHA >> 4) ^ 0xF;

	if (!waitRelease) {

		int dir = yInc[negJoy];

		if (dir)
			menuLine = setBounds(menuLine + dir, sizeof(smallWord) / sizeof(smallWord[0]) - 1);

		else {

			dir = xInc[negJoy];
			if (dir) {

				ADDAUDIO(SFX_SCORE);
				switch (menuLine) {

				case 0:

					Room = setBounds(Room + dir, getRoomCount() - 1);
					break;

				case 1:
				case 2:
					enableICC = !enableICC;
					break;
				}
			}
		}

		if (dir) {
			resetMode();
			mustWatchDelay = MUSTWATCH_MENU;
			ADDAUDIO(SFX_BLIP);
		}
	}
}

void MenuVerticalBlank() {

#if __ENABLE_ATARIVOX
	processSpeech();
#endif

	getJoystick();
	checkButtonRelease();
	doFlash();

	switch (KERNEL) {

	case KERNEL_STATS: { // VBlank

		initKernel(KERNEL_GAME); //<-- goes overtime (264)
		return;
		break;
	}

	case KERNEL_COPYRIGHT: // VBlank

		if (!--mustWatchDelay) {
			initKernel(KERNEL_MENU);
			return;
		}

		interleaveColour();
		drawPalette(EXTERNAL(__COLOUR_POOL));
		break;

	case KERNEL_MENU:

		handleMenuVB();
		break;
	}
}

void doubleSize(int x, int y, int letter, unsigned char colour) {

	const unsigned char *gfx = EXTERNAL(__CHAR_A_TO_Z) + letter * LETTER_HEIGHT;

	unsigned char *col0 = RAM + _BUF_MENU_GRP0A + _ARENA_SCANLINES * x + y;
	unsigned char *p = RAM + _BUF_MENU_COLUP0 + y - 1;

	for (int line = 0; line < LETTER_HEIGHT; line++) {

		int doubledByte = 0;
		for (int i = 7; i >= 0; --i) {
			unsigned char bit = (gfx[line] >> i) & 1;
			doubledByte = (doubledByte << 2) | (bit << 1) | bit;
		}

		for (int stretch = 0; stretch < 3; stretch++) {
			*col0 = doubledByte >> 8;
			*(col0++ + _ARENA_SCANLINES) = doubledByte;
			*p++ = colour;
		}
	}
}

#if ENABLE_SERIAL_NUMBER
int halfSize(int x, int y, int letter, bool render) {

	int col = x >> 3;
	int shift = 8 - (x & 7);

	if (letter <= '9')
		letter = letter - '0' + 'Z' + 1;

	const unsigned char *gfx =
	    EXTERNAL(__CHAR_A_TO_Z) + letter * LETTER_HEIGHT - ('>' * LETTER_HEIGHT);
	unsigned char *col0 = RAM + _BUF_MENU_GRP0A + _ARENA_SCANLINES * col + y;

	unsigned char merged = 0;
	for (int line = 0; line < LETTER_HEIGHT; line++)
		merged |= gfx[line];

	while (merged && !(merged & 128)) {
		merged <<= 2;
		shift++;
	}

	int width = 1;
	while (merged) {
		merged <<= 2;
		width++;
	}

	if (render)
		for (int line = 0; line < LETTER_HEIGHT; line++) {

			unsigned int c = gfx[line];

			// clang-format off
            unsigned int half = (

                (c >> 7 << 31 >> 24 |    // 128
                 c >> 5 << 31 >> 25 |    // 32
                 c >> 3 << 31 >> 26 |    // 8
                 c >> 1 << 31 >> 27 ))   // 2

                << shift;
			// clang-format on

			*col0 |= half >> 8;
			*(col0++ + _ARENA_SCANLINES) |= half;
		}

	return width;
}
#endif

// EOF