#include "keypad.h"

#include "LPC17xx.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_pinsel.h"

#include "debug_frmwrk.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int keypad_initialised = 0;
uint8_t keypad_rxBuff;

char keypad[ 4 ][ 4 ] = { { '1', '2', '3', 'A'}, { '4', '5', '6', 'B' }, { '7', '8', '9', 'C' }, { '*', '0', '#', 'D' } };

int Keypad_Init( void )
{
	I2C_M_SETUP_Type trans;
	trans.sl_addr7bit = 0x21;
	trans.tx_data = ( uint8_t * ) NULL;
	trans.tx_length = sizeof( uint8_t );
	trans.rx_data = &keypad_rxBuff;
	trans.rx_length = sizeof( keypad_rxBuff );
	trans.retransmissions_max = 2;

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
	sprintf( debugCBuff, "Attempting to communicate with keypad at address 0x%x.\n", ( unsigned int ) trans.sl_addr7bit );
	_DBG_( debugCBuff );
	#endif //DEBUG

	Status status = I2C_MasterTransferData( LPC_I2C1, &trans, I2C_TRANSFER_POLLING );

	if ( status == SUCCESS )
	{
		#ifdef DEBUG
		_DBG_( "Found keypad, allocating memory for instruction buffer..\n" );
		#endif //DEBUG

		#ifdef DEBUG
		_DBG_( "Initialisation complete.\n" );
		#endif //DEBUG

		keypad_initialised = 1;
		return 1;
	}
	else
	{
		#ifdef DEBUG
		_DBG_( "Error! Cannot communicate with keypad! Could be no power to keypad or insufficient buffer size.\n" );
		#endif //DEBUG

		return 0;
	}
}

char decode_keypad( uint8_t data )
{
	int col, row;
	data = 0xFF ^ data; //invert
	col = 4;
	row = 4;

	if ( data & 0x80 == 0x80)
	{
		col = 0;
	}
	else if ( data & 0x40 == 0x40 )
	{
		col = 1;
	}
	else if (data & 0x20 == 0x20 )
	{
		col = 2;
	}
	else if ( data & 0x10 == 0x10 )
	{
		col = 3;
	}

	if ( data & 0x08 == 0x08 ) 
	{
		row = 0;
	}
	else if ( data & 0x04 == 0x04 )
	{
		row = 1;
	}
	else if ( data & 0x02 == 0x02 )
	{
		row = 2;
	}
	else if ( data & 0x01 == 0x01 )
	{
		row = 3;
	}

	if ( ( col == 4 ) || ( row == 4 ) ) //If row or column was not transmitted, X will be returned
	{
		#ifdef DEBUG
		_DBG_( "Warning: Row or column data was not transmitted!\n" );
		#endif //DEBUG
		return 'X';
	}
	else
	{ //Tom edited this
		#ifdef DEBUG		
		char buff[ 100 ];
		sprintf( buff, "Successfully decoded keypad data, character: %c\n", keypad[ row ][ col ] );
		_DBG_( buff );
		#endif //DEBUG

		return keypad[ row ][ col ];
	}
}

void delay( int duration )
{
	int i;
	i = 0;
	while (i < duration)
		i++;
}

char Keypad_GetInput( void )
{
	if ( !keypad_initialised )
	{
		#ifdef DEBUG
		_DBG_( "Error: Tried to fetch input from keypad without initialising!\n" );
		#endif //DEBUG

		return '\0';
	}

	uint8_t rowOut = 0x0F;
	uint8_t colOut = 0xF0;

	uint8_t rowIn = 0x00;
	uint8_t colIn = 0x00;

	I2C_M_SETUP_Type rowTrans;
	rowTrans.sl_addr7bit = ( 0x42 >> 1 );
	rowTrans.retransmissions_max = 2;
	rowTrans.tx_data = &rowOut;
	rowTrans.tx_length = sizeof( rowOut );
	rowTrans.rx_data = &rowIn;
	rowTrans.rx_length = sizeof( rowIn );

	I2C_M_SETUP_Type colTrans;
	colTrans.sl_addr7bit = ( 0x42 >> 1 );
	colTrans.retransmissions_max = 2;
	colTrans.tx_data = &colOut;
	colTrans.tx_length = sizeof( colOut );
	colTrans.rx_data = &colIn;
	colTrans.rx_length = sizeof( colIn );
	
	Status i2c_status_row;
	Status i2c_status_col;

	while ( 1 )
	{
		i2c_status_row = I2C_MasterTransferData( LPC_I2C1, &rowTrans, I2C_TRANSFER_POLLING );
		i2c_status_col = I2C_MasterTransferData( LPC_I2C1, &colTrans, I2C_TRANSFER_POLLING );

		//Sometimes when the key is released only i2c_status_row transmission is successful, whereas
		//i2c_status_col returns no data. Therefore we need to check if both packets are received
		if ( ( i2c_status_row == SUCCESS ) && ( i2c_status_col == SUCCESS ) && ( colIn != 0xF0 ) && ( rowIn != 0x0F ) )
		{
			#ifdef DEBUG
			_DBG_( "Keypad data transfer sucessful, decoding..\n" );
			#endif //DEBUG

			delay( 0xFFFF0 );
			delay( 0xFFFF0 );
			return decode_keypad( colIn | rowIn );
		}
	}
}
