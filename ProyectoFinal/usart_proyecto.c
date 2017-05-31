/*
 * File:   usart_proyecto.c
 * Author: boggd
 *
 * Created on 30 de mayo de 2017, 07:47 PM
 */

// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)


#include <xc.h>



void enviar_byte(unsigned char dato)
{
 while (TXSTAbits.TRMT==0);   // wait until TX buffer is empty
 TXREG = dato;
 return;
}

void main(void) {

char valor_RECIBIDO;

//CONFIGURANDO OSCILADOR (8MHz)
OSCCON = 0b01111100;
    
ANSEL = 0;
ANSELH = 0;                     //DEFINO ENTRADAS DIGITALES

TRISD = 0;
TRISB = 1;
PORTB = 0;
PORTD = 0;

TXSTAbits.SYNC = 0;
TXSTAbits.BRGH = 0;
TXSTAbits.TXEN = 1;  // HABILITO TRANSMISION
/*TXSTA = 0x00;  // Clear TX status
  TXSTA.SYNC=0;  // Async mode
  TXSTA.TXEN=1;  // Enable TX
  TXSTA.TRMT=1;  // TRS empty  (we start with an empty TX register)*/


RCSTAbits.SPEN = 1;
RCSTAbits.RX9 = 0;
RCSTAbits.CREN = 1;  // HABILITO RECEPCION
/*RCSTA =0x00;  // Clear RC status
  RCSTA.SPEN=1; // Enable Serial Port
  RCSTA.CREN=1; // Enable Receiver*/

BAUDCTLbits.BRG16 = 0; 
SPBRG = 12; // BAUDRATE DE 9600
SPBRGH = 0;
 // BRGH=0 -> Baud Rate = Fosc / (64 * (SPBRG+1))
 // BRGH=1 -> Baud Rate = Fosc / (16 * (SPBRG+1))
 
while(1){

    if(PIR1bits.RCIF == 1){
       valor_RECIBIDO = RCREG;
       PORTD = valor_RECIBIDO;
       PIR1bits.RCIF = 0;
    }
    
    
    if(valor_RECIBIDO == 0){
        PORTBbits.RB0 = 1;
        enviar_byte(24);
    }
    if(valor_RECIBIDO == 1){
        PORTBbits.RB1 = 1;
        enviar_byte(8);
    }
    if(valor_RECIBIDO == 2){
        PORTBbits.RB2 = 1;
        enviar_byte(2);
    }
    if(valor_RECIBIDO == 3){
        PORTBbits.RB3 = 1;
        enviar_byte(48);
    }
    if(valor_RECIBIDO == 4){
        PORTBbits.RB4 = 1;
        enviar_byte(64);
    }
    if(valor_RECIBIDO == 5){
        PORTBbits.RB5 = 1;
        enviar_byte(100);
    }
    
}


   
}
