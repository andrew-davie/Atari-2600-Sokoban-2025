#include <stdbool.h>

extern unsigned char *room[];
extern int pillCount;

void unpackRoom(int *x, int *y, bool dryRun, int room);
int getRandomRoomNumber();
int getRoomCount();

// EOF