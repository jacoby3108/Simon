#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <ctype.h>
#include <time.h>
#include <unistd.h>

#include "libterm.h"
//*****************Select Hardware Type: PI_Board or None (Simulaton)*********************************
#define NONE		0
#define PI_BOARD	1

#define HARDWARE 	NONE
//*****************************************************************************************

int main(void)
{
	
	#if HARDWARE == PI_BOARD
	initspi();
	#endif
	
	
	return 0;
}
