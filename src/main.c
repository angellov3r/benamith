#include "LPC17xx.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"

#include <stdio.h>
#include <stdlib.h>

#include "debug_frmwrk.h"

#include "lcd.h"
#include "lcdinstr.h"

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
	//Actually clear the screen.

	//Turn on display, configure it, clear it (ish..) and set up entry mode.
	LCD_AddInstr( CONTROLBYTE );
	LCD_AddInstr( FUNCTIONSET | FUNCTIONSET_DL );
	LCD_AddInstr( CLEARDISPLAY);
	LCD_AddInstr( RETURNHOME );
	LCD_AddInstr( ENTRYMODESET | ENTRYMODESET_ID );
	LCD_AddInstr( DISPLAYCONTROL | DISPLAYCONTROL_D );
	LCD_Write();

	//Actually clear the screen.
	LCD_ClearScreen();

	LCD_WriteString( "Yo yo yo whats the happy haps" );

	//Last control byte, reset cursor position
	LCD_AddInstr( CONTROLBYTE | CONTROLBYTE_CO );
	LCD_AddInstr( RETURNHOME );
	LCD_Write();
	
	while ( 1 )
	{
	}

	return 0;
}

