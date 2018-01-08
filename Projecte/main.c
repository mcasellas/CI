/* Main.c
 * Grup G: Mart� Fern�ndez, Carlos Rodr�guez i Marc Casellas
 * Creat:   13 de novembre del 2017
 * Processor: PIC18F4550
 * Compiler:  MPLAB XC8
 */

#include <xc.h>
#define _XTAL_FREQ 8000000
#define PINA0 PORTAbits.RA0
#define PINA1 PORTAbits.RA1
#define PINA2 PORTAbits.RA2
#define PINA3 PORTAbits.RA3
#include <string.h>
#include "config.h"
#include "GLCD.h"

const char * pacman = "*";
const char * fantasmito = "+";
const char * GAMEOVER = "GAME OVER !\n";


byte X = 3;
byte Y = 12;
byte fx = 3;
byte fy = 3;
byte fx2 = 3;
byte fy2 = 20;

byte aX = 0;
byte aY = 0;
byte afx = 0;
byte afy = 0;
byte afx2 = 0;
byte afy2 = 0;

int contador = 0;
int imprimeix = 0;
int direccio = 4; // 4 = null, 1 = esquerra, 0 = dalt, 3 = baix, 2 = dreta
int cal_moure = 0;
int moure_fantasmes = 0;


void escriure_pantalla(){
    clearGLCD(0,7,0,127); // Neteja pantalla
    // X = vertical Y = horitzontal
    putch(X, Y, '*');  // Pacman
    putch(fx, fy, '+');  // Fantasma 1
    putch(fx2, fy2, '+');  // Fantasma 2
}

void writeTxt(byte page, byte y, char * s) {
	int i=0;
	while (*s! = '\n') {
		putch(page, y+i, *(s++));
		i++;
	};
}

void interrupt timer2(void) {
  if (PIR1bits.TMR2IF) {
    ++contador; // Ha passat 1.9ms
    // Cada 1.9 * 200 ms fa soroll i mou el bitxo
    if (contador == 200) {

      // Activa o desactiva el soroll
      if (TRISCbits.RC2) TRISCbits.RC2 = 0;
      else TRISCbits.RC2 = 1;
      // 4 = null, 1 = esquerra, 0 = dalt, 3 = baix, 2 = dreta
      cal_moure = 1;
      direccio = 4;
      if(PINA0 == 1) direccio = 0;
      if(PINA1 == 1) direccio = 1;
      if(PINA2 == 1) direccio = 2;
      if(PINA3 == 1) direccio = 3;
      if (direccio == 4) cal_moure = 0;

    }
    if (contador == 400) {
      contador = 0;
      moure_fantasmes = 1;
    }
  }
  PIR1bits.TMR2IF = 0; // Posem el flag a 0
}

void morir(){
  TRISCbits.RC2 = 0;
  clearGLCD(0,7,0,127);
  // Treu Game Over Per Pantalla
  writeTxt(3,3, GAMEOVER);
  __delay_ms(3000);
  asm("reset");
}


void main(void) {
{
	ADCON1 = 0x0F;   // Configurat com a digital
	// Valors
	PORTA = 0x00;
  PORTD = 0x00;    //Donem uns valors inicials als ports
	PORTB = 0x00;
	// TRIS
  TRISA = 0xFF;
	TRISD = 0x00;		 //Configurem D i B de sortida
	TRISB = 0x00;
	TRISCbits.RC2 = 0; // Pin RC2-> Sortida

	// Set pantalla
	GLCDinit();		   //Inicialitzem la pantalla
	clearGLCD(0,7,0,127);      //Borrem pantalla
	setStartLine(0);           //Definim inici
  // Interruptions
  INTCONbits.GIE = 1; // General Intertupt Enable
  PIE1bits.TMR2IE = 1; // Activem interrupcions del TMR2
  INTCONbits.GIEL = 1; // Permetem diferents prioritats (PEIE)
  T2CONbits.T2CKPS1 = 1; // Preescaler = 4
  PR2 = 239; // PWM period
  // Configure the CCPx module for PWM operation
  CCP1CONbits.CCP1M3 = 1;
  CCP1CONbits.CCP1M2 = 1;
  //  PWM duty cycle = 0
  CCPR1L = 120;
  CCP1CONbits.DC1B1 = 0;
  CCP1CONbits.DC1B0 = 0;
  T2CONbits.TMR2ON = 1; // Activem el timer

  // Comen�em escriure pantalla
  escriure_pantalla();
}

while (1){
    int cal_imprimir = 0;
    // Movem el pacman
    if (cal_moure) {
      if(direccio==0){ // pujar
        aX = X;
        if(X>1) --X;
      }

      else if(direccio==1){ // esquerra
        aY = Y;
        if(Y > 1) --Y;
      }

      else if(direccio==3){ // baixar
        aX = X;
        if(X < 24) ++X;
      }

      else if(direccio==2){ // dreta
        aY = Y;
        if(Y < 7) ++Y;
      }

      cal_moure = 0;
      cal_imprimir = 1;
    }

    if (moure_fantasmes){
      // Mou el fantasmito 1 : Comportament 1
      if (Y < fy) {
        afy = fy;
        --fy;
      }
      else if(Y > fy) {
        afy = fy;
        ++fy;
      }
      else if (X < fx) {
        afx = fx;
        --fx;
      }
      else if (X > fx) {
        afx = fx;
        ++fx;
      }
      else morir();

      // Mou el fantasmito 2 : Comportament 2
      if (Y < fy2) {
        afy = fy2;
        --fy2;
      }
      else if(Y > fy2) {
        afy = fy2;
        ++fy2;
      }
      else if (X < fx2) {
        afx = fx2;
        --fx2;
      }
      else if (X > fx2) {
        afx = fx2;
        ++fx2;
      }
      else morir();

      moure_fantasmes = 0;
      cal_imprimir = 1;
    }

    if (cal_imprimir){
      escriure_pantalla();
      cal_imprimir = 0;
    }
  }
}
