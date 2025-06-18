/*
 * GccApplication2.c
 *
 * Created: 13-5-2025 10:55:01
 * Author : jimmy
 */ 

#define F_CPU 16000000UL

#include <avr/sfr_defs.h>
#include <avr/io.h>
#include <util/delay.h>

typedef unsigned char byte;


inline void serial_write_byte(byte c)
{
	loop_until_bit_is_set(UCSR0A, UDRE0); UDR0 = c;
}

inline byte serial_read_byte()
{
	loop_until_bit_is_set(UCSR0A, RXC0); return UDR0;
}

byte serial_read_nibble()
{
	byte nibble = serial_read_byte();
	
	if ((nibble >= 'A') && (nibble <= 'F'))
	{
		nibble -= 'A' - 10;
	}
	else if ((nibble >= 'a') && (nibble <= 'f'))
	{
		nibble -= 'a' - 10;
	}
	else if ((nibble >= '0') && (nibble <= '9'))
	{
		nibble -= '0';
	}
	else
	{
		nibble = 0;
	}
	
	return nibble;
}

void shift_out_595(uint8_t data) {
	for (int i = 7; i >= 0; i--) {
		// Zet DATA bit
		if (data & (1 << i))
		PORTB |= (1 << PB0);   // DATA hoog	;	Port D8 arduino nano
		else
		PORTB &= ~(1 << PB0);  // DATA laag

		// Pulse CLOCK (SRCLK)
		PORTB |= (1 << PB1);		// port D9 arduino nano
		PORTB &= ~(1 << PB1);
	}

	// Pulse LATCH (RCLK)
	PORTD |= (1 << PD3);
	PORTD &= ~(1 << PD3);			// port D3 arduino nano

	// Zet DATA terug naar 0
	PORTB &= ~(1 << PB0);
}

void SPI_transmit(uint8_t SPI_data){
	SPDR = SPI_data; // Zet de data klaar om verzonden te worden
}


uint8_t SPI_receive() {		
	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)));
	/* Return Data Register */
	return SPDR;
}


byte SPI_read_nibble()
{
	byte SPI_nibble = SPI_receive();
	
		if ((SPI_nibble >= 'A') && (SPI_nibble <= 'F'))
		{
			SPI_nibble -= 'A' - 10;
		}
		else if ((SPI_nibble >= 'a') && (SPI_nibble <= 'f'))
		{
			SPI_nibble -= 'a' - 10;
		}
		else if ((SPI_nibble >= '0') && (SPI_nibble <= '9'))
		{
			SPI_nibble -= '0';
		}
		else
		{
			SPI_nibble = 0;
		}
		
		return SPI_nibble;
	 
}

uint8_t readBinairy()
{
	uint8_t Binair = PINC & 0x03;
	
	return Binair;
}



const byte digits[16] = {
	~0x7E,		// 0		
	~0x48,		// 1		
	~0x3D,		// 2		
	~0x6D,		// 3		
	~0x4B,		// 4		
	~0x67,		// 5		
	~0x77,		// 6		
	~0x4C,		// 7		
	~0x7F,		// 8
	~0x6F,		// 9	
	~0x5F,		// A
	~0x7f,		// B
	~0x36,		// C
	~0x7E,		// D
	~0x37,		// E
	~0x17		// F	
};



int main(void)
{

	UBRR0L = 0X10;
	UBRR0H = 0;
	
	UCSR0B = (1 << RXEN0)|(1 << TXEN0);						// receive & transmit enabled
	
	UCSR0C = 3 << UCSZ00;									// 8 data-bits, 1 stop-bit, no parity
	
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB4) ;			// Outputs
	DDRB &= ~((1 << PB2) | (1 << PB3) | (1 << PB5));		// Inputs
	DDRD |= (1 << PD3) | (1 << PD5);						// Outputs
	DDRC &= ~(0x03);										// Inputs
	
	
	/* Enable SPI */
	SPCR = (1 << SPE);
	
	while (1)
	{
		
		if (UCSR0A & (1 << RXC0)){			
		byte input = serial_read_byte();
		
		
		if (input == 'B')	
		{
			byte hi = serial_read_nibble();
			byte lo = serial_read_nibble();
			
			PORTB = (hi << 4) | lo;
			
			serial_write_byte('a');
			serial_write_byte('a');
			serial_write_byte('n');
			serial_write_byte(' ');				
		}
		
		
		
		else if (input == 'D' || input == 'd')	
		{
			
			byte lo = serial_read_nibble();
			//byte value = (hi << 4) | lo;
			byte value = lo;
			
			if (value <= 15)
			{
				byte number = digits[value];
				
		
				if (value > 9) {
					number &= ~(1 << 7);				// DP aan bij getallen groter dan 9
				}
				
				shift_out_595(number);
				
				serial_write_byte('O');
				serial_write_byte('K');
				serial_write_byte(' ');
			}

			
			 else 
			 {
				serial_write_byte('E');					// Foutmelding
				serial_write_byte('R');
				serial_write_byte('R');
				serial_write_byte(' ');
			 }
		
		
		} 
		
		
		
		
		else if(input == 'S' || input == 's')
		{
			byte hi = serial_read_nibble();
			byte lo = serial_read_nibble();
			byte segments = (hi << 4) | lo;
			
			shift_out_595(~segments);
			
			if(segments == 0xff){
			serial_write_byte('O');
			serial_write_byte('n');
			serial_write_byte(' ');				
			}	
			else if(segments == 0x00){
			serial_write_byte('O');
			serial_write_byte('F');
			serial_write_byte('F');	
			serial_write_byte(' ');				
			}
							
		}
		
		
		
		
		else if(input == 'M' || input == 'm')
		{
			byte number = serial_read_nibble();
			byte state = serial_read_nibble();
			byte segment;
			
			if((number >= 0 && number <= 8) && (state == 0 || state == 1))
			{
				
				
				if (state == 1)
				{
					segment &= ~(1 << number);
				}
								
				else
				{
					segment |= (1 << number);
				}
				
				shift_out_595(segment);
				serial_write_byte('S');
				serial_write_byte('E');
				serial_write_byte('G');
				serial_write_byte('M');
				serial_write_byte('E');
				serial_write_byte('N');
				serial_write_byte('T');
				serial_write_byte(' ');					
			}
			
			else
			{
				serial_write_byte('E');  // Foutmelding
				serial_write_byte('R');
				serial_write_byte('R');
				serial_write_byte(' ');				
			}
		}
		
				
		else if(input == 'Y' || input == 'y')
		{
			PORTD |= (1 << PD5);
			_delay_us(100);
			PORTD &= ~(1 << PD5);
			
			serial_write_byte('B');
			serial_write_byte('I');
			serial_write_byte('N');
			serial_write_byte(' ');
			
		}
		
		
		
		else
		{
			serial_write_byte('n');
			serial_write_byte('i');
			serial_write_byte('x');	
			serial_write_byte(' ');	
		}
		
		
		

	}
	
	else if(SPSR & (1<<SPIF))
	{
		
		byte incomming = SPI_receive();

		if(incomming == 'W' || incomming == 'w')
		{
			
			byte low = SPI_read_nibble();
						
			if (low <= 15)
			{
				byte numberSPI = digits[low];
				
				
				if (low > 9) 
				{
					numberSPI &= ~(1 << 7);				// DP aan bij getallen groter dan 9
				}
				
				shift_out_595(numberSPI);
				
				serial_write_byte('S');
				serial_write_byte('P');
				serial_write_byte('I');
				serial_write_byte(' ');
			}

		}
		
		else if(incomming == 'R' || incomming == 'r')
		{
			serial_write_byte('r');
			uint8_t data_to_send = readBinairy();
			SPI_transmit(data_to_send);
			
		}
	}
	
	}
		}

