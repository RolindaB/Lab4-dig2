/******************************************************************************
Universidad Del Valle De Guatemala
IE2023: Electrónica digital 2
Proyecto: Laboratorio 4
Hardware: Atmega238p
Rolinda Beb 22274, Jose Romero 22171
Última modificación: 4/08/2024
******************************************************************************/
// LIBRERIAS
#define F_CPU 16000000
#include <avr/io.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "I2C/I2C.h"

// DEFINICIONES DE PINES
#define BUTTON1_PIN PIND4
#define BUTTON2_PIN PIND6
#define LED1_PIN PINC0
#define LED2_PIN PINC1
#define LED3_PIN PINC2
#define LED4_PIN PINC3
#define SLAVE_ADDR 0x02  // Dirección del esclavo

// VARIABLES
volatile uint8_t CONTADOR = 0;
volatile uint8_t dato = 0; // Variable global para almacenar datos recibidos
volatile uint8_t last_contador = 0; // Último valor del contador para comparación

// VARIABLES DE ANTI-REBOTE
volatile uint8_t button1_state = 0;
volatile uint8_t button2_state = 0;
volatile uint8_t debounce_counter = 0;

// PROTOTIPOS
void initPCint1(void);
void initTimer1(void);

// ISR de TWI (I2C)
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
		TWDR = CONTADOR; // Carga el valor del contador en el registro de datos
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

/**********************************************************************************************************/
void initPCint1(void) {
	// INTERRUPCIONES EN LOS BOTONES BUTTON1_PIN, BUTTON2_PIN
	PCMSK2 |= (1 << PCINT20) | (1 << PCINT22); // HABILITANDO PCINT EN LOS PINES BUTTON1_PIN Y BUTTON2_PIN
	PCICR |= (1 << PCIE2); // HABILITANDO LA ISR PCINT[7:0]
}

/**********************************************************************************************************/
void initTimer1(void) {
	// Configura Timer1 para generar una interrupción cada 1 ms
	TCCR1A = 0; // Modo normal
	TCCR1B = (1 << WGM12) | (1 << CS11); // Modo CTC y prescaler de 8
	OCR1A = 1999; // Valor para 1 ms (16MHz / 8 / 2000)
	TIMSK1 = (1 << OCIE1A); // Habilitar la interrupción de comparación
}

/**********************************************************************************************************/
ISR(PCINT2_vect) {
	// Cambia el estado de los botones para manejar el anti-rebote
	debounce_counter = 1;
}

/**********************************************************************************************************/
ISR(TIMER1_COMPA_vect) {
	// Temporizador para manejar anti-rebote
	if (debounce_counter) {
		debounce_counter++;
		if (debounce_counter >= 20) { // Esperar 20 ms para estabilización
			debounce_counter = 0;

			// Leer el estado actual de los botones
			uint8_t button1_current = PIND & (1 << BUTTON1_PIN);
			uint8_t button2_current = PIND & (1 << BUTTON2_PIN);

			// Verificar el estado del botón 1
			if (!(button1_current) && button1_state == 0) {
				CONTADOR++; // Incrementa el contador
				if (CONTADOR > 15) {
					CONTADOR = 15; // Limita el contador a 15 (4 LEDs)
				}
				button1_state = 1; // Actualiza el estado del botón 1
			}
			if (button1_current) {
				button1_state = 0; // Restablece el estado del botón 1
			}

			// Verificar el estado del botón 2
			if (!(button2_current) && button2_state == 0) {
				CONTADOR--; // Decrementa el contador
				if (CONTADOR > 15) {
					CONTADOR = 0; // Limita el contador a 0
				}
				button2_state = 1; // Actualiza el estado del botón 2
			}
			if (button2_current) {
				button2_state = 0; // Restablece el estado del botón 2
			}

			// Actualizar el puerto solo si el valor del contador ha cambiado
			if (CONTADOR != last_contador) {
				PORTC = CONTADOR; // Actualiza el puerto C con el nuevo valor
				last_contador = CONTADOR; // Actualiza el último valor del contador
			}
		}
	}
}

/**********************************************************************************************************/
int main(void) {
	cli(); // DESHABILITAR INTERRUPCIONES

	// CONFIGURACIÓN DE BOTONES
	DDRD &= ~((1 << BUTTON1_PIN) | (1 << BUTTON2_PIN)); // PINES BUTTON1_PIN Y BUTTON2_PIN COMO ENTRADAS
	PORTD |= (1 << BUTTON1_PIN) | (1 << BUTTON2_PIN); // PULLUPS

	// CONFIGURACIÓN DE LEDS
	DDRC |= (1 << LED1_PIN) | (1 << LED2_PIN) | (1 << LED3_PIN) | (1 << LED4_PIN); // PONER LED1_PIN, LED2_PIN, LED3_PIN Y LED4_PIN COMO SALIDAS
	PORTC = 0; // Apagar todos los LEDs

	I2C_Slave_Init(SLAVE_ADDR); // Inicializar el módulo I2C en modo esclavo
	initPCint1(); // Inicializar las interrupciones de pines
	initTimer1(); // Inicializar Timer1
	sei(); // HABILITAR INTERRUPCIONES
	
	while (1) {
		// El bucle principal no realiza ninguna acción
	}
}
