#ifndef __RANDOM_H
#define __RANDOM_H

extern unsigned int rndX;
extern unsigned int prng_a, prng_b;
extern unsigned int Room_random_a, Room_random_b;

void initRandom();
unsigned int getRandom32();
unsigned int rangeRandom(int range);

#endif
// EOF