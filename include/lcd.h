#ifndef LCD_H
#define LCD_H

/*
LCD_Init - sets up pins, the I2C bus, etc...
@return success/failure
*/
int LCD_Init( void );

/*
LCD_AddInstr
@param the instruction to addd to the instruction queue
@return success/failure
*/
int LCD_AddInstr( uint8_t instr )

/*
LCD_Write - send the queued instructions
@return success/failure
*/
int LCD_Write( void );

#endif //LCD_H
