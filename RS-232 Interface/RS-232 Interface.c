#include  <p18f4520.h>
#include <stdlib.h>
#include <stdio.h>

// Delay function
void delay_ms(unsigned int count)
{
    while (count != 0)
    {
        count = count - 1;
    }
}

unsigned int delay = 200;

// Device setup initialization
void setup(void)
{
    ADCON1 = 16;
    TRISB = 0x00;
    PORTB = 0xFF;
    TRISD = 0x00;
    // UART setup
    BAUDCON=0;           	// set up for 8-bit communication 
    RCSTA = 0;
    RCSTAbits.SPEN = 1;    //enable the serial port
    RCSTAbits.CREN = 1;    //enable the receiver
    TRISC = 0;             //makes RC6 (USART transmit) an output
    TRISCbits.TRISC7 = 1;  //makes RC7 (USART receive) an input
    TXSTA = 0;
    TXSTAbits.BRGH = 1;    //sets up for 19200 baud from the 4MHz FOSC
    TXSTAbits.TXEN = 1;    //enable transmission 
    SPBRG = 12;            //sets up for 19200 baud from the 4MHz FOSC
    
    // LCD setup
    PORTDbits.RD7 = 1;          //Keep the power on
    delay_ms(600);
    PORTD = 0b10000010;         // Power, E, R/W, RS, DB7-4, Function Set
    PORTDbits.RD6 = 1; 
    delay_ms(delay);
    PORTDbits.RD6 = 0;
    delay_ms(delay);

    PORTD = 0b10000000;     //Display on/off control
    PORTDbits.RD6 = 1;
    delay_ms(delay);
    PORTDbits.RD6 = 0;
    PORTD = 0b10001110;
    PORTDbits.RD6 = 1;
    delay_ms(delay);
    PORTDbits.RD6 = 0;

    PORTD = 0b10000000;     //Entry mode set
    PORTDbits.RD6 = 1;
    delay_ms(delay);
    PORTDbits.RD6 = 0;
    delay_ms(delay);
    PORTD = 0b10000110;
    PORTDbits.RD6 = 1;
    delay_ms(delay);
    PORTDbits.RD6 = 0;


    
}

// Transmit characters
void PPutch(char c){
    while (TXSTAbits.TRMT == 0);
    TXREG = c;
}

// Receive characters
unsigned char PGetch(void)
{
    while (PIR1bits.RCIF == 0) { // delay_ms for char
    if (RCSTAbits.OERR == 1) {
        RCSTAbits.OERR = 0; // clear overrun if it occurs
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }
}
return RCREG;
}

// Display characters on LCD
void display(char x){
    PORTD = 0x80;
    int first = x >> 4;
    PORTDbits.RD4 = 1;
    
    PORTD |= first & 0b00001111; 
    PORTDbits.RD6 = 1;
    delay_ms(delay);
    PORTDbits.RD6 = 0;
    
    PORTD = 0x80;
    PORTDbits.RD4 = 1;
    PORTD |= x & 0b00001111;
    PORTDbits.RD6 = 1;
    delay_ms(delay);
    PORTDbits.RD6 = 0;
    PORTDbits.RD4 = 0;
}


int main()
{
	// Initialzes device
    setup();
	
	// Continuously transmit and receive characters
    while (1)
    {
        unsigned char x;
        x = PGetch();
        PPutch(x);
        display(x);
    }
    return 0;
}
