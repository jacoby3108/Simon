#ifndef FLIBSPI_H
#define FLIBSPI_H

// ============ Colored Text ===========

#define BLACK_TEXT		   "\e[30;1m"
#define RED_TEXT		   "\e[31;1m"
#define GREEN_TEXT 		   "\e[32;1m"
#define YELLOW_TEXT 	   "\e[33;1m"
#define BLUE_TEXT 		   "\e[34;1m"
#define MAGENTA_TEXT 	   "\e[35;1m"
#define CYAN_TEXT 	   	   "\e[36;1m"
#define WHITE_TEXT 		   "\e[37m"

// ============ SOUNDS ===========

#define SIMON_RED_SOUND 		system("aplay -q ./sounds/red.wav")	
#define SIMON_GREEN_SOUND 		system("aplay -q ./sounds/green.wav")	
#define SIMON_BLUE_SOUND 		system("aplay -q ./sounds/blue.wav")	
#define SIMON_YELLOW_SOUND 		system("aplay -q ./sounds/yellow.wav")	
#define SIMON_BUZZ_SOUND 		system("aplay -q ./sounds/buzz.wav")

// ============ LEDS ============

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

// ============ KEYS ============


#define BLUE_KEY_MASK   0x01
#define GREEN_KEY_MASK  0x02
#define RED_KEY_MASK    0x04
#define YELLOW_KEY_MASK 0x08

#define BLUE_DOWN 		(BLUE_KEY_MASK | 0x80)
#define BLUE_UP 		(BLUE_KEY_MASK | 0x00)

#define GREEN_DOWN 		(GREEN_KEY_MASK | 0x80)
#define GREEN_UP 		(GREEN_KEY_MASK | 0x00)

#define RED_DOWN 		(RED_KEY_MASK | 0x80)
#define RED_UP 		    (RED_KEY_MASK | 0x00)

#define YELLOW_DOWN 	(YELLOW_KEY_MASK | 0x80)
#define YELLOW_UP 		(YELLOW_KEY_MASK | 0x00)

#define NO_KEY			0xFF

void Set_Pin(int pin);
void Clr_Pin(int pin);
int Read_Pin(int pin);
void InitHard(void);
void scan_keys(void);


#endif
