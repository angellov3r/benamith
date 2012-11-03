#include "LPC17xx.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"

#include <stdio.h>
#include <stdlib.h>

#include "debug_frmwrk.h"

#include "lcd.h"

int main( void )
{
	debug_frmwrk_init();
	
	int init = LCD_Init();

	if ( !init ) //whaaaaaaat
	{
		return 1;
	}

	uint8_t voltage = X_SETVLCD_16V | X_SETVLCD_8V | X_SETVLCD_4V;

	//Setup voltage
	LCD_AddInstr( CONTROLBYTE );
	LCD_AddInstr( FUNCTIONSET | FUNCTIONSET_X | FUNCTIONSET_DL );
	LCD_AddInstr( X_SETVLCD | voltage );
	LCD_AddInstr( X_SETVLCD | X_SETVLCD_V | voltage );
	LCD_Write();

	//Turn on display, configure it, clear it (ish..) and set up entry mode.
	LCD_AddInstr( CONTROLBYTE );
	LCD_AddInstr( FUNCTIONSET | FUNCTIONSET_DL );
	LCD_AddInstr( CLEARDISPLAY);
	LCD_AddInstr( RETURNHOME );
	LCD_AddInstr( ENTRYMODESET | ENTRYMODESET_ID );
	LCD_AddInstr( DISPLAYCONTROL | DISPLAYCONTROL_D );
	LCD_Write();

	//Actually clear the screen.
	LCD_Clear();

	//"Hello, world!"
	LCD_AddInstr( CONTROLBYTE | CONTROLBYTE_RS );
	LCD_AddInstr( CHAR_UPPER | CHAR_H );
	LCD_AddInstr( CHAR_LOWER | CHAR_E );
	LCD_AddInstr( CHAR_LOWER | CHAR_L );
	LCD_AddInstr( CHAR_LOWER | CHAR_L );
	LCD_AddInstr( CHAR_LOWER | CHAR_O );
	LCD_AddInstr( CHAR_COMMA );
	LCD_AddInstr( CHAR_SPACE );
	LCD_AddInstr( CHAR_LOWER | CHAR_W );
	LCD_AddInstr( CHAR_LOWER | CHAR_O );
	LCD_AddInstr( CHAR_LOWER | CHAR_R );
	LCD_AddInstr( CHAR_LOWER | CHAR_L );
	LCD_AddInstr( CHAR_LOWER | CHAR_D );
	LCD_AddInstr( CHAR_EXCLAM );
	LCD_Write();

	//Last control byte, reset cursor position
	LCD_AddInstr( CONTROLBYTE | CONTROLBYTE_CO );
	LCD_AddInstr( RETURNHOME );
	LCD_Write();

	while ( 1 )
	{
	}

	return 0;
}
