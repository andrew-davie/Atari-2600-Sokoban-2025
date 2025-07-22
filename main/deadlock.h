#ifndef __DEADLOCK_H
#define __DEADLOCK_H

#include <stdbool.h>

extern bool deadlock;
extern int deadlockCounter;

void checkDeadlocks();

#endif
// EOF
