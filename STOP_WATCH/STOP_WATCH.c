/*
 * STOP_WATCH.c
 *
 *  Created on: Oct 18, 2020
 *      Author: Amr Mohamed
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char sec   = 0 ;       // sec counter
unsigned char min   = 0 ;       // min counter
unsigned char hours = 0 ;       // Hours counter

/********** INT0 Initialization Code **********/

void INT0_init( void )
{
	GICR  |= ( 1<<INT0 )  ;        // enable interrupt 0
	MCUCR |= ( 1<<ISC01 ) ;        // interrupt request at falling edge
	DDRD  &= ~( 1<<PD2 )  ;        // config pin 2 in PORTD as input pin
	PORTD |= ( 1<<PD2 )   ;        // enable internal pull up
}

/********** INT1 Initialization Code **********/

void INT1_init( void )
{
	GICR  |= ( 1<<INT1 ) ;                   // enable interrupt 0
	MCUCR |= ( 1<<ISC11 ) | ( 1<<ISC10 ) ;   // interrupt request at rising edge
	DDRD  &= ~( 1<<PD3 ) ;                   // config pin 3 in PORTD as input pin
}

/********** INT2 Initialization Code **********/
void INT2_init( void )
{
	GICR   |= ( 1<<INT2 )   ;        // enable interrupt 2
	MCUCSR &= ~( 1<<ISC2 )  ;        // interrupt request at falling edge
	DDRB   &= ~( 1<<PB2 )   ;        // config pin 2 in PORTB as input pin
	PORTB  |= ( 1<<PB2 )    ;        // enable internal pull up
}


/********** TIMER1 Initialization Code **********/
void Timer1_init_CTC_mode ( int ticks )
{
	TCNT1  = 0 ;                             // initial value = 0
	OCR1A  = ticks ;                         // compare value
	TIMSK |= ( 1<<OCIE1A ) ;                 // Channel A interrupt enable
	TCCR1B =  ( 1<<WGM12 ) | ( 1<<CS12 ) ;   // clock with prescaler clk/256
	TCCR1A = ( 1<<FOC1A ) ;

}

/********** ISR FOR INT0 **********/
/********** RESET STOP WATCH **********/
ISR(INT0_vect)
{
	sec   = 0 ;
	min   = 0 ;
	hours = 0 ;
	Timer1_init_CTC_mode ( 3906 ) ;
}

/********** ISR FOR INT1 **********/
/********** PAUSE STOP WATCH **********/
ISR(INT1_vect)
{
	TCCR1B &= 0xF8 ;                      // disable the clock of timer1
}

/********** ISR FOR INT2 **********/
/********** RESUME STOP WATCH **********/
ISR(INT2_vect)
{
	Timer1_init_CTC_mode ( 3906 ) ;      // enable timer
}

/********** ISR FOR TIMER1 **********/
ISR(TIMER1_COMPA_vect)
{
	if( sec == 59 )
	{
		if( min == 59 )
		{
			min = 0 ;
			sec = 0 ;
			hours++ ;
		}
		else
		{
			min++ ;
		}
		sec = 0 ;
	}
	else
	{
		sec++ ;
	}
}

/****************** main function *************/
int main ( void )
{
	/********** Initialization Code **********/

	DDRC  |= 0x0F ;              // configer first 4 pins of PORTC as output pins
	PORTC  = 0 ;                 // Initializing PORTC with value 0
	DDRB  &= ~( 1<<PB2 ) ;       // configer pin 2 in PORTB as input pin
	PORTB |= ( 1<<PB2 )  ;       // activating internal pull up
	DDRA  |= 0x3F ;              // configer first 6 pins of PORTA as output pins
	PORTA  = 0x00 ;              // Initializing PORTA with value 0
	SREG  |= ( 1<<7 ) ;          // set Global interrupt flag

	INT0_init() ;
	INT1_init() ;
	INT2_init() ;
	Timer1_init_CTC_mode ( 3906 ) ;

	while(1)
	{
		/********** Display Code **********/
		PORTA = 0x01;
		PORTC = ( PORTC & 0xF0 ) | ( ( sec % 10 ) & 0x0F ) ;    // display sec 0
		_delay_ms(6);
		PORTA = 0x02 ;
		PORTC = ( PORTC & 0xF0 ) | ( ( sec / 10 ) & 0x0F ) ;    // display sec 1
		_delay_ms(6);
		PORTA = 0x04 ;
		PORTC = ( PORTC & 0xF0 ) | ( ( min % 10 ) & 0x0F ) ;    // display min 0
		_delay_ms(6);
		PORTA = 0x08 ;
		PORTC = ( PORTC & 0xF0 ) | ( ( min / 10 ) & 0x0F ) ;    // display min 1
		_delay_ms(6);
		PORTA = 0x10 ;
		PORTC = ( PORTC & 0xF0 ) | ( ( hours % 10 ) & 0x0F ) ;  // display H0
		_delay_ms(6);
		PORTA = 0x20 ;
		PORTC = ( PORTC & 0xF0 ) | ( ( hours / 10 ) & 0x0F ) ;   // display H1
		_delay_ms(6);


	}

}
