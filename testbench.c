#include <stdio.h>
#include "cqueue.h"


void main (void)	// Test bench
{
unsigned char data;
unsigned char status;

	QueueInit();	// initialize queue
	PushQueue(1);	// Push something into the queue
	PushQueue(2);
	PushQueue(3);
	
	if(PushQueue(4)==QOVERFLOW)
	{
		printf( "Queue Overflowed \n");
		return;
	}

/*
	if(PushQueue(5)==QOVERFLOW)
	{
		printf( "Queue Overflowed \n");
		return;
	}

*/	

	while (QueueStatus())		// some news?
	{
		data=PullQueue(); 	// if yes then get data from queue
		printf( "Data pulled: %d , Remaining: %d   \n",data,QueueStatus());
	}

}


