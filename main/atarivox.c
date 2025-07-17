// clang-format off
#include <stdbool.h>
#include "defines_cdfj.h"
#include "defines_from_dasm_for_c.h"

#include "main.h"
//#include "colour.h"
// clang-format on

#if __ENABLE_ATARIVOX

static int speakWord = 0;
static int spk = 0;

void sayWord(int word) {
	speakWord = word;
	spk = 0;
}

void processSpeech() {

	static const unsigned short *const sp = (const unsigned short *)__SPEECH_ADDRESS;
	const unsigned char *const s = (const unsigned char *)(int)sp[speakWord];

	if ((RAM[_BUF_SPEECH] = s[spk]) != 0xFF)
		spk++;
}

#endif

// EOF