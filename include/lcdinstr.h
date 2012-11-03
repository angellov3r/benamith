/*
BATRON I2C LCD Instructions
USAGE: Reference the relevant instruction, and bitwise OR ( | ) it with any additional wanted functions.
	eg: CONTROLBYTE | CONTROLBYTE_CO - this is a control byte and is the last control byte
*/

#ifndef LCDINSTR_H
#define LCDINSTR_H

#define		CONTROLBYTE			0b00000000 //Use to set which register you need to write to (see below)
#define		CONTROLBYTE_CO		0b10000000 //If unset, more control bytes will follow this one at some point. Set if this is the last control byte.
#define		CONTROLBYTE_RS		0b01000000 //If unset, write to the instruction register. Set if you want to write to the data register for writing/reading characters.

#define 	FUNCTIONSET 		0b00100000 //Sets various aspects of the LCD, see below. Note, this is useless alone, OR it with those below!
#define		FUNCTIONSET_X		0b00000001 //If unset, the standard instruction set is used. Set if you want to use the extended instruction set. You cannot use standard instructions while set.
#define		FUNCTIONSET_DL		0b00010000 //If unset, the data length mode is 4 bits. Set to use 8 bit data length. This should usually be set.
#define		FUNCTIONSET_M		0b00000100 //If unset, the screen display is 1 line x 32 characters(??). If set, 2 lines x 16 characters.
#define		FUNCTIONSET_SL		0b00000010 //If unset, see FUNCTIONSET_M. If set, overrides FUNCTIONSET_M and gives a 1 line x 16 character display.

/*STANDARD INSTRUCTION SET - Remember to issue a CONTROLBYTE where CONTROLBYTE_RS is NOT set!*/

#define		CLEARDISPLAY		0b00000001 		//Clears the display. Note the character set we use means it fills with arrows..

#define		RETURNHOME		0b00000010 		//Sets cursor and display to original position

#define		ENTRYMODESET		0b00000100 		//Sets cursor move direction and display shift
	#define		ENTRYMODESET_ID		0b00000010 	//If unset, cursor position will decrement with each character written. Set if you want to increment the position.
	#define		ENTRYMODESET_S		0b00000001 	//If unset, the display will not shift. Set if you want the display to shift (see below and ????)

#define		DISPLAYCONTROL		0b00001000 		//Screen power and cursor settings (see below)
	#define 	DISPLAYCONTROL_D	0b00000100 	//If unset, the display will go into power-down mode. Set to turn on the display.
	#define		DISPLAYCONTROL_C	0b00000010 	//If unset, the cursor will not be displayed. Set to display it.
	#define 	DISPLAYCONTROL_B	0b00000001 	//If unset, the cursor will not blink. Set to make it blink.

#define 	CURSORDISPLAYSHIFT	0b00010000 		//Shifts the cursor or display without changing memory contents (see below)
	#define		CURSORDISPLAYSHIFT_SC	0b00001000 	//If unset, the cursor is shifted. Set to shift the display.
	#define		CURSORDISPLAYSHIFT_RL	0b00000100 	//If unset, the cursor/display is shifted left. Set to shift right.

/*EXTENDED INSTRUCTION SET - Remember to issue a CONTROLBYTE where CONTROLBYTE_RS is NOT set! Also ensure you send a FUNCTIONSET byte where FUNCTIONSET_X IS set!*/

#define		X_SETVLCD		0b10000000		//Set VLCD (contrast)
	#define		X_SETVLCD_V		0b01000000 	//If unset, sets the voltage for character mode (usually what we want). If set, sets the voltage for character mode.
	/*Various voltages, bitwise OR these together to get what you want.*/
	#define		X_SETVLCD_1V		0b00000001
	#define		X_SETVLCD_2V		0b00000010
	#define		X_SETVLCD_4V		0b00000100
	#define		X_SETVLCD_8V		0b00001000
	#define		X_SETVLCD_16V		0b00010000
	#define		X_SETVLCD_32V		0b00100000
	
/*CHARACTER CODES - Remember to issue a CONTROLBYTE where CONTROLBYTE_RS IS set! Also, if you want to print an alphabet character, 
you MUST bitwise OR it with CHAR_UPPER or CHAR_LOWER! If you don't, the incorrect character will be printed!*/

#define		CHAR_UPPER	0b11000000
#define 	CHAR_LOWER	0b01100000

#define 	CHAR_A		0b00000001
#define 	CHAR_B		0b00000010
#define 	CHAR_C		0b00000011
#define 	CHAR_D		0b00000100
#define 	CHAR_E		0b00000101
#define 	CHAR_F		0b00000110
#define 	CHAR_G		0b00000111
#define 	CHAR_H		0b00001000
#define 	CHAR_I		0b00001001
#define 	CHAR_J		0b00001010
#define 	CHAR_K		0b00001011
#define 	CHAR_L		0b00001100
#define 	CHAR_M		0b00001101
#define 	CHAR_N		0b00001110
#define 	CHAR_O		0b00001111
#define 	CHAR_P		0b00010000
#define 	CHAR_Q		0b00010001
#define 	CHAR_R		0b00010010
#define 	CHAR_S		0b00010011
#define 	CHAR_T		0b00010100
#define 	CHAR_U		0b00010101
#define 	CHAR_V		0b00010110
#define 	CHAR_W		0b00010111
#define 	CHAR_X		0b00011000
#define 	CHAR_Y		0b00011001
#define 	CHAR_Z		0b00011010

#define		CHAR_0		0b10110000
#define		CHAR_1		0b10110001
#define		CHAR_2		0b10110010
#define		CHAR_3		0b10110011
#define		CHAR_4		0b10110100
#define		CHAR_5		0b10110101
#define		CHAR_6		0b10110110
#define		CHAR_7		0b10110111
#define		CHAR_8		0b10111000
#define		CHAR_9		0b10111001

#define		CHAR_SPACE	0b10100000
#define		CHAR_AT		0b10000000
#define		CHAR_POUND	0b10000001
#define		CHAR_DOLLAR	0b10000010
#define		CHAR_EXCLAM	0b10100001
#define		CHAR_DBLQUOTE	0b10100010
#define		CHAR_HASH	0b10100011
#define		CHAR_PERCENT	0b10100101
#define		CHAR_AMPERS	0b10100110
#define		CHAR_SGLQUOTE	0b10100111
#define		CHAR_LBRACKET	0b10101000
#define		CHAR_RBRACKET	0b10101001
#define		CHAR_ASTERIX	0b10101010
#define		CHAR_PLUS	0b10101011
#define		CHAR_COMMA	0b10101100
#define		CHAR_MINUS	0b10101101
#define		CHAR_PERIOD	0b10101110
#define		CHAR_COLON	0b10111010
#define		CHAR_SEMI	0b10111011
#define		CHAR_LESS	0b10111100
#define		CHAR_EQUALS	0b10111101
#define		CHAR_GREATER	0b10111110
#define		CHAR_QUESTION	0b10111111
#define		CHAR_LARROW	0b00010000
#define		CHAR_RARROW	0b00100000
#define		CHAR_UPARROW	0b00010010
#define		CHAR_DOWNARROW	0b00110000

#endif //LCDINSTR_H
