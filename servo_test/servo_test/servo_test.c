/*
 * servo_test.c
 *
 * Created: 8/31/2017 6:48:27 PM
 * Author : troye
 *
 * So this is just a test of the pan/tilt device using two potentiometers. 
 * I'm not going to use a separate power supply, so it is entirely possible that I 
 * am going to totally ruin something. Hopefully that will not be the case...
 */ 

#include <avr/io.h>
#include <stdlib.h>

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

//Analog to Digital Conversion 
void init_ADC(){
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0) ; //Prescale by 128
	ADMUX = (1<<ADLAR) | (1<<REFS0); //left adjust (only need 8 bits) | use internal AREF
}

void doADC(int pin){
	ADCSRA |= (1<<ADSC); ///start conversion
	switch(pin){
		case(0):
		ADMUX &= ~(1<<MUX0);
		break;
		
		case(1):
		ADMUX |= (1<<MUX0);
		break;
	}
}

int main(void)
{
    /* Replace with your application code */
	UART_Init(MYUBBR);
	init_ADC();
	
    while (1){
		
		//Debugging: Print ADC values for pot0 and pot1 to serial
		char buffer [4];
		
		UART_trans_string("POT0: ");
		doADC(0);
		itoa(ADCH,buffer,10);
		UART_trans_string(buffer);
		
		UART_trans_string("  POT1: ");
		doADC(1);
		itoa(ADCH,buffer,10);
		UART_trans_string(buffer);

		UART_trans_string("\r\n");
		
    }
}

