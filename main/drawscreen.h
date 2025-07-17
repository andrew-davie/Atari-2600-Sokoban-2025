#ifndef __DRAWSCREEN_H
#define __DRAWSCREEN_H

void drawScreen();
void drawHalfScreen(int half);
void drawOverviewScreen(int startRow, int endRow);
void drawIconScreen(int startRow, int endRow);
void initIconPalette();
void initIconScreen();
void getShakenCoords(int *x, int *y);
void sparkly();
void doSparkles();
bool drawBit(char x, int y);

#endif
// EOF
