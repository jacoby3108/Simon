#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include "cqueue.h"    // circular queue services
#include "flibspi.h"
#include <pthread.h>

#define GAME_SIZE    8 // 200

//#define TEXTIFY(A) #A
//#define GPIO(pin)  TEXTIFY(/sys/class/gpio/gpio ## pin )


//*****************************************************************************************

/* TEST OF THREADS */

//Compile

//gcc simon.c flibspi.c cqueue.c -pthread -o simon

//scp simon.c   pi@192.168.1.106:/home/pi/Simon
//scp -r sounds/  pi@192.168.1.106:/home/pi/Simon



#define ONE_MS    		1000	   // One mseg
#define time_delay(x)  (x/100)    // x in ms , minvalue 100mseg


//http://linuxprograms.wordpress.com/2007/12/29/threads-programming-in-linux-examples/

unsigned int TimerTick;
void play (void);
void show_blue(void);
void show_green(void);
void show_red(void);
void show_yellow(void);
void show_buzz(void);
void wait_for_other_game(void);
void Flush_Key_Queue(void);
void you_won(void);
int maintest(void);


// ===== Timer service ===========================

#define TIMEOUT 1
void Timer_ISR(void);
void Set_Timer(unsigned int delay); // en segundos 
unsigned char Get_Timer_Status(void); 		//  
static unsigned int timer_delay=0;

// =============SOUND Service=====================
#define NO_SOUND		0
#define BLUE_SOUND		1
#define GREEN_SOUND		2
#define RED_SOUND		3
#define YELLOW_SOUND	4
#define BUZZ_SOUND		5
int play_sound = NO_SOUND;




// ===============================================
#define OK 0
#define USER_WRONG_ANSWER 			-1
#define USER_ANSWER_OK    			-2 
#define USER_ANSWER_IN_PROGRESS    	-3 

unsigned char Get_Next_Key(void);
void show_next_key_to_user(int next_key);
int Check_User_Keys(int);

unsigned int i,j,k;

                              
unsigned char keyvalues[]={BLUE_DOWN,GREEN_DOWN,RED_DOWN,YELLOW_DOWN};
const unsigned int Number_Of_Keys=4;

// ===============================================

unsigned char memory[GAME_SIZE];



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
	
	while (1) 
	{
		

		if (!TimerTick)			// Wait for Thread1
		{
	      //i--;  
	        TimerTick=time_delay(200);      // 200ms 
	       // printf("Done %d\n",TimerTick);  //
	        scan_keys();	
	        Timer_ISR();
	              
	    }
	        
	        	

	}

	

}




void * thread3() // The APP

{

	while (1) {

		//	maintest();
			play();
			
	
	}


}

void * thread4()  // Sound Thread
{
    while(1)
    {
        switch(play_sound)						// Show the selected  key to user
		{
		
			case NO_SOUND:
			break;
				
			case BLUE_SOUND:
			Set_Pin(PIN_BLUE_LED);
			SIMON_BLUE_SOUND;
			Clr_Pin(PIN_BLUE_LED);
			play_sound=NO_SOUND;
			break;
		
			case GREEN_SOUND:
			Set_Pin(PIN_GREEN_LED);
			SIMON_GREEN_SOUND;
			Clr_Pin(PIN_GREEN_LED);
			play_sound=NO_SOUND;
			break;
		
			case RED_SOUND:
			Set_Pin(PIN_RED_LED);
			SIMON_RED_SOUND;
			Clr_Pin(PIN_RED_LED);
			play_sound=NO_SOUND;
			break;
		
			case YELLOW_SOUND:
			Set_Pin(PIN_YELLOW_LED);
			SIMON_YELLOW_SOUND;
			Clr_Pin(PIN_YELLOW_LED);
			play_sound=NO_SOUND;
			break;
			
			
			case BUZZ_SOUND:
			
			Set_Pin(PIN_BLUE_LED);
			Set_Pin(PIN_GREEN_LED);
			Set_Pin(PIN_RED_LED);
			Set_Pin(PIN_YELLOW_LED);
			
			SIMON_BUZZ_SOUND;

			Clr_Pin(PIN_BLUE_LED);
			Clr_Pin(PIN_GREEN_LED);
			Clr_Pin(PIN_RED_LED);
			Clr_Pin(PIN_YELLOW_LED);
		

			play_sound=NO_SOUND;
			break;
							
		
			default:
			printf("Unknkown Sound");
			break;
		}
    }
}
//*****************************************************************************************

int maintest(void)  // this function is to see how use the services 
{

	unsigned char key,i ;
	char *colors[]={BLUE_TEXT,GREEN_TEXT,RED_TEXT,YELLOW_TEXT};
	unsigned char led_colors[]={PIN_BLUE_LED,PIN_GREEN_LED,PIN_RED_LED,PIN_YELLOW_LED};
	
	QueueInit();	// initialize queue
	InitHard();
	
 while(1)
 {
	
	
	printf(YELLOW_TEXT "Simon Services Test\n" WHITE_TEXT );

	printf("Try to press a key while test is running: \n\n");
	
	
		if(QueueStatus())	// How to use keyboard 
		{
			
			key=PullQueue();
			printf("Key (HEX Code): %.2X \n",key);
					
			if(key==BLUE_DOWN)
			printf(BLUE_TEXT "BLUE DOWN \n" WHITE_TEXT);
			
			if(key==BLUE_UP)
			printf(BLUE_TEXT "BLUE UP \n" WHITE_TEXT);
		
						
			if(key==GREEN_DOWN)
			printf(GREEN_TEXT "GREEN DOWN \n" WHITE_TEXT);
			
			if(key==GREEN_UP)
			printf(GREEN_TEXT "GREEN UP \n" WHITE_TEXT);
			
			if(key==RED_DOWN)
			printf(RED_TEXT "RED DOWN \n" WHITE_TEXT);
			
			if(key==RED_UP)
			printf(RED_TEXT "RED UP \n" WHITE_TEXT);
			
			if(key==YELLOW_DOWN)
			printf(YELLOW_TEXT "YELLOW DOWN \n" WHITE_TEXT);
			
			if(key==YELLOW_UP)
			printf(YELLOW_TEXT "YELLOW UP \n" WHITE_TEXT);
		}
		
 

	//how play a sound
	play_sound=YELLOW_SOUND; // play sound and led
	//while(play_sound==YELLOW_SOUND);

	for (i=0;i<sizeof(colors)/sizeof(colors[0]);i++)
	{  
		//Set_Pin(led_colors[i]);
		printf("%s\n",colors[i] );
		printf("Counting in color \n");
		Set_Timer(1); // 1 seg					// How to use the timer
		while(Get_Timer_Status()!=TIMEOUT);
		//Clr_Pin(led_colors[i]);
			
	}
	
 }


	return 0;
}

int main(void)
{
        pthread_t tid1,tid2,tid3,tid4;

		
		
		clrscr();		// Clear Screen
		gotoxy(20,3);
		printf(GREEN_TEXT "******* Welcome to Simon Game ********* \n" WHITE_TEXT);
		
		gotoxy(20,5);
		printf(GREEN_TEXT "Press Green Key to start or Blue Key to exit \n" WHITE_TEXT);
	
		QueueInit();	// initialize keyboard queue
		InitHard();		// Initialize Hardware Driver 


        pthread_create(&tid1,NULL,thread1,NULL);
        pthread_create(&tid2,NULL,thread2,NULL);
        pthread_create(&tid3,NULL,thread3,NULL);
        pthread_create(&tid4,NULL,thread4,NULL);
        pthread_join(tid1,NULL);
        pthread_join(tid2,NULL);
        pthread_join(tid3,NULL);
        pthread_join(tid4,NULL);
        
        
        return 0;
}

void play (void)
{

	int actual_key=0;
	int game_status=OK;
	int game_round;
	int ran_key;
	
	game_round=0;
	Flush_Key_Queue();
	wait_for_other_game();
	Flush_Key_Queue();
	
	printf("Start game .........:)/n");
	
	for (i=0;i<GAME_SIZE;i++)				// Create random keys for the game  
	{
		
		ran_key=rand()%Number_Of_Keys;
		printf("rankey:%d ",ran_key);
		memory[i]=keyvalues[ran_key];
	}
	printf("\n");
/*	
	memory[0]=keyvalues[0];
	memory[1]=keyvalues[1];
	memory[2]=keyvalues[2];
	memory[3]=keyvalues[3];
	memory[4]=keyvalues[0];
	memory[5]=keyvalues[1];
	memory[6]=keyvalues[2];
	memory[7]=keyvalues[3];
	
*/	
	
	game_round=0;
	
	
	while (game_round < GAME_SIZE )
	{
	
		sleep(1);
		for(i=0;i<=game_round;i++)
		show_next_key_to_user(memory[i]);			// Show it to user 
			
		
	
		game_status=Check_User_Keys(game_round);  	// Ckeck user pressed sequence for this round
		
		if(game_status==OK)							// Actual round was ok
		{
		
			printf("Next round \n");    			// Go to next round
			game_round++;	
		
		}
		else 				// User failed to follow the key sequence or Timeout
		{
			game_round=0;	 
			show_buzz();
			break;
		}	
		
		
	} // end of while	
	
	if(game_round == GAME_SIZE)
			you_won();
	
	
		
}

void you_won(void)
{
	int i;
	printf("you won !!! \n");
	
	for (i=0;i<6;i++)
	{
		Set_Pin(PIN_BLUE_LED);
	  	Set_Pin(PIN_GREEN_LED);
		usleep(300*ONE_MS); 	// 300ms *
		Clr_Pin(PIN_GREEN_LED);
		Clr_Pin(PIN_BLUE_LED);
		
		Set_Pin(PIN_YELLOW_LED);
		Set_Pin(PIN_RED_LED);
		usleep(300*ONE_MS); 	// 300ms *
		Clr_Pin(PIN_YELLOW_LED);
		Clr_Pin(PIN_RED_LED);
	}	
		
}


int Check_User_Keys(int game_round)
{
	
	int next_key;
	
	
		for(i=0;i<=game_round;i++)	// check user keys against memory
		{	
			Set_Timer(5); // 5 seg  (timeout)
			
			next_key=NO_KEY;
					
			while(Get_Timer_Status()!=TIMEOUT && next_key==NO_KEY) 
			{
			
				next_key=Get_Next_Key();
			
			}
			
			if (Get_Timer_Status()==TIMEOUT)			// Lazy User 
			{	
				printf("Game over Timeout \n");
					
				return(TIMEOUT);	
			}	
			
			if(memory[i]!=next_key) 					// Check user answer
			{
				printf("User wrong answer \n");
				
				return(USER_WRONG_ANSWER);
			}
			else
			{
					
				printf("Good Job \n");					// Good Job !!
										
			}
		

		} // end of for
		
		
	printf("Gooooood Round!!\n");	// succesfull round
	return(OK);
}



unsigned char Get_Next_Key(void)
{
unsigned char key=NO_KEY;

	if(QueueStatus())		// some news?
	{
			key=PullQueue();
			
			if(key==QOVERFLOW)
				printf("Key Buffer Overfloed");
					
			printf("Key Pulled from CQueue: %.2X\n",key);
		
			if(key==BLUE_DOWN)
			{
				show_blue();
				return(key);
		    }
			
			if(key==GREEN_DOWN)
			{
				show_green();
				return(key);
			}
						
			if(key==RED_DOWN)
			{
				show_red();
				return(key);
		    }
			
			if(key==YELLOW_DOWN)
			{
				show_yellow();
				return(key);
		    }
		
		 		
	}
	
	return (NO_KEY);				// No news 
}



// ============= Timer service ============00


void Timer_ISR(void)
{
	
		if(timer_delay)				// if counter <> 0 pump it down 
			timer_delay--; 
	
}

void Set_Timer(unsigned int delay) // en segundos 
{
	timer_delay=delay*5;			// Asumo 200ms en thread1 => 5*200ms=1seg
	
}
 
unsigned char Get_Timer_Status(void) 		//  
{
	return(!timer_delay);			// =1 timeout
	
}
// ===============END Timer Services==========================



 
void show_next_key_to_user(int next_key)
{		
	
	printf(" >>>>>>>>>Show next>>>>>>>>> \n");
	
		switch(next_key)						// Show the selected  key to user
		{
		
			case BLUE_DOWN:
			show_blue();
			break;
		
			case GREEN_DOWN:
			show_green();
			break;
		
			case RED_DOWN:
			show_red();
			break;
		
			case YELLOW_DOWN:
			show_yellow();
			break;
		
			default:
			printf("Unknkown key");
			break;
			
			
			
		}

}

//// ========= Show Status functions =========== ////

void show_blue(void)
{
	
	printf(BLUE_TEXT "BLUE DOWN \n" WHITE_TEXT);
	play_sound=BLUE_SOUND;
	while(play_sound==BLUE_SOUND);
	
	
}
void show_green(void)
{
	printf(GREEN_TEXT "GREEN DOWN \n" WHITE_TEXT);
	play_sound=GREEN_SOUND;
	while(play_sound==GREEN_SOUND);
	

}

void show_red(void)
{
	printf(RED_TEXT "RED DOWN \n" WHITE_TEXT);
	play_sound=RED_SOUND;
	while(play_sound==RED_SOUND);
	
}

void show_yellow(void)
{
	printf(YELLOW_TEXT "YELLOW DOWN \n" WHITE_TEXT);
	play_sound=YELLOW_SOUND;
	while(play_sound==YELLOW_SOUND);
}


void show_buzz(void)
{
	printf(CYAN_TEXT "BUZZ !!!! \n" WHITE_TEXT);
	play_sound=BUZZ_SOUND;
	while(play_sound==BUZZ_SOUND);
}


void wait_for_other_game(void)
{

int key;

	//printf("Press the Green button to start....\n");  
	
	while(!QueueStatus())		// some news?
	{
	

		Set_Pin(PIN_BLUE_LED);
		usleep(300*ONE_MS); 	// 300ms *
		Clr_Pin(PIN_BLUE_LED);
		
		Set_Pin(PIN_RED_LED);
		usleep(300*ONE_MS); 	// 300ms *
		Clr_Pin(PIN_RED_LED);
		
		Set_Pin(PIN_GREEN_LED);
		usleep(300*ONE_MS); 	// 300ms *
		Clr_Pin(PIN_GREEN_LED);
				
		Set_Pin(PIN_YELLOW_LED);
		usleep(300*ONE_MS); 	// 300ms *
		Clr_Pin(PIN_YELLOW_LED);
	}
	
	key=PullQueue();
	
	if(key==BLUE_DOWN)
	{
		printf("Bye Bye Simon ....\n");
		exit(0);
	}
	
	if(key==GREEN_DOWN)
	{
		printf("Starting ......\n");	
		sleep(1);
	}
		
}

void Flush_Key_Queue(void)
{

int key;

	while(QueueStatus())		// some news?
	{
		key=PullQueue();
	}
}


