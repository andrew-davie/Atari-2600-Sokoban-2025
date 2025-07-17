// clang-format off

#include "defines_cdfj.h"
#include "defines.h"
#include "defines_from_dasm_for_c.h"

#include "main.h"
#include "scroll.h"
#include "drawplayer.h"

#if ENABLE_SWIPE
#include "swipe.h"
#endif

#include "player.h"
#include "man.h"

// clang-format on

int scrollX[2], scrollY[2];

static int speedX[2], speedY[2];

bool isScrolling() {
	return (displayMode == DISPLAY_NORMAL && (speedX[DISPLAY_NORMAL] | speedY[DISPLAY_NORMAL]));
}

void clamp(int *value, int max) {
	if (*value < 0)
		*value = 0;
	else if (*value > max)
		*value = max;
}

void accelerate(int accelTarget, int decelTarget, int playerScreen, int center, int targetBox,
                int *speed, int increment) {

	int rSpeed = (((SPEED_FLOOR + 0) << 16) / SPEED_FLOOR) >> 8;

	accelTarget = (accelTarget * rSpeed) >> 8;
	int speedIncrement = (rSpeed * increment) >> 8;

	if ((center - targetBox) > playerScreen) {
		if (*speed > -accelTarget)
			*speed -= speedIncrement;
	}

	else if (playerScreen > (center + targetBox)) {
		if (*speed < accelTarget)
			*speed += speedIncrement;
	}

	decelTarget = (decelTarget * rSpeed) >> 8;

	if (speed) {
		if (*speed > 0) {
			*speed -= decelTarget;
			if (*speed < 0) {
				*speed = 0;
			}
		} else {

			*speed += decelTarget;
			if (*speed > 0)
				*speed = 0;
		}
	}
}

void Scroll() {

	for (int mode = DISPLAY_NORMAL; mode < DISPLAY_OVERVIEW; mode++) {

		if (manDead && !waitRelease && animationList[ANIM_PLAYER]->frame == FRAME_BLANK) {

#if ENABLE_SWIPE
			if (swipeComplete)
#endif
				for (int dir = 0; dir < 4; dir++) {
					if (!(SWCHA & (joyDirectBit[dir] << 4))) {
						scrollX[mode] += ((int)xInc[joyDirectBit[dir]])
						                 << (SHIFT_SCROLLX - 1 + mode);
						scrollY[mode] += ((int)yInc[joyDirectBit[dir]]) << (SHIFT_SCROLLY + mode);
					}
				}
		}

		else {

			int x, y;
			getPlayerScreenPosition(&x, &y, mode);
			x += manFaceDirection * frameAdjustX;

			switch (mode) {

			case DISPLAY_NORMAL:
				accelerate(0xC00, 0x80, x, 80, 25, &speedX[mode], 0xC00);
				accelerate(0x5000, 0x900, y, SCANLINES >> 1, 15, &speedY[mode], 0x5000);
				break;

			case DISPLAY_HALF:
				accelerate(0x1000, 0x100, x, 80, 35, &speedX[mode], 0x1000);
				accelerate(0x10000, 0x900, y, SCANLINES >> 1, 20, &speedY[mode], 0x2800);
				break;
			}

			scrollX[mode] += speedX[mode];
			scrollY[mode] += speedY[mode];

			if (mode == DISPLAY_HALF) {
				if (roomWidth < 14)
					scrollX[mode] = 76 << SHIFT_SCROLLX;
				if (roomHeight < 10)
					scrollY[mode] = 66 << SHIFT_SCROLLY;
			}
		}
	}

	clamp(&scrollX[DISPLAY_NORMAL], SCROLL_MAXIMUM_X_NORMAL);
	clamp(&scrollY[DISPLAY_NORMAL], SCROLL_MAXIMUM_Y_NORMAL);

	clamp(&scrollX[DISPLAY_HALF], SCROLL_MAXIMUM_X_HALF);
	clamp(&scrollY[DISPLAY_HALF], SCROLL_MAXIMUM_Y_HALF);
}

// EOF