/*
 * File:   sensor ultrasonico y sensor de fuerza.c
 * Author: Admin
 *
 * Created on 18 de mayo de 2017, 11:48 AM
 */
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
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

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include <stdarg.h>

#define _XTAL_FREQ 8000000      //Frecuencia 
#define Trigger PORTAbits.RA0   //Salida
#define Echo PORTAbits.RA1      //Entrada
#define LED_Azul PORTAbits.RA3  //Salida
#define LED_Verde PORTAbits.RA4  //Salida
#define LED_Amarillo PORTAbits.RA5  //Salida
#define PIN_Temperatura PORTBbits.RB0  //Entrada

//!!!!!!!!!!!!!!!!!!!!Copiar la varialble !!!!!!!!!!!!!!!!!!
char presion_zero = 0;          //variable alamacena el valor cuando no se tiene nada. ES necesaria

char medicion_ultrasonico(void);
char medicion_temperatura(void);
void calibracion_presion(void);
char medicion_presion(void);


void interrupt ISR(){
    
    
}


void main(void){
    OSCCONbits.HTS = 1;         //ES ESTABLE
    OSCCONbits.IRCF0 = 1;
    OSCCONbits.IRCF1 = 1;
    OSCCONbits.IRCF2 = 1;       //Fosc = 8Mhz   
    OSCCONbits.OSTS = 0;        //RELOJ INTERNO
    OSCCONbits.SCS = 1;         //RELOJ INTERNO
     
    TRISA=0B00000010;       
    TRISB=0B00000001;       
    TRISD=0B00000000;
    
    ANSEL=0;
    ANSELH=0B00000001;

    PORTD=0;
    PORTA=0;
    PORTB=0;
    
    ADCON1bits.VCFG1 = 0;       //referencia a VSS
    ADCON1bits.VCFG0 = 0;       //Referencia a VDD
    ADCON1bits.ADFM = 0;        //Justificado a la izquierda
    PIE1bits.ADIE = 1;          //ADC Interrup enable
    ANSELHbits.ANS12 = 1;       //AN12 RB0 como analogica,
    ADCON0bits.ADCS1 = 1;       //FOSC/32 (ADC CLOCK PERIOD, TAD, ESTABLE PARA 8MHZ)
    ADCON0bits.ADCS0 = 0;       //FUENTE DE RELOJ
    ADCON0bits.CHS3 = 1;         //Canal AN12-RB0
    ADCON0bits.CHS2 = 1;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS0 = 0;
    ADRESL = 0;                 //Borramos registros
    ADRESH = 0;
    ADCON0bits.ADON = 1;        //ADC On, Encendido
    __delay_ms(1);              //Tiempo para cambiar canal
       
    //medicion_ultrasonico();
    //medicion_temperatura();
    //calibracion_presion();
    //presion_zero=42;            //
    //medicion_presion();         //
    while (1){
       
    }
    
}


char medicion_ultrasonico(void){
    unsigned int tiempo=0; 
    char objeto = 0;
    Trigger = 0;                //Ponemos 0 el trigger del ultrasonico y esperamos quese estabilice
    __delay_us(10);             
    Trigger = 1;                //Mandamos un pulso de 10us para iniciar la medicion 
    __delay_us(10);
    Trigger = 0; 
    while (1){                  //Esperamos a que el pin Echo comomienze a trasmitir
        tiempo=0;
        if (Echo==1){           
            while (Echo==1){    //Se cuenta los us que permanece en high
                __delay_us(1);
                tiempo=tiempo+1;
            }
            tiempo=tiempo/11;   //Se convierte a centimetro
            if (tiempo<=75){    //Si esta un objeto en menos de 75 cm se enciende la luz
                objeto=1;
            }
            else {
                objeto=0;
            }
            __delay_ms(1000);   //
            return objeto;
        }
    }
}

char medicion_temperatura(void){
    char numero=0;              //variable
    ADCON0bits.GO=1;            //se activa la conversion analogica
    while(PIR1bits.ADIF==0){    //se espera a que termine la conversion
        __delay_us(1);
    }
    PIR1bits.ADIF=0;
    numero=ADRESH;              //guardamos el valor   
    numero=numero-115;          //la variable nuemro ya tiene el valor en grados    
    return numero;
}

char medicion_presion(void) {
    char numero=0;              //variable
    char objeto=0; 
    ADCON0bits.GO=1;            //se activa la conversion analogica
    while(PIR1bits.ADIF==0){    //se espera a que termine la conversion
        __delay_us(1);
    }
    PIR1bits.ADIF=0;
    numero=ADRESH;              //guardamos el valor   
    if (numero > (presion_zero+3)){ //se compara de que haya algo
        objeto = 1;       //indicador que ya hay algo
    }
    return objeto;
}

void calibracion_presion(void) {    //se utiliza para calibrar cuando se enciende la maquina, necesita de la variable colocada afuera
    char numero=0;              
    ADCON0bits.GO=1;            //se activa la conversion analogica
    while(PIR1bits.ADIF==0){    //se espera a que termine la conversion
        __delay_us(1);
    }
    PIR1bits.ADIF=0;
    numero=ADRESH;              //guardamos el valor   
    presion_zero=numero;               //se envia la medicion
    PORTD=numero;
    return;                     //no devuelve nada, solo guarda el valor en la variable global
}




