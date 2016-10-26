/* Circular Queue */


#include <stdio.h>
#include "cqueue.h"
#define QSIZE  16	// Queue size (must be less than 0xFF)	


static unsigned char buffer[QSIZE];  // Storage for circuar queue
static unsigned char news;	         // How many "news" are remaining in Queue
static unsigned char *pin,*pout;      // Input and output queue pointers 	




/*
 Initialize queue 
*/
void QueueInit(void)

{

	pin=buffer;	//set up pin,pout and "news" counter 	
	pout=buffer;
	news=0;

}

/*
  Push data on queue 
*/
unsigned char PushQueue(unsigned char data)

{
	


	if (news > QSIZE-1)		//test for Queue overflow
	{
		news=QOVERFLOW;		// inform queue has overflowed
		return (news);		
	}	

	*pin++=data;			// pull data
	news++;				// update "news" counter

	if (pin == buffer+QSIZE)	// if queue size is exceded reset pointer
		pin=buffer;

    

	return(news);			// inform Queue state
}

/*
  Retrieve data from queue 
*/
unsigned char PullQueue(void)

{
unsigned char data;

	


	data=*pout++;		// pull data
	news--;				// update "news" counter
		

	if (pout == buffer+QSIZE)	// Check for Queue boundaries
		pout=buffer;		// if queue size is exceded reset pointer
	return (data);			// rerturn retrieved data
}

/*
  Get queue Status
*/
unsigned char QueueStatus(void)

{
	
//----- For Debug --------------  	

/*
    int i;
    
	printf("Queue values: ");
	for(i=0;i<QSIZE;i++)
	printf("[%.2X] ",buffer[i]);
	printf("\n");
*/

//--------------------------------
	
	return (news);			// Retrieve "news" counter		
}



