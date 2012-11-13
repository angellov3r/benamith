#ifndef LCD_H
#define LCD_H

#include "LPC17xx.h"

/**
Initialises the I2C bus. Returns 0 on failure, 1 on success.
@return success/failure
*/
int Keypad_Init( void );

/**
Returns character representing the button currently being pressed.
@returns character pressed
*/
char Keypad_GetInput( void );


#endif //LCD_H
