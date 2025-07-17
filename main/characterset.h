#ifndef __PIECES_H
#define __PIECES_H

typedef struct {
	const unsigned char *big;
	const unsigned char *small;
} CSET;

#if __ENABLE_PARALLAX
void createParallaxCharset();
#endif
void createDripCharacter();

#if ENABLE_PLAYERCELL
void copyPlayerCell(int c);
void modifyPlayerCell();
#endif

extern const CSET charSet[];

#endif
