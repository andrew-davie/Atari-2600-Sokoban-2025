#ifndef __SWIPE_C
#define __SWIPE_C

enum CIRCLEPHASE {
	SWIPE_SEARCH,
	SWIPE_REGROW,
	SWIPE_RESHRINK,

	SWIPE_GROW,
	SWIPE_SHRINK,
};

enum SWIPE {
	SWIPE_CIRCLE,
	SWIPE_STAR,
};

void swipe(int reserved);
void setSwipe(int x, int y, int radius, int step, enum CIRCLEPHASE phase);
void applySwipeMask();

void initStarSwipe();
void clearMask(int v);

int abs(int value);
extern bool swipeComplete;

extern enum SWIPE swipeType;
extern enum CIRCLEPHASE swipePhase;

extern unsigned char swipeMask[6][_ICC_SCANLINES];

#endif
