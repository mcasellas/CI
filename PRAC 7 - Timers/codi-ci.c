/*
 * Grup G - CI 2017
 * Practica 7 - Timers
 * Marc Casellas, Carlos Rodriguez, Martí Fernandez
 * Created: 7-11-17
 * Processor: PIC18F4550
 * Compiler:  MPLAB XC8
 */

#include <xc.h>
#include <string.h>
#include "config.h"
#include "GLCD.h"


#define _XTAL_FREQ 8000000 // Needed for __delay_ms function

// Estats del timer
#define ESTAT_INIT 0
#define ESTAT_RUNNING 1
#define ESTAT_STOPPED 2

// Defines
#define BOTO PORTAbits.RA0 // Botó RA0

#define GIEH INTCONbits.GIEH

#define TMR0ON T0CONbits.TMR0ON
#define T0CS T0CONbits.T0CS
#define PSA T0CONbits.PSA
#define T08BIT T0CONbits.T08BIT
#define T0PS0 T0CONbits.T0PS0
#define T0PS1 T0CONbits.T0PS1
#define T0PS2 T0CONbits.T0PS2

#define TMR0IE INTCONbits.TMR0IE
#define TMR0IF INTCONbits.TMR0IF
#define TMR0IP INTCON2bits.TMR0IP

#define IPEN RCONbits.IPEN



// Variables globals (decimes, estat del crono, etc.)
unsigned int decimes = 0;
unsigned int segons = 0;
unsigned int minuts = 0;

int calcul_preescaler = 65535 - 3125;

unsigned char estatCrono = ESTAT_INIT;  // initial state

// Valors constants d'info
const char * info1 = "CRONOMETRE\n";
const char * info2 = "mm:ss.d\n";

// RSI High Priority for handling Timer0
void interrupt rsi_timer(void) {
	if(TMR0IE && TMR0IF) {
		tic();
		TMR0 = calcul_preescaler;
		TMRIE = 0;
	}
}

// Comptador
void tic() {
	++decimes;

	if (decimes >= 10){ // Ha passat 1 segon
		decimes = 0;
		++segons;
		if (segons >= 60){ // Ha passat 1 minut
			segons = 0;
			++minuts;
		}
	}
}

// Reset del contador
void reset_cont() {
	decimes = 0;
	segons = 0
	minuts = 0;
}

// Funció imprimir
void writeTxt(byte page, byte y, char * s) {
	int i = 0;
	while (*s != '\n') {
		putch(page, y+i, *(s++));
		i++;
	}
}

void actualitzar(void) {

	writeTxt(0,8, info1);
	writeTxt(2,9, info2);

	int linia = 3;
	int car = 9;

	putch(linia, car++, minuts / 10 + '0');
	putch(linia, car++, minuts % 10 + '0');

	putch(linia, car++, ':');

	putch(linia, car++, segons / 10 + '0');
	putch(linia, car++, segons % 10 + '0');

	putch(linia, car++, '.');

	putch(linia, car, decimes % 10 + '0');
}

// Detecció de botó apretat
char lecturaFlancRA0() {
	if(BOTO == 1) {
		__delay_ms(10);
		return '1';
	}
	return '0';
}

// Initialize PORTs and basic PIC resources
void InitPIC() {
  ADCON1 = 0x0F;

	TRISA = 0xFF; // Entrada
	TRISD = 0x00; // Sortida

	//Desactivem les interrupcions
	GIEL = 0;

	PSA = 0; //Volem prescaler

	// Valors del prescaler
	T0PS0 = 0;
	T0PS1 = 0;
	T0PS2 = 1;

	T0CS = 0; // FOSC/4
	T08BIT = 0; // timer de 8 bits

	TMR0IF = 0; // Restablim la IF
	TMR0IE = 0; // Enable
  TMR0IP = 1; // Alta prioritat
	IPEN = 1; // Permetem les interrupcions de prioritat
	GIEH = 1; // Permetem interrupcions de high

}




void main(void) {

  InitPIC();
  GLCDinit(); // GLCD routines are in rutines_GLCD.C
  clearGLCD(0, 7, 0, 127);
  setStartLine(0);

	// Inicialitzem els elements de la pantalla
	actualitzar();

  // MAIN LOOP
  while (1) {
    if (lecturaFlancRA0()) { // Quan s'apreta un botó

      switch(estatCrono) {
        case ESTAT_INIT: // de ESTAT_INIT a ESTAT_RUNNING
					TMR0 = calcul_preescaler;
					TMR0ON = 1; // Encenem el timer
					estatCrono = ESTAT_RUNNING;

				break;

        case ESTAT_RUNNING: // de ESTAT_RUNNING a ESTAT_STOPPED
					TMR0ON = 0; // Apaguem el timer
					estatCrono = ESTAT_STOPPED;
				break;

        case ESTAT_STOPPED: // de ESTAT_STOPPED a ESTAT_INIT
					reset_cont();
					estatCrono = ESTAT_INIT;
				break;

				default: break;
      }

			while (BOTO == 1) actualitzar();
    }

		actualitzar();
  }
}
