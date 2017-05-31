//PIC MAESTRO

#include <xc.h>

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

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


char ADC=0;
int cont=0;
char n_mensaje=0;
int dinero=0;
char temperatura=0;


void LCD_t(char c){
    //RB5 ON
    PORTA=c|32;
    __delay_us(5);
    PORTAbits.RA5=0;
    __delay_us(5);
    PORTAbits.RA5=1;
    __delay_us(5);
    return;
}

//ENVIA COMANDOS COMPLETOS
void LCD_t8(int i){
    char rs = (char)(i>>9);
    char com  = i & 0xff;
    
    //PARTE ALTA Y BAJA
    char l=com & 0x0f;
    char h=com>>4;
    
    LCD_t(h+rs*16);
    LCD_t(l+rs*16);
    
    __delay_us(50);    
    return;    
}

//COMANDO DE ESCRITURA CARACTER
void LCD_W(char c){ 
    
    //PARTE ALTA Y BAJA
    char l=c & 0x0f;
    char h=c>>4;
    
    LCD_t(h+16);
    LCD_t(l+16);
    
    __delay_us(50);
    return;
}

//ESCRIBIR MENSAJES EN LCD
void LCD_WM(char c[]){
    char i=0;
    while (c[i]!=0){
        LCD_W(c[i]);
        i++;
    }
    return;
}

//INICIALIZAR LCD
void LCD_init(void){
    
    __delay_ms(10);
    LCD_t(0b00011);
    __delay_ms(2);
    LCD_t(0b00011);
    LCD_t(0b00011);
    __delay_ms(2);
    
    LCD_t(0b00010);
    __delay_ms(2);
    
    //DEFINIR LINEAS Y FUENTE
    LCD_t8(0b0000101000);
    __delay_ms(2);
    
    //VISUALIZADOR APAGADO
    LCD_t8(0b0000001000);
    __delay_ms(2);
    
    //VISUALIZADOR BORRADO
    LCD_t8(0b0000000001);
    __delay_ms(2);
    
    //AJUSTE MODO DE MUESTREO
    LCD_t8(0b0000000110);
    __delay_ms(2);
    
    //ENCENDER CURSOR
    LCD_t8(0b0000001100);
    __delay_ms(2);
    return;
}

//MENSAJES
void check_mensajes(){
    if (n_mensaje==1 & dinero==0){
        n_mensaje=0;
        //BORRAR
        LCD_t8(0b0000000001);
        __delay_ms(2);
        //EN PRIMERA LINEA
        LCD_t8((0b001<<7)|(0x00));
        LCD_WM("Hola!!! Inserte");
        //COLOCAR EN SEGUNDA LINEA
        LCD_t8((0b001<<7)|(0x40));
        LCD_WM("una moneda -->");
    }
    if (n_mensaje==2 & dinero==0){
        n_mensaje=0;
        //BORRAR
        LCD_t8(0b0000000001);
        __delay_ms(2);
        //EN PRIMERA LINEA
        LCD_t8((0b001<<7)|(0x00));
        LCD_WM("Temperatura:");
        //EN SEGUNDA LINEA
        LCD_t8((0b001<<7)|(0x40));
        LCD_WM("--> ");
        char str_temperatura[6];
        sprintf(str_temperatura, "%d", temperatura);
        LCD_WM(str_temperatura);
        LCD_W(0b11011111);
        LCD_W('C');
    }            
    if (n_mensaje==1 & dinero>0){
        n_mensaje=0;
        //BORRAR
        LCD_t8(0b0000000001);
        __delay_ms(2);
        //EN PRIMERA LINEA
        LCD_t8((0b001<<7)|(0x00));
        LCD_WM("Dinero:");
        //EN SEGUNDA LINEA
        LCD_t8((0b001<<7)|(0x40));
        LCD_WM("--> ");
        
        //DESPLEGAR DINERO
        char str_dinero[8];
        char str_dinero2[8];
        sprintf(str_dinero, "%d", dinero);
        sprintf(str_dinero2, "%d", dinero);
        
        char i; //LONGITUD
        for(i = 0; str_dinero[i] != '\0'; ++i);
        if (i==2){str_dinero2[0]='0';str_dinero2[1]='\0';}
        if (i>2){str_dinero2[i-2]='\0';}
        LCD_WM(str_dinero2);
        //DECIMALES
        LCD_W('.');LCD_W(str_dinero[i-2]);LCD_W(str_dinero[i-1]);LCD_W('Q');
    }
    if (n_mensaje==3){
        n_mensaje=0;
        cont=0; //SE DESPLIEGA POR UNOS SEGUNDOS
        //BORRAR
        LCD_t8(0b0000000001);
        __delay_ms(2);
        //EN PRIMERA LINEA
        LCD_t8((0b001<<7)|(0x00));
        LCD_WM("DESPACHO BEBIDA:");
        //COLOCAR EN SEGUNDA LINEA
        LCD_t8((0b001<<7)|(0x40));
        LCD_WM("Tampico");
    }
    if (n_mensaje==4){
        n_mensaje=0;
        cont=0; //SE DESPLIEGA POR UNOS SEGUNDOS
        //BORRAR
        LCD_t8(0b0000000001);
        __delay_ms(2);
        //EN PRIMERA LINEA
        LCD_t8((0b001<<7)|(0x00));
        LCD_WM("DESPACHO:");
        //COLOCAR EN SEGUNDA LINEA
        LCD_t8((0b001<<7)|(0x40));
        LCD_WM("Tortrix");
    }
}

//TRANSMISIONES
void check_transmisiones(){

}

void interrupt ISR(){
    if (INTCONbits.RBIF==1){
        //MONEDAS
        if (PORTBbits.RB0==0){dinero+=25;n_mensaje=1;}
        if (PORTBbits.RB1==0){dinero+=50;n_mensaje=1;}
        if (PORTBbits.RB2==0){dinero+=100;n_mensaje=1;}
        
        //DESPACHO BEBIDA
        if (PORTBbits.RB3==0&dinero>=350){
            dinero-=350;
            PORTE=0b11111110;
            n_mensaje=3;
            SSPBUF=1;
            PORTD=1;
        }
        //DESPACHO CHUCHERIA
        if (PORTBbits.RB4==0&dinero>=200){
            dinero-=200;
            PORTE=0b11111110;
            n_mensaje=4;
            SSPBUF=2;
            PORTD=2;
        }
        INTCONbits.RBIF=0;
    }
    
    if (PIR1bits.ADIF==1){
        PIR1bits.ADIF=0;
        //PORTC=ADRESH;
        ADC=ADRESH;
        __delay_ms(2);
        ADCON0bits.GO=1;
    }
    if (PIR1bits.TMR2IF == 1){
        PIR1bits.TMR2IF =0;
        cont++;    
        if (cont==500){
            n_mensaje=1;
        }
        if (cont==1000){
            n_mensaje=2;
            cont=0;
        }
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
    TRISA=0b11000000;
    TRISB=0b11111111;
    TRISC=0b11010111;
    TRISD=0b00000000;
    TRISE=0b00000000;
    
    PORTD=0;
    PORTE=0b11111111;
    
    LCD_init();
    //EN PRIMERA LINEA
    LCD_t8((0b001<<7)|(0x00));
    LCD_WM("Bienvenido!!!");
    //EN SEGUNDA LINEA
    LCD_t8((0b001<<7)|(0x40));
    LCD_WM("Cmamut");
    
    //INTERRUPCIONES PUERTO B Y PULL UP
    IOCB=0b00011111;
    WPUB=0b00011111;
    INTCONbits.RBIF=0;
    INTCONbits.RBIE=1;
    OPTION_REGbits.nRBPU=0;
    
    //TIMER2
    PR2=155;
    T2CON = 0b01111111;     //PRESCALER 16 Y ENCENDER
    PIR1bits.TMR2IF = 0;    //BORRAR FLAG TIMER2
    PIE1bits.TMR2IE = 1;    //HABILITAR INTERRUPCION
    
    //CONFIGURANDO MSSP MODO SPI
    SSPSTAT=0b01000000;
    //MASTER FOSC/64
    SSPCON=0b00110010;
    
    //HABILITAR INTERRUPCIONES
    INTCONbits.PEIE=1;  //INTERRUPCIONES PERIFERICAS
    INTCONbits.GIE=1;   //INTERRUPCIONES GLOBALES
    INTCONbits.PEIE=1;  //INTERRUPCIONES PERIFERICAS
    
    char dato=0;
    while(1){
        check_mensajes();
    }
}
