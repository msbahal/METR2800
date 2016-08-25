/*
 * KillerSatellite.c
 *
 * Created: 21/08/2016 2:16:47 PM
 * Author : s4318365
 */ 
#define F_CPU 8000000UL // 8MHz

#include <util/delay.h> // delay library
#include <avr/io.h> // input output library
#include <stdint.h> 
#include <avr/interrupt.h> // interrupt library
#include <stdbool.h> // true/false accepted

#define ADC_IR1_PIN		0
#define LAZER_PIN		PB2
#define ADC_THRES		350

// initial function definition
uint16_t adc_val(uint8_t adcx);
void laserFire(void);
void laserFireInit ( void);
void checkIR(void);
void motorFwd(void);
void motorReverse(void);

volatile uint16_t senValue1 = 0; // global variable

int extraTime = 0; // variable


// main function that runs the loop 
int main(void)
{
	DDRB = 0xFF; // B registers as output
	DDRC = 0xF0; // C register as half input and half output
	laserFireInit(); // initialise the system by two quick laser fires
	TCCR0A = (1 << WGM01); //Set CTC Bit - clear on timer compare
	OCR0A = 78; // correspons to a timer interrupt every ms
	TIMSK0 = (1 << OCIE0A); // activate interrupt function 
	sei(); // set all intrrupts
	PORTB = 0x00; // B registers for input
	TCCR0B = (1 << CS02) | (1 << CS00); //start at 1024 prescalar
	
	laserFire(); // two second laser fire

	//IR sensing ADC
	ADCSRA |= _BV(ADEN); // start ADC

	while(1){
		//val = PIND & _BV(PD0);

		// ******************IR Sensing ciruit*****************************
		checkIR(); // check for IR value

		while (PIND & _BV(PD0)){
			checkIR();

			// if both buttons are pressed
			if (PIND & _BV(PD2)){
				PORTB &= 0b11100111; // turn both motor lights off
				PORTB |= 0b00001000; // turn the appropriate motor direction light on
				motorFwd();
			}
			else { // if only the top button is pressed
				PORTB &= 0b11100111;
				PORTB |= 0b00010000;
				motorReverse();
			}
			
		}
		
		PORTB &= 0b11100111; // turn the motor lights off
		
	}
		
		//TODO:: Please write your application code
	
}

// this function turns the motor in one direction
void motorFwd(void){
	PORTC |= 0b00010000;
	_delay_ms(20);
	PORTC &= 0b11101111;
	_delay_ms(20);
}

//this funciton turns the motor in the opposite direction from the 
//motorFwd function
void motorReverse(void){
	PORTC |= 0b00100000;
	_delay_ms(20);
	PORTC &= 0b11011111;
	_delay_ms(20);
}


// check if the IR value is met and se the laser to come on
void checkIR(void) {
	if (senValue1 > ADC_THRES){
		PORTB |= 0b00000100;
	}
	else {
		PORTB &= 0b11111011;
	}
}

// TIMER0 interrupt funciton. This function is executed everytime the 
//OCCR0A register is matched
ISR(TIMER0_COMPA_vect)
{
	extraTime++;
	//only execute the function every 20 ms (whatever the value is below)
	if(extraTime > 20)
	{
		extraTime = 0;
		PORTB ^= (1 << PORTB1);

		senValue1 = adc_val(ADC_IR1_PIN); // get sensor value from ADC pin 

	}
}
	
    

uint16_t adc_val(uint8_t adcx){
		ADMUX &= 0xf0; //select ADC 0 by setting all MUX to 0
		ADMUX |= adcx;
		
		ADCSRA |= _BV(ADSC); // start conversion
		
		while ( (ADCSRA & _BV(ADSC)) ); // wait until conversion is finished
										// can change for an interrupt
		
		return ADC;
}


// This function sets the laser on for two seconds
void laserFire ( void)
{
	// initial test- turn laser on twice to indicate start of program
	PORTB = 0b00000100;
	_delay_ms(2000); // wait 2000 milliseconds

	PORTB = 0b00000000;
	_delay_ms(100); // wait 1000 milliseconds

}
	

void laserFireInit ( void)
{
	// initial test- turn lazer on twice to indicate start of program
	PORTB = 0b00000100;
	_delay_ms(500); // wait 500 milliseconds

	PORTB = 0b00000000;
	_delay_ms(500); // wait 500 milliseconds

	PORTB = 0b00000100;
	_delay_ms(500); // wait 500 milliseconds

	PORTB = 0b00000000;
	_delay_ms(500); // wait 500 milliseconds

}