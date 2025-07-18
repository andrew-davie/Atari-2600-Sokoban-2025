#ifndef __DRAWPLAYER_H
#define __DRAWPLAYER_H

void drawPlayerSprite();
void drawPlayerSmallSprite();
void drawHalfSprite();

void removeSprite();
void removeSmallSprite();
void removeHalfSprite();

void initSprites();

void getPlayerScreenPosition(int *screenX, int *screenY, int mode);

#endif
