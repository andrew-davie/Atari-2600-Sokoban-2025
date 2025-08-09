#ifndef __DRAWSCREEN_H
#define __DRAWSCREEN_H

extern unsigned char rollColours[3];

void drawScreen();
void drawHalfScreen(int half);
void drawOverviewScreen(int startRow, int endRow);
void getShakenCoords(int *x, int *y);
void sparkly();
void doSparkles();
bool drawBit(char x, int y, int colour);
void drawIconScreen(int startRow, int endRow);
void initIconPalette();

extern int startup;

#endif
// EOF
