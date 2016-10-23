#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cqueue.h"    // circular queue services
#include "flibspi.h"

// Simon v1.00 by Dany (User space version) 20-Oct-2016
// Note this version uses fopen high level function bear in mind that it is BUFFERED

// Compile command line
// gcc ...

//http://www.auctoris.co.uk/2012/08/23/gpio-with-sysfs-on-raspberry-pi-part-2/

//****************Select Debug  on/off *****************************************************
#define ON   1
#define OFF  0

// ---------------------------------
#define DEBUG  OFF    
// ---------------------------------

#if DEBUG == ON
# define DEBUG_PRINT(x) fprintf x
#elif DEBUG == OFF
# define DEBUG_PRINT(x) do {} while (0)
#endif

// Structures

typedef struct 
		{
			const unsigned char * name; 
		    const unsigned int  numpin;
		    const unsigned char * txtpin;
		    const unsigned char * direction;
			FILE * handle;
			
		} PIN;
			
			
PIN pins[] ={{"BLUE_LED",18,"18","out",0},
			 {"GREEN_LED",25,"25","out",0},
			 {"RED_LED",23,"23","out",0},
			 {"YELLOW_LED",24,"24","out",0},
			 {"BLUE_KEY",27,"27","in",0},
			 {"GREEN_KEY",17,"17","in",0},
			 {"RED_KEY",4,"4","in",0},
			 {"YELLOW_KEY",22,"22","in",0}};


// Pin file system

#define EXPORT "/sys/class/gpio/export"      // Export file 
 


// CLI Usermode examples (Keep this for reference)

//echo "17" > /sys/class/gpio/export
//echo "17" > /sys/class/gpio/unexport
//echo "out" > /sys/class/gpio/gpio17/direction  (NOTE "out" must be in lower case)
//cat /sys/class/gpio/gpio9/value
//echo "1" > /sys/class/gpio/gpio22/value


void Export_Pins(void);
void Set_PIN_Direction(void);
void Set_Port_handles(void);


FILE *handle_export;
int nWritten,nread;  // bytes read or written during file operations
int i,j,k;

int maintest1(void)     // Test Bench
{
	
	unsigned char key ;
	QueueInit();	// initialize queue
	
	InitHard();
	
	Set_Pin(PIN_BLUE_LED);
	Set_Pin(PIN_GREEN_LED);
	Set_Pin(PIN_RED_LED);
	Set_Pin(PIN_YELLOW_LED);
	
/*	
	k=Read_Pin(PIN_BLUE_KEY);
	printf("PIN_BLUE_LED is: %d\n\n",k);
	
	k=Read_Pin(PIN_GREEN_KEY);
	printf("PIN_GREEN_LED is: %d\n\n",k);
	
	k=Read_Pin(PIN_RED_KEY);
	printf("PIN_RED_LED is: %d\n\n",k);
	
	k=Read_Pin(PIN_YELLOW_KEY);
	printf("PIN_YELLOW_LED is: %d\n\n",k);
 
*/

	while(1)
	{
		usleep(200*1000); // 100ms * 
		scan_keys();
	
		if(QueueStatus())		// some news?
		{
			
			key=PullQueue();
			printf("Key: %.2X \n",key);
			
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
	
	}
	
	
	
	return 0;
}

void InitHard(void)
{
	Export_Pins();					// Export Used pins to User space for debug
	Set_PIN_Direction();			// Set Pin I/O 
	Set_Port_handles();				// Anotate pin handles for I/O operations
}

// =========================
// Read Pin 
// Reads input pin
//==========================
#define MAX_VAL 2

int Read_Pin(int pin)
{
////////	
char device[50]; 
const char *gpio_str="/sys/class/gpio/gpio";
const char *gpio_val_str="/value";
const char *W_mode="w";
const char *R_mode="r";
	
const char *mode=NULL;
FILE *handle;
/////////////	
char value[MAX_VAL];	

	

	if(fgets(value,MAX_VAL,pins[pin].handle)==NULL) // 
	{
		DEBUG_PRINT((stdout, "Read_Pin: Cannot read file %s (handle=%X). Try again later.\n",pins[pin].txtpin,pins[pin].handle));
		exit(1);
	}
	else
		DEBUG_PRINT((stdout, "Pin Value from file %s (handle=%X) is= %s\n",pins[pin].txtpin,pins[pin].handle,value));
	
	/// there is a problem reading continuosly from this file 
	/// unfortunately fseek or rewind doesn't work. I must close / open every time 
	/// Pseudofiles like those in /sys may not work with those functions 
	
	//rewind(pins[pin].handle);
	//fseek(pins[pin].handle, 0L, SEEK_SET) ;
	
	///  Finally i decided to close and open again for next read (uggly)
	
	// Must close :( otherwise next read has problems
	fclose(pins[pin].handle);  
	
	
	
	
	// open again for next read	

	strcpy(device,gpio_str);
	strcat(device,pins[pin].txtpin);
	strcat(device,gpio_val_str);
	
	
	if(strcmp(pins[pin].direction,"out")==0)
		mode=W_mode;
	else
		mode=R_mode;
				
				
	if ((handle = fopen(device,mode)) == NULL)
	{
    		DEBUG_PRINT((stdout, "On Read_Pin() Line: %d Cannot open %s. Try again later.\n",__LINE__,device ));
    		exit(1);
	}
	else
	{
			pins[pin].handle=handle;
			DEBUG_PRINT((stdout, "On Read_Pin() Line: %d  Pin port %s successfully opened (handle=%X) as %s \n",__LINE__,device,handle,mode ));
	}
		
	
	return(atoi(value));  // Return pin value 
}


void Set_Pin(int pin)
{
	

	if(fputc('1',pins[pin].handle)==-1) // int  write(  int  handle,  void  *buffer,  int  nbyte  );
	{
		DEBUG_PRINT((stdout, "Set_Pin: Cannot write to file %s (handle=%X). Try again later.\n",pins[pin].txtpin,pins[pin].handle));
		exit(1);
	}
	else
		DEBUG_PRINT((stdout, "Set_Pin write to file %s (handle=%X) successfully done.\n",pins[pin].txtpin,pins[pin].handle));
	
	fflush(pins[pin].handle);
}

void Clr_Pin(int pin)
{


	if(fputc('0',pins[pin].handle)==-1) // int  write(  int  handle,  void  *buffer,  int  nbyte  );
	{
		DEBUG_PRINT((stdout, "Clr_Pin: Cannot write to file %s (handle=%X). Try again later.\n",pins[pin].txtpin,pins[pin].handle));
		exit(1);
	}
	else
		DEBUG_PRINT((stdout, "Clr_Pin write to file %s (handle=%X) successfully done.\n",pins[pin].txtpin,pins[pin].handle));

	fflush(pins[pin].handle);
} 

void Export_Pins(void)
{

// Open export file 


	if ((handle_export = fopen(EXPORT,"w")) == NULL)
	{
    		DEBUG_PRINT((stdout, "Cannot open EXPORT File. Try again later.\n"));
    		exit(1);
	}

// And export pins
	
	for (i=0 ; i<sizeof(pins)/sizeof(PIN); i++)
	{ 
	
		nWritten=fputs(pins[i].txtpin,handle_export); 
		if (nWritten==-1){
			DEBUG_PRINT((stdout, "Cannot open EXPORT File for PIN %s(%s). Try again later.\n",pins[i].name,pins[i].txtpin));
			//	exit(1);
		}	
		else
			DEBUG_PRINT((stdout, "EXPORT File for PIN %s(%s) opened succesfully \n",pins[i].name,pins[i].txtpin));
		
		fflush(handle_export);  // Be carefull do this for EACH pin !!!
	}
	
	
}
	
	
void Set_PIN_Direction(void)
{
	char file[50]; 
	const char *gpio_str="/sys/class/gpio/gpio";
	const char *gpio_dir_str="/direction";
	FILE *handle_direction;
	
	
	
	
	for (i=0 ; i<sizeof(pins)/sizeof(PIN); i++)
	{ 
		// Open direction file 
	 
		strcpy(file,gpio_str);
		strcat(file,pins[i].txtpin);
		strcat(file,gpio_dir_str);
		
			
		if ((handle_direction = fopen(file,"w")) == NULL)
		{
    		
    		DEBUG_PRINT((stdout, "Cannot open DIRECTION File: %s\n",file));
    		exit(1);
		}

		// Set pins Directions
				
		if ((nWritten=fputs(pins[i].direction,handle_direction))==-1)
		{	
			
			DEBUG_PRINT((stdout, "Cannot open DIRECTION File for PIN %s(%s) as %s . Try again later.\n",pins[i].name,pins[i].txtpin,pins[i].direction));
			//exit(1);
		}
		else
		{	
		
			DEBUG_PRINT((stdout, "DIRECTION File for PIN %s(%s) opened succesfully \n",pins[i].name,pins[i].txtpin));
		    		
		}
		
		fflush(handle_direction); // Be carefull do this for EACH pin !!!
	}
	



	
}


// Open device port 
void Set_Port_handles(void)
{
	
	
	char device[50]; 
	const char *gpio_str="/sys/class/gpio/gpio";
	const char *gpio_val_str="/value";
	const char *W_mode="w";
	const char *R_mode="r";
	
	const char *mode=NULL;
	FILE *handle;
	
		
	for (i=0 ; i<sizeof(pins)/sizeof(PIN); i++)
	{ 
		
		strcpy(device,gpio_str);
		strcat(device,pins[i].txtpin);
		strcat(device,gpio_val_str);
		
		
		
		if(strcmp(pins[i].direction,"out")==0)
			mode=W_mode;
		else
			mode=R_mode;
				
				
		if ((handle = fopen(device,mode)) == NULL)
		{
    			DEBUG_PRINT((stdout, "Cannot open %s. Try again later.\n", device ));
    			exit(1);
		}
		else
		{
				pins[i].handle=handle;
				DEBUG_PRINT((stdout, "Pin port %s successfully opened (handle=%X) as %s \n", device,handle,mode ));
		}
		
	}
	
}





void scan_keys(void)
{

unsigned char k;
unsigned char keys;
unsigned char changed;
static unsigned char last=0;	// remember last key state


	keys=0;
	
	
		
	if(!(k=Read_Pin(PIN_BLUE_KEY)))				// if pressed
		keys|=BLUE_KEY_MASK;
		
	if(!(k=Read_Pin(PIN_GREEN_KEY)))
		keys|=GREEN_KEY_MASK;
	
	if(!(k=Read_Pin(PIN_RED_KEY)))
		keys|=RED_KEY_MASK;
	
	if(!(k=Read_Pin(PIN_YELLOW_KEY)))
		keys|=YELLOW_KEY_MASK;
	
	changed=last^keys;        // who changed
 	
	if(changed)
	{	
		//printf("Mask:%X  Change:%X\n",keys,changed);

		if((changed&BLUE_KEY_MASK)&keys)		// (changed & mask) & now   @posedge
		{
			PushQueue(BLUE_DOWN);	   
		   
		}
			
		if((changed&BLUE_KEY_MASK)&last)		// (changed & mask) & last  @negedge
		{
			PushQueue(BLUE_UP);	
		  
		}	
			
		if((changed&GREEN_KEY_MASK)&keys)		// (changed & mask) & now   @posedge
		{
			PushQueue(GREEN_DOWN);	
		
		}
		
		if((changed&GREEN_KEY_MASK)&last)		// (changed & mask) & last  @negedge
		{
			PushQueue(GREEN_UP);	
		
		}
		
		
		if((changed&RED_KEY_MASK)&keys)		// (changed & mask) & now   @posedge
		{
			PushQueue(RED_DOWN);	
		
		}
		
		if((changed&RED_KEY_MASK)&last)		// (changed & mask) & last  @negedge
		{
			PushQueue(RED_UP);	
		
		}
		
		if((changed&YELLOW_KEY_MASK)&keys)		// (changed & mask) & now   @posedge
		{
			PushQueue(YELLOW_DOWN);	
		
		}
		
		if((changed&YELLOW_KEY_MASK)&last)		// (changed & mask) & last  @negedge
		{
			PushQueue(YELLOW_UP);	
		
		}
		
		
	}


	last=keys;									// Update last key state
}


