#ifndef __MAN_H
#define __MAN_H

void moveMan(/*unsigned char *me*/);
void initMan();

extern int frameAdjustX;
extern int frameAdjustY;

extern enum FaceDirection manFaceDirection;
extern int manX, manY;

extern bool manDead;

extern const signed char xInc[];
extern const signed char yInc[];

#endif
// EOF