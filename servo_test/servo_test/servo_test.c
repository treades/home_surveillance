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
void UART_trans(unsigned char data){
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}

//transmit a full string over UART
void UART_trans_string(const char* str){
	while(*str){
		UART_trans(*str);
		str++;
	}
}


int main(void)
{
    /* Replace with your application code */
	UART_Init(MYUBBR);
	
    while (1){
		UART_trans_string("hello world!\r\n");
    }
}

