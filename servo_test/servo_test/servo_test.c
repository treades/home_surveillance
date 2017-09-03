/*
 * servo_test.c
 *
 * Created: 8/31/2017 6:48:27 PM
 * Author : Troy Eades (troyeades@gmail.com)
 *
 * So this is just a test of the pan/tilt device using two potentiometers and an ATmega328p. 
 * I'm not going to use a separate power supply, so it is entirely possible that I 
 * am going to totally ruin something. Hopefully that will not be the case...
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <stdint.h>

//These defines, UART_Init, and UART_trans are just from the 328P data sheet
//UART will just be used for debugging ADC from the potentiometers
#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBBR F_CPU/16/BAUD-1

//Initialize UART communication 
void UART_Init(unsigned int ubrr){
	UBRR0H = (unsigned char) (ubrr>>8);
	UBRR0L = (unsigned char) ubrr;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
}

//Transmit a byte over UART
void UART_trans_byte(unsigned char data){
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

//Transmit a full string over UART
void UART_trans_string(const char* str){
	while(*str){
		UART_trans_byte(*str);
		str++;
	}
}

/*Analog to Digital Conversion 
* Prescaler = 128
* Left adjust (8 bits)
* use internal AREF
*/
void init_ADC(){
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0); 
	ADMUX = (1<<ADLAR) | (1<<REFS0); 
}

void doADC(int pin){
	switch(pin){
		case(0):
		ADMUX &= ~(1<<MUX0);
		break;
		
		case(1):
		ADMUX |= (1<<MUX0);
		break;
	}
	ADCSRA |= (1<<ADSC); ///start conversion
	while(ADCSRA & (1<<ADSC)); //busy wait while conversion happening
}

/*Pulse width modulation using timer 1
* Fast PWM with ICR1 for TOP
* Prescaler = 8
*/
void init_PWM(){
	DDRB |= 1<<DDB1;
	DDRB |= 1<<DDB2;
	TCCR1A = (1<<WGM11) | (1<<COM1A1) | (1<<COM1B1);
	TCCR1B = (1<<WGM12) | (1<<WGM13) | (1<<CS11);
	ICR1 = 0x9C40; //40000
}


int main(void)
{

	UART_Init(MYUBBR);
	init_ADC();
	init_PWM();
	char buffer [4]; //for printing to serial
	
    while (1){
		
		//PAN
		doADC(0);
		double panAngle = 1500+( (3000/255)*ADCH );
		OCR1A = (uint16_t)panAngle;
	
		UART_trans_string("POT0: ");
		itoa(ADCH,buffer,10);
		UART_trans_string(buffer);
		
		
		//TILT
		doADC(1);
		double tiltAngle = 1500+( (3000/255)*ADCH );
		OCR1B = (uint16_t)tiltAngle;
		
		UART_trans_string("  POT1: ");
		itoa(ADCH,buffer,10);
		UART_trans_string(buffer);

		UART_trans_string("\r\n");
		
    }
}

