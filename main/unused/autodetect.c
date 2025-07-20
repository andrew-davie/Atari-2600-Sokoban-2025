#include <limits.h> // for INT_MAX

#define DETECT_FRAME_COUNT 10
#define T1TCR (*((volatile unsigned long *)0xE0008004))
#define T1TC (*((volatile unsigned long *)0xE0008008))

// console detection
#define NTSC 0
#define PAL 1
#define PAL_60 2
#define SECAM 3

unsigned int detectedPeriod;
int frame; // TODO: set to 0 on reset
// unsigned char mm_tv_type;

// frame is initialised to 0 on reset, and increments by one every screen frame
// detectConsoletype() is called every frame, but actually only 'triggers'
// detection on frame 10 This code will autodetect for SECAM, PAL, NTSC on both
// 60 MHz and 70 MHz ARM variants.

/*void detectConsoleType() {

  switch (frame) {

  case 0:

    mm_tv_type = NTSC; // force NTSC frame

    T1TC = 0;
    T1TCR = 1;
    break;

  case DETECT_FRAME_COUNT: {

    detectedPeriod = T1TC;

    static const struct fmt {

      int frequency;
      unsigned char format;

    } mapTimeToFormat[] = {

#define NTSC_70MHZ (0xB240F6 * DETECT_FRAME_COUNT / 10)
#define PAL_70MHZ (0xB3E40D * DETECT_FRAME_COUNT / 10)

        {
            NTSC_70MHZ,
            NTSC,
        },

#define SECAM_70MHZ (0xB294EA * DETECT_FRAME_COUNT / 10)
        {
            SECAM_70MHZ,
            SECAM,
        },
        {
            PAL_70MHZ,
            PAL_60,
        },

#define NTSC_60MHZ (0x98EB2F * DETECT_FRAME_COUNT / 10)
#define PAL_60MHZ (0x9A0EEF * DETECT_FRAME_COUNT / 10)
#define SECAM_60MHZ                                                            \
  (((PAL_60MHZ - NTSC_60MHZ) / 2 + NTSC_60MHZ) * DETECT_FRAME_COUNT / 10)

        {
            NTSC_60MHZ,
            NTSC,
        },
        {
            SECAM_60MHZ,
            SECAM,
        },
        {
            PAL_60MHZ,
            PAL_60,
        },
    };

    int delta = INT_MAX;
    for (unsigned int i = 0; i < sizeof(mapTimeToFormat) / sizeof(struct fmt);
         i++) {

      int dist = detectedPeriod - mapTimeToFormat[i].frequency;
      if (dist < 0)
        dist = -dist;

      if (dist < delta) {
        delta = dist;
        mm_tv_type = mapTimeToFormat[i].format; // <--- detected format!!
      }
    }

    break;
  }

  default:
    break;
  }
}*/
