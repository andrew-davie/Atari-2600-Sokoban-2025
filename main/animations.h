#ifndef __ANIMATIONS_H
#define __ANIMATIONS_H

#include "attribute.h"

extern const unsigned char *const AnimateBase[TYPE_MAX];
extern const unsigned char *Animate[TYPE_MAX];

#define ANIM_HALT 0
#define ANIM_LOOP 255
#define ANIM_RNDSPEED 254

#define ANIMMAN_STARTUP 2

extern const unsigned char *Animate[TYPE_MAX];
extern char AnimCount[TYPE_MAX];

extern void initCharAnimations();
extern void startCharAnimation(int type, const unsigned char *idx);
extern void startDefaultCharAnimation(int type);
extern void processCharAnimations();

#endif
