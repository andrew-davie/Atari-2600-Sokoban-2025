// clang-format off

#include "defines_cdfj.h"
#include "defines.h"

#include "main.h"

#include "joystick.h"
#include "player.h"
#include "score.h"

// clang-format on

unsigned char inpt4;
unsigned char swcha;

void getJoystick() {
	swcha = SWCHA;
	inpt4 = INPT4;
}

// EOF
