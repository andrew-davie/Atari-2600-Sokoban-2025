#ifndef __COLOUR_H
#define __COLOUR_H

void doFlash();
void setPalette();
void setBackgroundPalette(const unsigned char *c);
void initColours();
void setFlash2(unsigned char colour, int time);

void interleaveColour();
int convertColour(unsigned char colour);
void chooseColourScheme();

extern int flashTime;

#define FLASH(a, b) setFlash2(a, b);

extern unsigned char bgPalette[22];
extern unsigned char fgPalette[2];
extern int roller;

#endif
// EOF