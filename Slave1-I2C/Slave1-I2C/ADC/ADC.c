#include "ADC.h"
#include <stdint.h>
#include <avr/io.h>


void ADC_init(uint8_t prescaler){
	ADMUX |= (1<<REFS0);	// VREF = AVCC
	ADMUX &= ~(1<<REFS1);	// VREF = AVCC
	ADMUX &= ~(1<<ADLAR);	// 10bits
	// Habilita el ADC y la interrupción ADC
	ADCSRA |= (1 << ADEN) | (1 << ADIE);
	
	if (prescaler == 2){
		ADCSRA |= (1<<ADPS0);
		} else if (prescaler == 4){
		ADCSRA |= (1<<ADPS1) ;
		}else if (prescaler == 8){
		ADCSRA |= (1<<ADPS1) | (1<<ADPS0);
		}else if (prescaler == 16){
		ADCSRA |= (1<<ADPS2);
		}else if (prescaler == 32){
		ADCSRA |= (1<<ADPS2) | (1<<ADPS0);
		}else if (prescaler == 128){
		ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
		}else{
		ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	}
	
}

void ADC0(void){
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
}

void ADC1(void){
	//ADC1
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
	ADMUX |= (1<<MUX0);
}
void ADC2(void){
	//ADC2
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
	ADMUX |= (1<<MUX1);
}
void ADC3(void){
	//ADC2
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
	ADMUX |= (1<<MUX1) | (1<<MUX0) ;
}
void ADC4(void){
	//ADC4
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
	ADMUX |= (1<<MUX2);
}
void ADC5(void){
	//ADC5
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
	ADMUX |= (1<<MUX2) | (1<<MUX0);
	
}
void ADC6(void){
	//ADC5
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
	ADMUX |= (1<<MUX2) | (1<<MUX1) ;
	
}
void ADC7(void){
	//ADC7
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3) | (1<<MUX0));
	ADMUX |= (1<<MUX2) | (1<<MUX1) | (1<<MUX0);
}

void habilitar_conversion(void){
	ADCSRA |= (1<<ADSC);//HABILITAR CONVERSION
}