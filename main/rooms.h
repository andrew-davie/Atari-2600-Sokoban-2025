#include <stdbool.h>

extern const char *room[];
extern int pillCount;

typedef struct {
	int x;
	int y;
	int width;
	int height;
} BOUNDARY;

extern BOUNDARY boundary;

void unpackRoom(BOUNDARY *boundary, bool dryRun, int room);
int getRandomRoomNumber();
int getRoomCount();

// EOF