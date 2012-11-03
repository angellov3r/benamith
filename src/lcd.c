#include "lcd.h"
#include "lcdinstr.h"

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

	if ( !initialised )
	{
		#ifdef DEBUG
		_DBG_( "Error: Tried to add instruction to instruction buffer without initialising!\n" );
		#endif //DEBUG		

		return 0;
	}

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

		#ifdef DEBUG
		sprintf( debugCBuff, "Deallocating %d bytes for old instruction buffer..\n", sizeInstrList );
		_DBG_( debugCBuff );
		#endif //DEBUG		
		free( instrList ); //Deallocate memory for the old list.
		
		//Update all of our globals.
		instrList = newList;
		sizeInstrList = newSize;
	}

	instrList[ numInstr ] = instr;
	numInstr++;

	#ifdef DEBUG
	sprintf( debugCBuff, "Added instruction 0x%x to buffer (#%d in batch ).\n", instr, numInstr );
	_DBG_( debugCBuff );
	#endif //DEBUG

	return 1;
}

int LCD_Write( void )
{
	if ( !initialised )
	{
		#ifdef DEBUG
		_DBG_( "Error: Tried to send buffered instructions without initialising!\n" );
		#endif //DEBUG		

		return 0;
	}

	if ( numInstr == 0 ) //No instructions to send!
	{
		#ifdef DEBUG
		_DBG_( "Warning: Attempted to send buffered instructions but the buffer is empty!" );
		#endif //DEBUG

		return 0;
	}

	trans.tx_data = instrList;
	trans.tx_length = sizeof( uint8_t ) * numInstr;

	#ifdef DEBUG
	char debugCBuff[ 100 ];
	sprintf( debugCBuff, "Attempting to send %d buffered instructions..\n", numInstr );
	_DBG_( debugCBuff );
	#endif //DEBUG

	Status status = I2C_MasterTransferData( LPC_I2C1, &trans, I2C_TRANSFER_POLLING );

	if ( status == SUCCESS )
	{

		#ifdef DEBUG
		_DBG_( "Successfully sent buffered instructions. Clearing buffer..\n" );
		#endif //DEBUG

		//Clear out the instruction buffer
		memset( instrList, 0, sizeof( uint8_t ) * newSize );
		numInstr = 0;

		return 1;
	}
	else
	{
		#ifdef DEBUG
		_DBG_( "Error! Couldn't send buffered instructions correctly/at all!\n" );
		#endif //DEBUG

		return 0;
	}
}

int LCD_ClearScreen( void )
{
	LCD_AddInstr( CONTROLBYTE | CONTROLBYTE_RS );

	int res = 1;

	int i;
	for ( i = 0; spaceI < 32; spaceI++ )
	{
		res = res && LCD_AddInstr( CHAR_SPACE );
	}

	//Reset cursor position.
	res = res && LCD_AddInstr( CONTROLBYTE );
	res = res && LCD_AddInstr( RETURNHOME );

	if ( res )
	{
		return LCD_Write();
	}
	else
	{
		#ifdef DEBUG
		_DBG_( "Error: Failed adding instructions to buffer while attempting to clear screen.\n" );
		#endif //DEBUG

		return 0;
	}
}

//Oh god this is gonna be horrible..
int LCD_WriteString( char *str )
{
	LCD_AddInstr( CONTROLBYTE | CONTROLBYTE_RS );

	while ( *str != '\0' )
	{

		char c = *str;
		uint8_t code = 0;

		if ( 'a' <= c <= 'o' )
		{
			code = c - 'a' + 1;
			LCD_AddInstr( CHAR_LOWER | code );
		}
		else if ( 'p' <= c <= 'z' )
		{
			code = c - 'p' + 0x10;
			LCD_AddInstr( CHAR_LOWER | code );
		}
		else if ( 'A' <= c <= 'O' )
		{
			code = c - 'A' + 1;
			LCD_AddInstr( CHAR_UPPER | code );
		}
		else if ( 'P' <= c <= 'Z' )
		{
			code = c - 'P' + 0x10;
			LCD_AddInstr( CHAR_UPPER | code );
		}
		else if ( '0' <= c <= '9' )
		{
			code = c - '0' + 0xB0;
			LCD_AddInstr( code );
		}
		else
		{
			switch ( c )
			{
			case ' ':
				LCD_AddInstr( CHAR_SPACE );
				break;
			case '@':
				LCD_AddInstr( CHAR_AT );
				break;
			case '£':
				LCD_AddInstr( CHAR_POUND );
				break;
			case '$':
				LCD_AddInstr( CHAR_DOLLAR );
				break;
			case '!':
				LCD_AddInstr( CHAR_EXCLAM );
				break;
			case '"':
				LCD_AddInstr( CHAR_DBLQUOTE );
				break;
			case '#':
				LCD_AddInstr( CHAR_HASH );
				break;
			case '%':
				LCD_AddInstr( CHAR_PERCENT );
				break;
			case '&':
				LCD_AddInstr( CHAR_AMPERS );
				break;
			case '\'':
				LCD_AddInstr( CHAR_SGLQUOTE );
				break;
			case '(':
				LCD_AddInstr( CHAR_LBRACKET );
				break;
			case ')':
				LCD_AddInstr( CHAR_RBRACKET );
				break;
			case '*':
				LCD_AddInstr( CHAR_ASTERIX );
				break;
			case '+':
				LCD_AddInstr( CHAR_PLUS );
				break;
			case ',':
				LCD_AddInstr( CHAR_COMMA );
				break;
			case '-':
				LCD_AddInstr( CHAR_MINUS );
				break;
			case '.':
				LCD_AddInstr( CHAR_PERIOD );
				break;
			case ':':
				LCD_AddInstr( CHAR_COLON );
				break;
			case ';':
				LCD_AddInstr( CHAR_SEMI );
				break;
			case '<':
				LCD_AddInstr( CHAR_LESS );
				break;
			case '>':
				LCD_AddInstr( CHAR_GREATER );
				break;
			case '=':
				LCD_AddInstr( CHAR_EQUALS );
				break;
			case '?':
				LCD_AddInstr( CHAR_QUESTION );
				break;
			default:
				#ifdef DEBUG
				char buff[ 100 ];
				sprintf( buff, "Warning: Couldn't match character '%c'\n", c );
				_DBG_( buff );
				#endif //DEBUG
				break;
			}
		}

		str++;
	}

	LCD_Write();
	
	return 1;	
}
