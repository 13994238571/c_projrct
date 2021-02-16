#include "diceroll.h"
#include <stdio.h>
#include <stdlib.h>

int roll_count = 0;

static int rollem(int sides)
{
    int roll;
    roll = rand() % side + 1;
    ++ roll_count;
    return roll;


}

