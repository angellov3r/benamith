#ifndef LCD_H
#define LCD_H

#include "LPC17xx.h"

/**
Initialises the I2C bus. Returns 0 on failure, 1 on success.
@return success/failure
*/
int LCD_Init( void );

/**
Adds an instruction to the instruction buffer. Returns 0 on failure, 1 on success.
@param the instruction to add
@return success/failure
*/
int LCD_AddInstr( uint8_t instr );

/**
Sends buffered instructions to the LCD screen. Returns 0 on failure, 1 on success.
@return success/failure
*/
int LCD_Write( void );

/**
Helper procedure to clear the LCD's screen correctly with the R character set. Returns 0 on failure, 1 on success.
@return success/failure
*/
int LCD_ClearScreen( void );

/**
Helper procedure to write a C string to the LCD screen. Returns 0 on failure, 1 on success/
@param string to write to the screen
@return success/failure
*/
int LCD_WriteString( char *str );

#endif //LCD_H
