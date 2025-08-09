#ifndef __SCORELINE_H
#define __SCORELINE_H

#define DIGIT_SIZE 20 /* displayed lines */
#define SCOREVISIBLETIME                                                                           \
	200 /* # frames to show changed scoreline item before reverting to defaults                    \
	     */

#define DIGIT_SPACE 10
// #define DIGIT_LIVES     10
#define DIGIT_EXCLAMATION 11
#define DIGIT_PLUS 12
#define DIGIT_MOVES 13
#define DIGIT_Room 20
#define DIGIT_ALPHABET 14

#define ENABLE_COLOUR_SCORE 1

enum SCORE_MODE {
	SCORELINE_START,
	SCORELINE_TIME,
	SCORELINE_SCORE,
	SCORELINE_LIVES,
	SCORELINE_END,
	SCORELINE_UNDO,
	SCORELINE_STATS,
};

#define LETTER(a) ((a) - 'A' + DIGIT_ALPHABET)
#define NUMBER(a) ((a) - 74 + DIGIT_ALPHABET)

extern int scoreCycle;
extern unsigned char timeString[];
extern const int baser[];
extern const int pwr[];

extern int actualScore;
extern int guaranteedViewTime;

void drawBigDigit(int digit, int pos, int offset, int colour);
void drawLives();
void drawTheScore(int score);
void drawScore();
void addScore(int score);
void setScoreCycle(int scoreCycle);
int getScoreCycle();
void setScore(int score);
void drawTime2(int value);
void doubleSizeScore(int x, int y, int letter, int col);
char *drawDecimal2(char *buffer, unsigned char *colour_buffer,
                            unsigned int colour, int cvt);
void initScore();

#endif