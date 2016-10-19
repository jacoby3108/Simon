#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cqueue.h"

// bitbangspi v4.00 by Dany (User space version) 29-mayo-2016
// Note this version uses fopen high level function bear in mind that it is BUFFERED

//ON PI gcc simulacion.c portsimulator.c libterm.c flibspi.c -o simulacion Hi level fops (fopen)
//ON PI gcc simulacion.c portsimulator.c libterm.c libspi.c -o simulacion  Low level fops (open)
//ON PC gcc simulacion.c portsimulator.c libterm.o -o simulacion

//http://www.auctoris.co.uk/2012/08/23/gpio-with-sysfs-on-raspberry-pi-part-2/

//****************Select Debug  on/off *****************************************************
#define ON   1
#define OFF  0

#define DEBUG  OFF    

//*****************Select Hardware Type: Serial or parallel*********************************
#define PARA		0
#define SERIAL		1

#define HARDWARE 	SERIAL
//*****************************************************************************************


#if DEBUG == ON
# define DEBUG_PRINT(x) fprintf x
#elif DEBUG == OFF
# define DEBUG_PRINT(x) do {} while (0)
#endif

#define TEXTIFY(A) #A
#define GPIO(pin)  TEXTIFY(/sys/class/gpio/gpio ## pin )

typedef struct 
		{
			const unsigned char * name; 
		    const unsigned int  numpin;
		    const unsigned char * txtpin;
		    const unsigned char * direction;
			FILE * handle;
			
		} PIN;
			
			
#if HARDWARE == SERIAL


PIN pins[] ={{"BLUE_LED",18,"18","out",0},
			 {"GREEN_LED",25,"25","out",0},
			 {"RED_LED",23,"23","out",0},
			 {"YELLOW_LED",24,"24","out",0},
			 {"BLUE_KEY",27,"27","in",0},
			 {"GREEN_KEY",17,"17","in",0},
			 {"RED_KEY",4,"4","in",0},
			 {"YELLOW_KEY",22,"22","in",0}};
			 
			 

enum pin_index {
				PIN_BLUE_LED,
				PIN_GREEN_LED,
				PIN_RED_LED,
				PIN_YELLOW_LED,
				PIN_BLUE_KEY,
				PIN_GREEN_KEY,
				PIN_RED_KEY,
				PIN_YELLOW_KEY,
			   };

#elif HARDWARE == PARA
			
PIN pins[] ={
				{"D0",4,"4","out",0},
				{"D1",17,"17","out",0},
				{"D2",27,"27","out",0},
				{"D3",22,"22","out",0},
				{"D4",18,"18","out",0},
				{"D5",23,"23","out",0},
				{"D6",24,"24","out",0},
				{"D7",25,"25","out",0}};
				

				
enum pin_index {PIN_D0,
				PIN_D1,
				PIN_D2,
				PIN_D3,
				PIN_D4,
				PIN_D5,
				PIN_D6,
				PIN_D7};

#endif

// ============ KEYS ============


#define BLUE_KEY_MASK   0x01
#define GREEN_KEY_MASK  0x02
#define RED_KEY_MASK    0x04
#define YELLOW_KEY_MASK 0x08

#define BLUE_DOWN 		0x81 //(BLUE_KEY_MASK | 0x80)
#define BLUE_UP 		0x01 //(BLUE_KEY_MASK | 0x00)

#define GREEN_DOWN 		(GREEN_KEY_MASK | 0x80)
#define GREEN_UP 		(GREEN_KEY_MASK | 0x00)

#define RED_DOWN 		(RED_KEY_MASK | 0x80)
#define RED_UP 		    (RED_KEY_MASK | 0x00)

#define YELLOW_DOWN 	(YELLOW_KEY_MASK | 0x80)
#define YELLOW_UP 		(YELLOW_KEY_MASK | 0x00)


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
void Set_Pin(int pin);
void Clr_Pin(int pin);
int Read_Pin(int pin);
unsigned char spiWrite(const unsigned char regData);
void initspi(void);
void scan_keys(void);

FILE *handle_export;
int nWritten,nread;  // bytes read or written during file operations
unsigned char contador;
int i,j,k;

int main(void)
{
	
	unsigned char key ;
	QueueInit();	// initialize queue
	
	initspi();
	
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
			printf("BLUE DOWN \n");
			
			if(key==GREEN_DOWN)
			printf("GREEN DOWN \n");
		
		}
	
	}
	
	
	
	return 0;
}

void initspi(void)
{
	Export_Pins();					// Export Used pins to User space for debug
	Set_PIN_Direction();			// Set Pin I/O 
	Set_Port_handles();				// Anotate pin handles for I/O operations
}



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
	
	/// there is a problem reading continusly from this file 
	/// unfortunately fseek or rewind doesn't work. I must close / open every time 
	/// Pseudofiles like those in /sys may not work with those functions 
	
	//rewind(pins[pin].handle);
	//fseek(pins[pin].handle, 0L, SEEK_SET) ;
	
	///  Finally i decided to close and open again for next read (uggly)
	
	// Must close :( otherwise next read has problems
	fclose(pins[pin].handle);  
	
	
	
	
	// open again	

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
		   // printf("BDQin:%.2X  ",BLUE_DOWN);
		}
			
		if((changed&BLUE_KEY_MASK)&last)		// (changed & mask) & last  @negedge
		{
			PushQueue(BLUE_UP);	
		  //  printf("BUQin:%.2X  ",BLUE_UP);
		}	
			
		if((changed&GREEN_KEY_MASK)&keys)		// (changed & mask) & now   @posedge
		{
			PushQueue(GREEN_DOWN);	
		
		}
		
		if((changed&GREEN_KEY_MASK)&last)		// (changed & mask) & last  @negedge
		{
			PushQueue(GREEN_UP);	
		
		}
		
	}


	last=keys;									// Update last key state
}


