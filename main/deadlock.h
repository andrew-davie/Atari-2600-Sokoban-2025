#ifndef __DEADLOCK_H
#define __DEADLOCK_H

#include <stdbool.h>

extern int deadlock;
extern int lastDeadlock;

extern int deadlockCounter;

void checkDeadlocks();

#endif
// EOF
