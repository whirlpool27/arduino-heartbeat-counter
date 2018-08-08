/** Heartbeat Sensor with Arduino (7 Segment Display)                                                                     **/
/** Gilang Anggara 13214076                                                                                               **/
/** Nafitri Aulia 13214143																								  **/
/** Class 1                                                                                                               **/
/** May 19, 2017                                                                                                          **/
/***************************************************************************************************************************/
/** Purpose : This program is a heartbeat sensor by using sensor from pulsesensor.com and displaying it in Heartbeat 	  **/
/**			  per minute 																								  **/
/***************************************************************************************************************************/
/** Variable Description :                                                                                                **/
/** Variable Name       Type        Description                                                                           **/
/** pulse 				integer		the reading of voltage level from adc 												  **/
/** threshold			integer		threshold level for pulse, beat's counted when threshold > pulse 					  **/
/** beat				integer		heartbeat counted																	  **/
/** bpm					float		heartbeat per minute																  **/
/** flag 				integer 	determining whether the voltage is more or less than threshold						  **/
/** digitX				integer		store 7 segment display digit 														  **/
/** now 				float		duration measured from start 														  **/
/** Source Code :                                                                                                         **/
/***************************************************************************************************************************/
/**                                                       MODULES USED 	                                                  **/

//include standard avr input output
#include <avr/io.h>

//define clock used 16 MHz
#define F_CPU 16000000UL

//include utilities for delay
#include <util/delay.h>

//include interrupt
#include <avr/interrupt.h>

/***************************************************************************************************************************/
/** 													GLOBAL VARIABLES												  **/
/***************************************************************************************************************************/
int pulse = 0;
int threshold = 550;
int beat = 0;
float bpm = 0;
int flag = 0;
int digit3, digit2, digit1;
float now;

/***************************************************************************************************************************/
/**										FUNCTION & PROCEDURE DELCARATION												  **/
/***************************************************************************************************************************/
//define pin output used
void getPin(void);

//calculate BPM
void calculate(void);

//display 7 segment digit
void display_number(int number);

//turn off 7 segment display
void turnoff_display(void);

//display BPM to 7 segment (3 digit)
void display_heartbeat(void);

//start timer 1 (16 bit, 1024 prescaler)
void start_timer (void);

//initialise register needed for ADC0
void init_adc(void);

//start ADC conversion
void start_conversion(void);

/***************************************************************************************************************************/
/**															MAIN PROGRAM 												  **/
/***************************************************************************************************************************/
int main(void){
	getPin();
	init_adc();
	start_conversion();
	start_timer();

	while(1){
		//determine pulse state (high or low)
		// Description :
		// flag = 1 and pulse > threshold, HIGH
		// flag = 0 and pulse < threshold, LOW
		// count every rising edge
		if (pulse > threshold and flag == 0){ // rising edge
		  flag = 1;
		  calculate();
		} else if (pulse < threshold and flag == 1){ // falling edge
		  flag = 0;
		}

		//display the output to 7 segment
		display_heartbeat();
	}
}

/***************************************************************************************************************************/
/**											 FUNCTION & PROCEDURE DEFINITION 											  **/
/***************************************************************************************************************************/
ISR(TIMER1_COMPA_vect){
	now += 10.048;
}

ISR(ADC_vect){
	pulse = ADC;
	start_conversion();
}

void getPin(void){
	DDRB = 0x3F; //port B 0 to 5
	DDRD = 0xF0; //port D 4 to 7
}

void calculate(void){
	if (now > 1000){
		//start counting beat after 1 second to avoid early overshoot
		beat += 1;

		//count bpm after 5 s to get a more precise bpm
		if (now > 5000){
			bpm = beat/((now-1000)/1000)*60;
		}
	}
}

void display_number(int number){
	switch(number){
		case 0:
		PORTB |= 1 << 5; // A
		PORTB |= 1 << 4; // B
		PORTB |= 1 << 3; // C
		PORTB |= 1 << 2; // D
		PORTB |= 1 << 1; // E
		PORTB |= 1 << 0; // F
		break;
		case 1:
		PORTB |= 1 << 4; // B
		PORTB |= 1 << 3; // C
		break;
		case 2:
		PORTB |= 1 << 5; // A
		PORTB |= 1 << 4; // B
		PORTB |= 1 << 2; // D
		PORTB |= 1 << 1; // E
		PORTD |= 1 << 7; // G
		break;
		case 3:
		PORTB |= 1 << 5; // A
		PORTB |= 1 << 4; // B
		PORTB |= 1 << 3; // C
		PORTB |= 1 << 2; // D
		PORTD |= 1 << 7; // G
		break;
		case 4:
		PORTB |= 1 << 4; // B
		PORTB |= 1 << 3; // C
		PORTB |= 1 << 0; // F
		PORTD |= 1 << 7; // G
		break;
		case 5:
		PORTB |= 1 << 5; // A
		PORTB |= 1 << 3; // C
		PORTB |= 1 << 2; // D
		PORTB |= 1 << 0; // F
		PORTD |= 1 << 7; // G
		break;
		case 6:
		PORTB |= 1 << 5; // A
		PORTB |= 1 << 3; // C
		PORTB |= 1 << 2; // D
		PORTB |= 1 << 1; // E
		PORTB |= 1 << 0; // F
		PORTD |= 1 << 7; // G
		break;
		case 7:
		PORTB |= 1 << 5; // A
		PORTB |= 1 << 4; // B
		PORTB |= 1 << 3; // C
		break;
		case 8:
		PORTB |= 1 << 5; // A
		PORTB |= 1 << 4; // B
		PORTB |= 1 << 3; // C
		PORTB |= 1 << 2; // D
		PORTB |= 1 << 1; // E
		PORTB |= 1 << 0; // F
		PORTD |= 1 << 7; // G
		break;
		case 9:
		PORTB |= 1 << 5; // A
		PORTB |= 1 << 4; // B
		PORTB |= 1 << 3; // C
		PORTB |= 1 << 2; // D
		PORTB |= 1 << 0; // F
		PORTD |= 1 << 7; // G
		break;
	}
}

void turnoff_display(void){
	PORTB &= ~(1 << 5); // A
	PORTB &= ~(1 << 4); // B
	PORTB &= ~(1 << 3); // C
	PORTB &= ~(1 << 2); // D
	PORTB &= ~(1 << 1); // E
	PORTB &= ~(1 << 0); // F
	PORTD &= ~(1 << 7); // G
}

void display_heartbeat(void){

	//display digit 3
	digit3 = (int) bpm%10;
	display_number(digit3);
	PORTD |= 1 << 6;
	_delay_ms(2.5);
	turnoff_display();
	PORTD &= ~(1 << 6);

	//display digit 2
	digit2 = (int) ((bpm)/10)%10;
	display_number(digit2);
	PORTD |= 1 << 5;
	_delay_ms(2.5);
	turnoff_display();
	PORTD &= ~(1 << 5);

	//display digit 1
	digit1 = (int) ((bpm)/100)%10;
	display_number(digit1);
	PORTD |= 1 << 4;
	_delay_ms(2.5);
	turnoff_display();
	PORTD &= ~(1 << 4);
}

void start_timer (void){
	OCR1A = 0x9C; //interrupt will be called after 10.048 ms

	TCCR1B |= (1 << WGM12);
	// Mode 4, CTC on OCR1A

	TIMSK1 |= (1 << OCIE1A);
	//Set interrupt on compare match

	TCCR1B |= (1 << CS12) | (1 << CS10);
	// set prescaler to 1024 and start the timer
}

void init_adc(void){
	//use AVCC for reference voltage, using ADC0
	ADMUX |= (1 << REFS0);

	//enable ADC
	ADCSRA |= (1 << ADEN);

	//enable interrupt
	ADCSRA |= (1 <<  ADIE);

	//Set Prescaler
	ADCSRA |= (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);

	//using 8 bit ADCH only, shift left to ADCH
	ADCSRA |= (1 << ADLAR);

	//disable digital buffer
	DIDR0 |= (1 << ADC0D);

	//start conversion
	start_conversion();
}

void start_conversion(void){
	//start conversion
	ADCSRA |= (1 << ADSC);
}