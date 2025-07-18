#ifndef __MENU_H
#define __MENU_H

#define MUSTWATCH_STATS 0x200
#define MUSTWATCH_COPYRIGHT 50 /*0x180*/
#define MUSTWATCH_MENU 0x400

#define DETECT_FRAME_COUNT 10

void MenuOverscan();
void MenuVerticalBlank();
void SchedulerMenu();
void initKernel(int krn);
// void clearBuffer(int *buffer, int size);
void clearBuffer(int *buffer, int size);
void zeroBuffer(int *buffer, int size);

void initMenuDatastreams();

#endif
