#include "random.h"

unsigned int prng_b;
unsigned int prng_a;

unsigned int Room_random_a, Room_random_b;

unsigned int rndX;

void initRandom() {

	prng_a = 0xFACE1234;
	prng_b = 0xBABEACED;

	Room_random_a = prng_a;
	Room_random_b = prng_b;
}

unsigned int getRandom32() {
	prng_b = 36969 * (prng_b & 65535) + (prng_b >> 16);
	prng_a = 18000 * (prng_a & 65535) + (prng_a >> 16);
	return rndX = (prng_b << 16) + prng_a;
}

unsigned int rangeRandom(int range) { return ((getRandom32() >> 16) * range) >> 16; }

// EOF