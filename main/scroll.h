#ifndef __SCROLL_H
#define __SCROLL_H

extern int scrollX[2];
extern int scrollY[2];

void Scroll();
bool isScrolling();
void clamp(int *value, int max);

#define SHIFT_SCROLLX 16
#define SHIFT_SCROLLY 16

#define SCROLL_MAXIMUM_X_NORMAL (0xAC << SHIFT_SCROLLX)
#define SCROLL_MAXIMUM_X_HALF (180 << SHIFT_SCROLLX)

#define SCROLL_MAXIMUM_Y_NORMAL (0x8A << SHIFT_SCROLLY)
#define SCROLL_MAXIMUM_Y_HALF (0x90 << SHIFT_SCROLLY)

#endif
// EOF
