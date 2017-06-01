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
char vel=200; //velocidad
char accion; //accion transmitida
char presion_zero = 0; //alamacena valor cuando no hay objetos por entregar
char presion=0;

unsigned char medicion_presion(void) {
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
    if (numero < (presion_zero-6)){
        objeto=2;
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
    presion_zero=numero;        //se envia la medicion
    //PORTD=numero;
    return;                     //no devuelve nada, solo guarda el valor en la variable global
}

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

void move_pwmDC(void){
    CCP1CON=0b00001100;
    CCP1CONbits.DC1B0=(vel)&(0b1);
    CCP1CONbits.DC1B1=(vel>>1)&(0b1);
    CCPR1L=(vel>>2);
    while (medicion_presion()!=1){ //mientras no haya objeto, gire hasta que caiga
        __delay_ms(50);
    }
    CCP1CON=0;
    PORTCbits.RC2=0;
    return;
}



void interrupt ISR(){
    /*if (PIR1bits.ADIF==1){
        PIR1bits.ADIF=0;
        //PORTC=ADRESH;
        ADC=ADRESH;
        __delay_ms(2);
        ADCON0bits.GO=1;
    }
    return;*/
}

void main(void) {
    //CONFIGURANDO OSCILADOR (500kHz)
    OSCCON = 0b00111100;
    
    //ENTRADAS DIGITALES Y ANALÓGICAS
    ANSEL=0;
    ANSELH=0;
    ANSELbits.ANS0; //RA0 es ADC
    
    //ENTRADAS Y SALIDAS
    TRISA=0b00100001;
    TRISB=0b11110000;
    TRISC=0b11011001;
    TRISD=0;
    
    //POSICION INICIAL
    PORTA=0;
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
    
    //HABILITAR INTERRUPCIONES
    /*INTCONbits.GIE=1;   //Interrupciones globales
    INTCONbits.PEIE=1;  //Interrupciones perifericas
    PIE1bits.ADIE=1; ;  //Interruppcion ADC*/
    
    //CONFIGURANDO ADC
    ADCON1=0b00000000; //ADFM left justified, VCFG1=VSS, VCFG0=VDD
    ADCON0=0b11000001; //clk interno=500KHz, AN0=RA0,GO=no, ADCON=1
    __delay_ms(1);     // espera
    ADRESL=0;          //valores ADC=0
    ADRESH=0;
    
    giro=0;
    
    while(1){
        //CALIBRAR EL SENSOR DE FUERZA
        calibracion_presion();
        //presion = medicion_presion();

        //MOVER STEPPER
        if (accion==1){
            stepper();
            if (accion==1){accion=0;}
        }
        
        //MOVER MOTOR DC
        if (accion==2){
            move_pwmDC();
            
            
            giro=1; 
            while(giro==1){
                move_servo(); 
            }  //abre la compuerta
            calibracion_presion();
            
            while(medicion_presion()!=2){PORTD=3;} //verifica si sacó el producto
            PORTD=1;
            __delay_ms(2000);
            giro=2;
            while(giro==2){
                move_servo(); 
            }
            PORTD=2;
            if (accion==2){accion=0;}
        }
        //CERRAR PUERTA
        if ((accion==4&pos==20)){
            //__delay_ms(6000); //recogieron el producto, espera 3 seg.
            giro=2;
            
            if (accion==4){accion=0;}
        }
        //ABRRIR PUERTA
        if ((accion==3&pos==77)){
            giro=1;
            if (accion==3){accion=0;}
        }
        //MOVER SERVO
        if (giro==1|giro==2){ 
            //while(medicion_presion()==1){} //verifica si hay algo despachado
            move_servo(); //cierra la puerta
        }
        
        //RECIBIR ACCIONES
        if (SSPSTATbits.BF==1){
            accion=SSPBUF;
            //PORTD=accion;  
        }
        
    }
}
