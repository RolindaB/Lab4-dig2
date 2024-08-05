/******************************************************************************
Universidad Del Valle De Guatemala
IE2023: Electrónica digital 2
Proyecto: Laboratorio 4
Hardware: Atmega238p
Rolinda Beb 22274, Jose Romero 22171
Última modificación: 4/08/2024
******************************************************************************/
#define F_CPU 16000000UL // Define la frecuencia de tu microcontrolador en Hz
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "LCD/LCD.h"
#include "I2C/I2C.h"

#define SLAVE1_ADDR 0x01 // Dirección del primer esclavo
#define SLAVE2_ADDR 0x02 //Dirección del segundo esclavo

uint8_t adc_value, counter;
char buffer[16];
char buffer1[16];

void ADCI2C(){
	uint8_t result = read_from_slave(SLAVE1_ADDR, &adc_value);

	if (result == 0) {
		// Si la lectura es exitosa
		snprintf(buffer, sizeof(buffer), " %u", adc_value);
		} else {
		// Maneja el error de lectura
		snprintf(buffer, sizeof(buffer), "Error %u", result);
	}
}
void CounterI2C(){
	uint8_t result = read_from_slave(SLAVE2_ADDR, &counter);

	if (result == 0) {
		// Si la lectura es exitosa
		snprintf(buffer1, sizeof(buffer1), " %u", counter);
		} else {
		// Maneja el error de lectura
		snprintf(buffer1, sizeof(buffer1), "Error %u", result);
	}
	
}
int main(void) {
	// Inicializa el LCD
	initLCD8bits(); // Asegúrate de que esta función configure el LCD correctamente
	// Inicializa el bus I2C con 100 kHz y prescaler 1
	I2C_Master_Init(100, 4);

	sei(); // Habilita las interrupciones globales

	while (1) {
		// Lee un byte del esclavo
		ADCI2C();
		CounterI2C();
	//ADC:
		LCD_Set_Cursor(1, 2); // Posiciona el cursor en la primera línea del LCD
		LCD_Write_String("        "); // Limpia la línea
		LCD_Set_Cursor(1, 2); // Posiciona el cursor nuevamente
		LCD_Write_String(buffer); // Muestra el valor o el error
		LCD_Set_Cursor(2, 1); // Posiciona el cursor nuevamente
		LCD_Write_String("ADC:"); // Muestra el valor o el error
	//CONTADOR;
		LCD_Set_Cursor(9, 2); // Posiciona el cursor en la primera línea del LCD
		LCD_Write_String("        "); // Limpia la línea
		LCD_Set_Cursor(9, 2); // Posiciona el cursor nuevamente
		LCD_Write_String(buffer1); // Muestra el valor o el error
		LCD_Set_Cursor(9, 1); // Posiciona el cursor nuevamente
		LCD_Write_String("Counter:"); // Muestra el valor o el error
		
		_delay_ms(450); // Espera antes de la siguiente lectura
	}
}