#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "cqueue.h"    // circular queue services
#include "flibspi.h"



//*****************************************************************************************

int main(void)
{

	unsigned char key ;
	QueueInit();	// initialize queue
	
	InitHard();
	
	Set_Pin(PIN_BLUE_LED);
	Set_Pin(PIN_GREEN_LED);
	Clr_Pin(PIN_RED_LED);
	Clr_Pin(PIN_YELLOW_LED);

	
	return 0;
}
