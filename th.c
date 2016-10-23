/* TEST OF THREADS */

//Compile  gcc -o th2 th2.c -lpthread

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "flibspi.h"

#define  ONE_MS    		1000	   // One mseg
#define  time_delay(x)  (x/100)    // x in ms , minvalue 100mseg

#define RED_TEXT		   "\e[31;1m"
#define WHITE_TEXT 		   "\e[37m"

	
//http://linuxprograms.wordpress.com/2007/12/29/threads-programming-in-linux-examples/

unsigned int TimerTick;


void * thread1()  // Time base Thread
{
    while(1)
    {
               
		usleep(100*ONE_MS); // 100ms * 

		if (TimerTick)
			TimerTick--;
	
    }
}

void * thread2() // Periodic Task Thread

{
	int r;
	
	while (1) 
	{
		

		if (!TimerTick)			// Wait for Thread1
		{
	        r = rand()%4;
	        TimerTick=time_delay(500);       // 500ms 
	        printf("Ran %d\n",r);  // Call SPI
	      // system("aplay -q ./sounds/green.wav");	

		
		
	    
	    /*   
	       
	       SIMON_RED_SOUND; 			
		   SIMON_GREEN_SOUND; 			
           SIMON_BLUE_SOUND; 			
           SIMON_YELLOW_SOUND; 
	       SIMON_BUZZ_SOUND;
	   */    
	       
	       
	              
	    }
	        
	        	

	}

	

}




void * thread3() // The APP

{



	while (1) {
	
				printf(RED_TEXT "The" " App\n" WHITE_TEXT );
				sleep(1);
	
	}


}


int main()
{
        int status;
        pthread_t tid1,tid2,tid3;
        srand(time(NULL));

        pthread_create(&tid1,NULL,thread1,NULL);
        pthread_create(&tid2,NULL,thread2,NULL);
        pthread_create(&tid3,NULL,thread3,NULL);
        pthread_join(tid1,NULL);
        pthread_join(tid2,NULL);
        pthread_join(tid3,NULL);
        return 0;
}

