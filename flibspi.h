#ifndef FLIBSPI_H
#define FLIBSPI_H


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

#define BLUE_DOWN 		0x81 //(BLUE_KEY_MASK | 0x80)
#define BLUE_UP 		0x01 //(BLUE_KEY_MASK | 0x00)

#define GREEN_DOWN 		(GREEN_KEY_MASK | 0x80)
#define GREEN_UP 		(GREEN_KEY_MASK | 0x00)

#define RED_DOWN 		(RED_KEY_MASK | 0x80)
#define RED_UP 		    (RED_KEY_MASK | 0x00)

#define YELLOW_DOWN 	(YELLOW_KEY_MASK | 0x80)
#define YELLOW_UP 		(YELLOW_KEY_MASK | 0x00)

void Set_Pin(int pin);
void Clr_Pin(int pin);
int Read_Pin(int pin);
void InitHard(void);
void scan_keys(void);


#endif
