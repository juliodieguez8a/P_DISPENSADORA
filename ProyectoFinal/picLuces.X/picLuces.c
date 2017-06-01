//PIC MOTORES
#include <xc.h>

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

#define _XTAL_FREQ 8000000
#define Trigger PORTAbits.RA0   //Salida
#define Echo PORTAbits.RA1      //Entrada


char accion; //accion transmitida
char ADC;
char leds=0b111;
int cont;


char ColorLed(char binario){
    if((binario&0b1)==1){
        PORTAbits.RA2=1;
    }
    else {PORTAbits.RA2=0;}
    
    if((binario&0b10)==2){
        PORTAbits.RA3=1;
    }
    else {PORTAbits.RA3=0;}
    
    if((binario&0b100)==4){
        PORTAbits.RA4=1;
    }
    else{PORTAbits.RA4=0;}
    return;
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
                ColorLed(leds);
                cont=0;
                //TMR2ON=1;   
            }
            else {
                cont++;
                if (cont>500){
                    objeto=0;
                    ColorLed(0b0000);
                    cont=0;
                }
            }
            return objeto;
        }
    }
}

void interrupt ISR(){
    if (PIR1bits.ADIF==1){
        PIR1bits.ADIF=0;
        //PORTC=ADRESH;
        ADC=ADRESH;
        __delay_ms(2);
        ADCON0bits.GO=1;
    }
    return;
}

void main(void) {
    //CONFIGURANDO OSCILADOR (8MHz)
    OSCCON = 0b01111100;
    
    //ENTRADAS DIGITALES
    ANSEL=0;
    ANSELH=0;
    
    //ENTRADAS Y SALIDAS
    TRISB=0b11110000;
    TRISC=0b11011001;
    TRISA=0b11100010;
    TRISD=0;
    
    ColorLed(leds);

    //CONFIGURANDO MSSP MODO SPI ESCLAVO
    SSPSTAT=0b01000000;
    //ESCLAVO
    SSPCON=0b00110100;
    
    //CONFIGURANDO ADC
    /*
    ADCON1bits.VCFG1=0;     //REF VSS
    ADCON1bits.VCFG0=0;     //REF VDD
    ADCON1bits.ADFM=0;      //JUSTIFICADO IZQUIERDA
    PIE1bits.ADIE=1;        //HABILITAR INTERRUPCION
    
    TRISAbits.TRISA0=1;
    ANSELbits.ANS0=1;       //RA0 ANALOGICA
    
    ADCON0bits.ADCS=0b10;    //FOSC 1/32
    ADCON0bits.CHS=0;
    
    ADRESL=0;
    ADRESH=0;   //PRIMERA LECTURA 0
    ADCON0bits.ADON=1;    //ENCENDEMOS
    */
    //HABILITAR INTERRUPCIONES
    //INTCONbits.GIE=1;   //INTERRUPCIONES GLOBALES
    //INTCONbits.PEIE=1;  //INTERRUPCIONES PERIFERICAS
    
    
    while(1){
        __delay_ms(10);
        medicion_ultrasonico(); 
        //RECIBIR ACCIONES
        if (SSPSTATbits.BF==1){
            accion=SSPBUF;
            PORTD=accion;
            leds=accion;
            ColorLed(leds);
        }
        if (accion==3){
            while(BF==0){}
            //ESPERA SIGUIENTE DATO
            leds=SSPBUF;
        }
        
    }
}
