#include "lcd.h"

#include "LPC17xx.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"

#include <string.h>

int initialised = 0;
uint8_t rxBuff;

uint8_t *instrList;
int numInstr = 0;
int sizeInstrList = 0;

I2C_M_SETUP_Type trans;
trans.sl_addr7bit = 0x3b;
trans.tx_data = ( uint8_t * ) NULL;
trans.tx_length = sizeof( uint8_t );
trans.rx_data = &rxBuff;
trans.rx_length = sizeof( rxBuff );
trans.retransmissions_max = 2;

int LCD_Init( void )
{
	//Configure the I2C lines
	PINSEL_CFG_Type pin;
	pin.OpenDrain = PINSEL_PINMODE_NORMAL;
	pin.Pinmode = PINSEL_PINMODE_PULLUP;
	pin.Funcnum = PINSEL_FUNC_3;
	pin.Pinnum = PINSEL_PIN_0;
	pin.Portnum = PINSEL_PORT_0;
	PINSEL_ConfigPin( &pin );

	pin.Pinnum = PINSEL_PIN_1;
	PINSEL_ConfigPin( &pin );

	#ifdef DEBUG
	_DBG_( "Initialising I2C bus at 100KHz..\n" );
	#endif //DEBUG

	//Initialise the I2C bus at 100KHz and enable it.
	I2C_Init( LPC_I2C1, 100000 );
	I2C_Cmd( LPC_I2C1, ENABLE );

	#ifdef DEBUG
	char debugCBuff[ 100 ];
	sprintf( debugCBuff, "Attempting to communicate with LCD screen at address 0x%x.\n", trans.sl_addr7bit );
	_DBG_( debugCBuff );
	#endif //DEBUG

	Status status = I2C_MasterTransferData( LPC_I2C1, &trans, I2C_TRANSFER_POLLING );

	if ( status == SUCCESS )
	{
		#ifdef DEBUG
		_DBG_( "Found screen, allocating memory for instruction buffer..\n" );
		#endif //DEBUG

		sizeInstrList = 10;
		instrList = malloc( sizeof( uint8_t ) * sizeInstrList );

		#ifdef DEBUG
		_DBG_( "Initialisation complete.\n" );
		#endif //DEBUG

		initialised = 1;
		return 1;
	}
	else
	{
		#ifdef DEBUG
		_DBG_( "Error! Cannot communicate with LCD screen! Could be no power to screen or insufficient buffer size.\n" );
		#endif //DEBUG

		return 0;
	}
}

int LCD_AddInstr( uint8_t instr )
{
	#ifdef DEBUG
	char debugCBuff[ 100 ];
	sprintf( debugCBuff, "Attempting to add instruction 0x%x to instruction buffer.\n", instr );
	_DBG_( debugCBuff );
	#endif //DEBUG

	if ( numInstr == sizeInstrList ) //We've run out of memory, allocate some more.
	{
		#ifdef DEBUG
		_DBG_( "Filled buffer, extending..\n" );
		#endif //DEBUG

		int newSize = 2 * sizeInstrList; //Double previously allocated space.
		uint8_t *newList = malloc( sizeof( uint8_t ) * newSize ); //Allocate memory for new list.
		#ifdef DEBUG
		sprintf( debugCBuff, "Allocated %d bytes for new instruction buffer.\nCopying contents of old buffer into new buffer..\n", newSize );
		_DBG_( debugCBuff );
		#endif //DEBUG

		memset( newList, 0, sizeof( uint8_t ) * newSize );
		memcpy( newList, instrList, sizeof( uint8_t ) * sizeInstrList ); //Copy values from old list into the new list.

		free( instrList ); //Deallocate memory for the old list.
		
		//Update all of our globals.
		instrList = newList;
		sizeInstrList = newSize;
	}

	instrList[ numInstr ] = instr;
	numInstr++;

	return 1;
}

int LCD_Write( void )
{
	if ( numInstr == 0 ) //No instructions to send!
	{
		return 0;
	}

	trans.tx_data = instrList;
	trans.tx_length = sizeof( uint8_t ) * numInstr;

	Status status = I2C_MasterTransferData( LPC_I2C1, &trans, I2C_TRANSFER_POLLING );

	if ( status == SUCCESS )
	{
		//Clear out the instruction buffer
		memset( instrList, 0, sizeof( uint8_t ) * newSize );
		numInstr = 0;

		return 1;
	}
	else
	{
		return 0;
	}
}
