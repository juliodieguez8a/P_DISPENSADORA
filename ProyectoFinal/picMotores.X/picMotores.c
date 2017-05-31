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

#define _XTAL_FREQ 500000

char puerta=0;
char ADC=0;
char giro=0;
char pos=20; //de 20 a 78
char vel=100; //velocidad
char accion; //accion transmitida

void stepper(void){
    char i=3;
    while (i!=0){
        PORTB=0b0011;
        __delay_ms(200);
        PORTB=0b0110;
        __delay_ms(200);
        PORTB=0b1100;
        __delay_ms(200);
        PORTB=0b1001;
        __delay_ms(200);
        i--;
    }
    
    return;
}

void stepper2(void){
    PORTB=0b0001;
    __delay_ms(500);
    PORTB=0b0011;
    __delay_ms(500);
    PORTB=0b0010;
    __delay_ms(500);
    PORTB=0b0110;
    __delay_ms(500);
    PORTB=0b0100;
    __delay_ms(500);
    PORTB=0b1100;
    __delay_ms(500);
    PORTB=0b1000;
    __delay_ms(500);
    PORTB=0b1001;
    __delay_ms(500);
    return;
}

void move_pwmDC(void){
    CCP1CON=0b00001100;
    CCP1CONbits.DC1B0=(vel)&(0b1);
    CCP1CONbits.DC1B1=(vel>>1)&(0b1);
    CCPR1L=(vel>>2);
    __delay_ms(1000);
    CCP1CON=0;
    PORTCbits.RC2=0;
    return;
}

void servo(void){
    CCP2CONbits.DC2B0=(pos)&(0b1);
    CCP2CONbits.DC2B1=(pos>>1)&(0b1);
    CCPR2L=(pos>>2);
    return;
}

void move_servo(void){
    if (giro==1){
        pos++;
        servo();
        __delay_ms(100);
        if(pos==78){
            giro=0;
            pos=77;
        }
    }
    if (giro==2){
        pos--;
        servo();
        __delay_ms(100);
        if(pos==19){
            giro=0;
            pos=20;
        }
    }
    return;
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
    //CONFIGURANDO OSCILADOR (500kHz)
    OSCCON = 0b00111100;
    
    //ENTRADAS DIGITALES
    ANSEL=0;
    ANSELH=0;
    
    //ENTRADAS Y SALIDAS
    TRISB=0b11110000;
    TRISC=0b11011001;
    TRISD=0;
    
    //POSICION INICIAL
    PORTB=0b1001;
    PORTCbits.RC2=0;
    PORTCbits.RC1=0;
    
    //CONFIGURANDO PWM
    CCP2CON=0b00001111;
    servo();
    //TIMER2
    PR2=155;
    T2CON = 0b00000111;     //PRESCALER 16 Y ENCENDER
    PIR1bits.TMR2IF = 0;    //BORRAR FLAG TIMER2
    
    
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
    
    giro=0;
    
    while(1){
        //MOVER STEPPER
        if (PORTBbits.RB4==1|accion==1){
            while (PORTBbits.RB4==1){
            PORTBbits.RB4==1;
            }
            stepper();
            if (accion==1){accion=0;}
        }
        //MOVER MOTOR DC
        if (PORTBbits.RB7==1|accion==2){
            while (PORTBbits.RB7==1){}
            move_pwmDC();
            if (accion==2){accion=0;}
        }
        //CERRAR PUERTA
        if (PORTBbits.RB5==1| (accion==3&pos==20)){
            while (PORTBbits.RB5==1){}
            giro=1;
            if (accion==3){accion=0;}
        }
        //ABRRIR PUERTA
        if (PORTBbits.RB6==1| (accion==3&pos==77)){
            while (PORTBbits.RB6==1){}
            giro=2;
            if (accion==3){accion=0;}
        }
        //MOVER SERVO
        if (giro==1|giro==2){
            move_servo();
        }
        //RECIBIR ACCIONES
        if (SSPSTATbits.BF==1){
            accion=SSPBUF;
            PORTD=accion;  
        }
        
    }
}
