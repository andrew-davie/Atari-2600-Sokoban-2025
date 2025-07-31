#include <stdbool.h>

struct roomDef {

	int id; // from https://sokoban-solver.com/ or -1 if unknown
	char width;
	char height;
	int pushes;
	int moves;
	const char *design;
};

extern const struct roomDef room[];
extern int pillCount;

void unpackRoom(int room);
int getRandomRoomNumber();
int getRoomCount();

// EOF