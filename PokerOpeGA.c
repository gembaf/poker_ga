//====================================================================
//  前処理
//====================================================================

#include <stdio.h>

#include "Poker.h"

extern int Step[];
extern int UsedFlag[];
int StepCounter = 0;

int strategy(const int hd[], const int fd[], int cg, int tk, const int ud[], int us)
{
  UsedFlag[StepCounter] = 1;
  return Step[StepCounter++];
}

