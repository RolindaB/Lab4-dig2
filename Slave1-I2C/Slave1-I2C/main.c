/******************************************************************************
Universidad Del Valle De Guatemala
IE2023: Electrónica digital 2
Proyecto: Laboratorio 4
Hardware: Atmega238p
Rolinda Beb 22274, Jose Romero 22171
Última modificación: 4/08/2024
******************************************************************************/
#define F_CPU 16000000UL  // Define la frecuencia de tu microcontrolador en Hz
#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "SLAVE-I2C/SLAVEI2C.h"

#define SLAVE_ADDR 0x01  // Dirección del esclavo

volatile uint8_t adc_value; // Valor predeterminado para pruebas
volatile uint8_t dato = 0; // Variable global para almacenar datos recibidos


// Rutina de interrupción del ADC
ISR(ADC_vect) {
	ADC2();
	adc_value = ADC;
	habilitar_conversion();
}
// Rutina de interrupción del TWI (I2C)
ISR(TWI_vect) {
	uint8_t estado = TWSR & 0xF8; // Lee los 3 bits superiores del registro de estado

	switch (estado) {
		case 0x60: // SLA+W recibido, ACK enviado
		case 0x70: // SLA+W recibido en modo general, ACK enviado
		TWCR |= (1 << TWINT); // Borra el flag TWINT para continuar
		break;

		case 0x80: // Datos recibidos, ACK enviado
		case 0x90: // Datos recibidos en llamada general, ACK enviado
		dato = TWDR; // Lee el dato recibido del registro de datos
		TWCR |= (1 << TWINT); // Borra el flag TWINT para continuar
		break;

		case 0xA8: // SLA+R recibido, ACK enviado
		case 0xB8: // Dato transmitido y ACK recibido
		TWDR = adc_value; // Carga el valor del ADC en el registro de datos
		TWCR |= (1 << TWINT) | (1 << TWEA); // Borra el flag TWINT y habilita ACK para el próximo byte
		break;

		case 0xC0: // Dato transmitido y NACK recibido
		case 0xC8: // Último dato transmitido y ACK recibido
		TWCR |= (1 << TWINT) | (1 << TWEA); // Borra el flag TWINT y habilita ACK para el próximo byte
		break;

		default: // Manejo de errores
		TWCR |= (1 << TWINT) | (1 << TWSTO); // Borra el flag TWINT y envía una condición de STOP
		break;
	}
}

int main(void) {
	I2C_Slave_Init(SLAVE_ADDR); // Inicializa el esclavo I2C
	ADC_init(128);
	ADC2();
	habilitar_conversion();
	sei(); // Habilita interrupciones globales

	while (1) {
		_delay_ms(100); // Delay para evitar lecturas demasiado rápidas
		// En este caso, no estamos usando ADC, así que el valor es constante
	}
}
