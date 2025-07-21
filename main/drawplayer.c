// clang-format off

#include <stdbool.h>

#include "animations.h"
#include "attribute.h"
#include "colour.h"
#include "defines.h"
#include "defines_cdfj.h"
#include "drawplayer.h"

#include "drawscreen.h"
#include "main.h"

#include "player.h"
#include "man.h"
#include "score.h"
#include "scroll.h"

// clang-format on

static int playerSpriteY;
static int playerHalfSpriteY;
static int playerSmallSpriteY;

int lastSpriteY;

extern const unsigned char playerColour[];
unsigned char convertedPlayerColour[9];

void initSprites() {

	lastSpriteY = 0;

	for (int i = 0; i < 9; i++)
		convertedPlayerColour[i] = convertColour(playerColour[i]);

	//    playerSpriteY =
	/* playerHalfSpriteY =
	 */
	//      playerSmallSpriteY - 1;
}

bool manObscured() {
	return (animationList[ANIM_PLAYER]->frame == FRAME_BLANK);
	//  ||
	//         Animate[TYPE_MAN] - AnimateBase[TYPE_MAN] >= 5);
}

void clipPosition() {
	if (P0_X < 4)
		P0_X = 4;
	else if (P0_X > 146)
		P0_X = 146;
}

void drawPlayerSprite() { // --> 1976 cycles

#if ENABLE_SHAKE

	int x, y;
	getShakenCoords(&x, &y);

	x >>= SHIFT_SCROLLX;
	y >>= SHIFT_SCROLLY;
#else
	int x = scrollX[displayMode] >> SHIFT_SCROLLX;
	int y = scrollY[displayMode] >> SHIFT_SCROLLY;

#endif

	// removeSprite();

	// if (manDeadRelease)
	//     return;

	P0_X = 20;
	P1_X = 20;

	// if (manDead || manObscured())
	//     return;

	if (manObscured())
		return;

	int ypos = (manY + 1) * CHAR_HEIGHT - y * 3 - frameAdjustY - 1;
	int xpos = manX * PIXELS_PER_CHAR - x;

	if ((!manDead && (frameAdjustY || frameAdjustX)) ||
	    (/*(type == TYPE_MAN)
&&*/ xpos >= 0 && xpos < 39 && ypos >= 0 && ypos < _ARENA_SCANLINES - CHAR_HEIGHT)) {

		const unsigned char *spr =
		    (const unsigned char *)playerBigSprite[animationList[ANIM_PLAYER]->frame];

		int frameOffset = *(const signed char *)spr++;
		int frameYOffset = *(const signed char *)spr++;

		playerSpriteY = ypos + frameYOffset - 2;

		int pX = ((manX * PIXELS_PER_CHAR) - x) * 4 +
		         (manFaceDirection * (frameOffset + frameAdjustX)) + 2;

		if (playerSpriteY < 0 || playerSpriteY >= _ARENA_SCANLINES - SPRITE_DEPTH || pX > 159)
			return;

		P0_X = pX;
		// clipPosition();

		if (manFaceDirection == FACE_LEFT) {
			P1_X = P0_X;
			P0_X += 8;
		} else
			P1_X = P0_X + 8;

		lastSpriteY = playerSpriteY;

		unsigned char *p0Colour = RAM + _BUF_COLUP0 + playerSpriteY;
		unsigned char *p1Colour = p0Colour + _ARENA_SCANLINES;

		unsigned char *p0 = RAM + _BUF_GRP0A + playerSpriteY;
		unsigned char *p1 = p0 + _ARENA_SCANLINES;

		for (int line = 0; line < SPRITE_DEPTH; line++) {

			if (manFaceDirection == FACE_RIGHT) {
				p0[line] = spr[0];
				p1[line] = spr[1];
			}

			else {
				p0[line] = BitRev[spr[0]];
				p1[line] = BitRev[spr[1]];
			}

			p0Colour[line] = convertedPlayerColour[spr[2] >> 4];
			p1Colour[line] = convertedPlayerColour[spr[2] & 0xF];

			spr += 3;
		}
	}
}

void getPlayerScreenPosition(int x, int y, int *screenX, int *screenY, int mode) {

	switch (mode) {
	case DISPLAY_NORMAL:
		*screenY = SCORE_SCANLINES + (y)*CHAR_HEIGHT - (scrollY[mode] >> SHIFT_SCROLLY) * 3 - 3;
		*screenX = (x * PIXELS_PER_CHAR - (scrollX[mode] >> SHIFT_SCROLLX)) * 4;
		break;

	case DISPLAY_HALF:
		*screenX =
		    ((x * PIXELS_PER_CHAR_HALF - (scrollX[DISPLAY_HALF] >> (SHIFT_SCROLLX + 1))) * 4);

		*screenY = SCORE_SCANLINES + y * CHAR_HEIGHT_HALF -
		           ((scrollY[DISPLAY_HALF] * 3) >> (SHIFT_SCROLLY + 1)) + 3;

		break;
	}
}

void drawHalfSprite() { // --> 2273 cycles

	P0_X = 20;
	P1_X = 20;

	if (manObscured())
		return;
#if CIRCLE
	if (!checkSwipeFinished())
		return;
#endif

	const signed char *spr = playerBigSprite[animationList[ANIM_PLAYER]->frame];
	int frameXOffset = *spr++;
	int frameYOffset = *spr++;

	playerHalfSpriteY = SCORE_SCANLINES + manY * CHAR_HEIGHT_HALF -
	                    ((scrollY[DISPLAY_HALF] * 3) >> (SHIFT_SCROLLY + 1)) +
	                    ((frameYOffset * 3) >> 2) - (((frameAdjustY * (0x600 / 9)) >> 8)) + 3;

	int x = ((manX * PIXELS_PER_CHAR_HALF - (scrollX[DISPLAY_HALF] >> (SHIFT_SCROLLX + 1))) * 4);
	x += manFaceDirection *
	     (((frameXOffset + frameAdjustX) * ((PIXELS_PER_CHAR_HALF << 8) / PIXELS_PER_CHAR)) >> 8);
	x -= 2;

	// if (manFaceDirection < 0)
	// 	x -= 2;

	P0_X = x; // + (x >> 1);
	P1_X = 20;

	if (playerHalfSpriteY < 0 || playerHalfSpriteY > _ARENA_SCANLINES - 15)
		return;

	lastSpriteY = playerHalfSpriteY;

	unsigned char *p0Colour = RAM + _BUF_COLUP0 + playerHalfSpriteY;
	unsigned char *p0 = RAM + _BUF_GRP0A + playerHalfSpriteY;
	unsigned int newSp;

	for (int line = 0; line < CHAR_HEIGHT_HALF - 3; line++) {
		const unsigned char *spr =
		    (const unsigned char *)playerBigSprite[animationList[ANIM_PLAYER]->frame] +
		    (((line * (0x1B00 / CHAR_HEIGHT_HALF) >> 8) * 3) + 5);

		int combined = (spr[0] << 8) | spr[1];

		// squash to half-width
		newSp = 0;
		for (int i = 0; i < 8; i++)
			newSp |= (combined & (0b11 << (i * 2))) ? (1 << i) : 0;

		p0[line] = (manFaceDirection == FACE_RIGHT) ? newSp : BitRev[newSp];
		p0Colour[line] = convertedPlayerColour[(spr[2] & 0xF)];
	}
}

void drawPlayerSmallSprite() { // --> 1149 cycles

	if (manObscured()) // || (frame & 16))
		return;

	const signed char *spr = playerBigSprite[animationList[ANIM_PLAYER]->frame];
	int frameOffset = *spr++;
	int frameYOffset = *spr++ >> 1;

	playerSmallSpriteY = manY * 9 + frameYOffset - ((frameAdjustY * (0x300 / 9) >> 8));

	if (playerSmallSpriteY < 0 || playerSmallSpriteY >= _ARENA_SCANLINES - 10) // SPRITE_DEPTH / 2)
		return;

	P0_X = (manX << 2) +
	       (manFaceDirection * (((frameOffset + frameAdjustX) * (0x100 / 5)) >> 8) + 1) - 3;
	P1_X = 20;

	if (P0_X < 15) {
		playerSmallSpriteY--;
	}

	lastSpriteY = playerSmallSpriteY;

	unsigned char *p0Colour = RAM + _BUF_COLUP0 + playerSmallSpriteY;
	unsigned char *p0 = RAM + _BUF_GRP0A + playerSmallSpriteY;

	int frac = 0;

	unsigned int newSp;
	unsigned int step = (SPRITE_DEPTH << 8) / 9;

	for (int line = 0; line < 9; line++) { // SPRITE_DEPTH / 2; line++) {

		const unsigned char *spr =
		    (const unsigned char *)(playerBigSprite[animationList[ANIM_PLAYER]->frame] + 8) +
		    (frac >> 8) * 3;
		int combined = (spr[0] << 8) | spr[1];
		combined |= (combined >> 1);

		// clang-format off
            newSp = (combined & 1) |
                    ((combined & (1 << 3)) >> 2) |
                    ((combined & (1 << 6)) >> 4) |
                    ((combined & (1 << 9)) >> 6) |
                    ((combined & (1 << 12)) >> 8) |
                    ((combined & (1 << 15)) >> 10);
		// clang-format on

		p0[line] = (manFaceDirection == FACE_RIGHT) ? newSp : BitRev[newSp];
		p0Colour[line] = convertedPlayerColour[spr[2] >> 4];

		frac += step;
	}
}

// EOF