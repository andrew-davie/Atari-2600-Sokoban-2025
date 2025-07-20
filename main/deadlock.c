#include "deadlock.h"
#include "attribute.h"
#include "colour.h"
#include "main.h"
#include <stdbool.h>

bool deadlock;

// Check if a position is a wall
bool is_wall(unsigned char *p) { return *p == CH_BRICKWALL; }

// Check if a position is a goal
bool is_goal(unsigned char *p) {
  return Attribute[CharToType[*p]] & ATT_TARGETLIKE;
}

bool isBox(unsigned char *p) { return Attribute[CharToType[*p]] & ATT_BOX; }

bool isUnplacedBox(unsigned char *p) {
  return (isBox(p) && !(Attribute[CharToType[*p]] & ATT_TARGETLIKE));
}

bool isBoxorWall(unsigned char *p) {
  return (Attribute[CharToType[*p]] & ATT_DEADLOCK);
}

bool validate(unsigned char *offset[4]) {

  bool dead =
      ((isBox(offset[1])) && isBoxorWall(offset[2]) && isBoxorWall(offset[3]));
  if (dead) {

    // axiom: offset[0] and offset[1] ARE boxes
    for (int i = 0; i < 4; i++)
      if (isUnplacedBox(offset[i]))
        *offset[i] = CH_BOX_DEADLOCK;
  }

  return dead;
}

// Detect static corner deadlock
void is_corner_deadlock() {

  //      BOX 0       BOX 1       BOX 2       BOX 3
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  // -1  | |W| |     | |W| |     | | | |     | | | |   -_1ROW
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  //  0  |W[B] |     | [B]W|     |W[B] |     | [B]W|   0
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  // +1  | | | |     | | | |     | |W| |     | |W| |   +_1ROW
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  //
  //     -1 0 1      -1 0 1      -1 0 1      -1 0 1
  //
  // [ ] = "me"
  //  W  = wall

  bool wall_up = is_wall(me - _1ROW);
  bool wall_down = is_wall(me + _1ROW);
  bool wall_left = is_wall(me - 1);
  bool wall_right = is_wall(me + 1);

  bool dead = !is_goal(me) &&
              (
                  // Any 90° corner
                  (wall_up && wall_left) || (wall_up && wall_right) ||
                  (wall_down && wall_left) || (wall_down && wall_right));

  // Against-wall options

  //      0/4/3       1/4/5       2/3/8       BOX 3
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  // -1  | | | |     | |X|X|     | | | |     | | | |   -_1ROW
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  //  0  |B[B] |     | [B]B|     |X[B] |     | [B]X|   0
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  // +1  |X|X| |     | | | |     |X|B| |     | |B|X|   +_1ROW
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  //
  //      BOX 4       BOX 5       BOX 6       BOX 7
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  // -1  |X|X| |     | | | |     |X|B| |     | | | |   -_1ROW
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  //  0  |B[B] |     | [B]B|     |X[B] |     |X[B] |   0
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  // +1  | | | |     | |x|X|     | | | |     |X|B| |   +_1ROW
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  //     -1 0 1      -1 0 1      -1 0 1      -1 0 1

  //      BOX 8       BOX 9       BOX A       BOX B
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  // -1  | |B|X|     | | | |     | | | |     | | | |   -_1ROW
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  //  0  | [B]X|     | [B] |     | [B] |     | [B] |   0
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  // +1  | | | |     | | | |     | | | |     | | | |   +_1ROW
  //     +-+-+-+     +-+-+-+     +-+-+-+     +-+-+-+
  //     -1 0 1      -1 0 1      -1 0 1      -1 0 1

  //
  //
  // [] = "me"

  // clang-format off


  dead |=
      validate((unsigned char *[4]){me, me - 1, me + _1ROW - 1, me + _1ROW}) |   /* 0*/
      validate((unsigned char *[4]){me, me + 1, me - _1ROW, me - _1ROW + 1}) |   /* 1*/
      validate((unsigned char *[4]){me, me + _1ROW, me - 1, me + _1ROW - 1}) |   /* 2*/
      validate((unsigned char *[4]){me, me + _1ROW, me + 1, me + _1ROW + 1}) |   /* 3*/
      validate((unsigned char *[4]){me, me - 1, me - _1ROW - 1, me - _1ROW}) |   /* 4*/
      validate((unsigned char *[4]){me, me + 1, me + _1ROW, me + _1ROW + 1}) |   /* 5*/
      validate((unsigned char *[4]){me, me - _1ROW, me - 1, me - _1ROW - 1}) |   /* 6*/
      validate((unsigned char *[4]){me, me + _1ROW, me - 1, me + _1ROW - 1}) |   /* 7*/
      validate((unsigned char *[4]){me, me - _1ROW, me + 1, me - _1ROW + 1});    /* 8*/

  // clang-format on

  if (dead) {
    deadlock = true;
    if (isUnplacedBox(me))
      *me = CH_BOX_DEADLOCK;
  }
}

bool checkBlock(unsigned char *p) {

  // check a 2x2 block with TL at p. If any of the blocks are not
  // 'box-wall'-like then the block itself is not a deadlock contributor

  bool dead = true;
  for (int x = 0; x < 2; x++)
    for (int y = 0; y < 2 * _1ROW; y += _1ROW)
      dead &= isBoxorWall(p + x + y);

  return dead;
}

void isBox_block() {

  // Detect all 2x2 box cluster deadlocks around "me"
  // If *any* of the numbered squares in each group are not 'box/wall'-like
  // then that group is not deadlocked and more specifically the [] square in
  // that group is not deadlocked by that block. If aany  Otherwise, all of
  // the squares in that group which are boxlike are marked as deadlocked.

  bool dead = false;
  for (int x = -1; x < 1; x++)
    for (int y = -_1ROW; y < _1ROW; y += _1ROW)
      dead |= checkBlock(me + x + y);

  if (dead) {

    deadlock = true;
    for (int x = -1; x < 2; x += 2)
      for (int y = -1; y < 2; y += 2) {
        unsigned char *p = me + x + y * _1ROW;
        if (isBox(p) && !(Attribute[CharToType[*p]] & ATT_TARGETLIKE))
          *(p) = CH_BOX_DEADLOCK;
      }
  }
}

void checkDeadlocks() {

  if (*me == CH_BOX_DEADLOCK)
    *me = CH_BOX;

  is_corner_deadlock();
  //	isBox_block();
}
