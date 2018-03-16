#include <p18f4550.h>

		#pragma config PLLDIV   = 5         // (20 MHz crystal on PICDEM FS USB board)
        #pragma config CPUDIV   = OSC1_PLL2   
        #pragma config USBDIV   = 2         // Clock source from 96MHz PLL/2
        #pragma config FOSC     = HSPLL_HS
        #pragma config FCMEN    = OFF
        #pragma config IESO     = OFF
        #pragma config PWRT     = OFF
        #pragma config BOR      = ON
        #pragma config BORV     = 3
        #pragma config VREGEN   = ON      //USB Voltage Regulator
        #pragma config WDT      = OFF
        #pragma config WDTPS    = 32768
        #pragma config MCLRE    = ON
        #pragma config LPT1OSC  = OFF
        #pragma config PBADEN   = OFF
//      #pragma config CCP2MX   = ON
        #pragma config STVREN   = ON
//      #pragma config CPD      = OFF
        #pragma config LVP      = OFF
//      #pragma config ICPRT    = OFF       // Dedicated In-Circuit Debug/Programming
        #pragma config XINST    = OFF       // Extended Instruction Set
        #pragma config CP0      = OFF
        #pragma config CP1      = OFF
//      #pragma config CP2      = OFF
//      #pragma config CP3      = OFF
        #pragma config CPB      = OFF
        #pragma config WRT0     = OFF
        #pragma config WRT1     = OFF
//      #pragma config WRT2     = OFF
//      #pragma config WRT3     = OFF
        #pragma config WRTB     = OFF       // Boot Block Write Protection
        #pragma config WRTC     = OFF
//      #pragma config WRTD     = OFF
        #pragma config EBTR0    = OFF
        #pragma config EBTR1    = OFF
//      #pragma config EBTR2    = OFF
//      #pragma config EBTR3    = OFF
        #pragma config EBTRB    = OFF

#pragma udata BDT = 0x400
volatile ram char BD0STAT_OUT;
volatile ram char BD0CNT_OUT;
volatile ram char BD0ADRL_OUT;
volatile ram char BD0ADRH_OUT;
volatile ram char BD0STAT_IN;
volatile ram char BD0CNT_IN;
volatile ram char BD0ADRL_IN;
volatile ram char BD0ADRH_IN;
#pragma code

#pragma udata PKTOUT = 0x500
volatile ram char Data_OUT[64];
#pragma code

#pragma udata PKTIN = 0x600
volatile ram char Data_IN[64];
#pragma code

#define OUTTOKEN 0x01
#define	INTOKEN 0x09
#define SETUP 0x0D
#define GETDEV 0x06

unsigned char ascii(unsigned char c); 
void SCI_Init(void);
void USB_Init(void);
void set_device_descriptor(void);
void display_data_rx(void);
void display_data_tx(void);
void dump_info(void);
void SCI_newline(void);



#pragma interrupt high_ISR
void high_ISR(void) 
{	
	UCONbits.PKTDIS = 0;
	PIR2bits.USBIF = 0;

	if(UIRbits.TRNIF) {
	//	while (!TXSTAbits.TRMT); TXREG = 'T';
	//	while (!TXSTAbits.TRMT); TXREG = 'E';
		//SCI_newline();
		
		if(!(USTAT & 0x04)) //OUT transaction?
		{
			switch((BD0STAT_OUT>>2)&0x0F) //token PID
			{
				case SETUP:
					while (!TXSTAbits.TRMT); TXREG = 'S';
					while (!TXSTAbits.TRMT); TXREG = 'T';
					while (!TXSTAbits.TRMT); TXREG = 'P';
					while (!TXSTAbits.TRMT); TXREG = '-';
					set_device_descriptor();
					if(Data_OUT[1] == GETDEV) //SETUP request type == GETDEV?
					{
						while (!TXSTAbits.TRMT); TXREG = 'G';
						while (!TXSTAbits.TRMT); TXREG = 'E';
						while (!TXSTAbits.TRMT); TXREG = 'T';
						while (!TXSTAbits.TRMT); TXREG = 'D';
						while (!TXSTAbits.TRMT); TXREG = 'E';
						while (!TXSTAbits.TRMT); TXREG = 'V';
						while (!TXSTAbits.TRMT); TXREG = '-';
					}
					while (!TXSTAbits.TRMT); TXREG = ascii(Data_OUT[6]>>4); // DATA_OUT[6]=number of bytes requested
					while (!TXSTAbits.TRMT); TXREG = ascii(Data_OUT[6]&0x0F);
					SCI_newline();
					break;
				case OUTTOKEN:
					while (!TXSTAbits.TRMT); TXREG = 'O';
					while (!TXSTAbits.TRMT); TXREG = 'U';
					while (!TXSTAbits.TRMT); TXREG = 'T';
					SCI_newline();
					break;
				default:
					while (!TXSTAbits.TRMT); TXREG = ascii((BD0STAT_OUT>>2)&0x0F);
			}
			dump_info(); //print out everything
			while(BD0STAT_OUT&0x08);
			BD0STAT_OUT = 0x80; //UOWN= 1, control back to SIE
			
		} else { //in token
			switch((BD0STAT_IN>>2)&0x0F)
			{
				case INTOKEN:
			//		while (!TXSTAbits.TRMT); TXREG = 'I';
			//		while (!TXSTAbits.TRMT); TXREG = 'N';
					//SCI_newline();
					break;
			}
			while(BD0STAT_IN&0x08);
			BD0STAT_IN = 0x80;
		}
		
		UIRbits.TRNIF = 0;
	}
	if (UIRbits.URSTIF) {
		
		while (!TXSTAbits.TRMT); TXREG = 'R';
		while (!TXSTAbits.TRMT); TXREG = 'S';
		while (!TXSTAbits.TRMT); TXREG = 'T';
		while (!TXSTAbits.TRMT); TXREG = 10;
		while (!TXSTAbits.TRMT); TXREG = 13;
		UIRbits.URSTIF = 0;

	}
	if(UIRbits.SOFIF) {
		UIRbits.SOFIF = 0;
	//while (!TXSTAbits.TRMT); TXREG = 'F';
	}
	if(UIRbits.UERRIF) { 
		while (!TXSTAbits.TRMT); TXREG = 'E';
		UIRbits.UERRIF = 0;
	}

	
			

}


#pragma code high_vector = 0x08
void interrupt_at_high_vector(void)
{
	_asm goto high_ISR _endasm
}
#pragma code

void main( void) {

	unsigned int i, j;
	unsigned int DATA;
	unsigned char TEMP;
	unsigned char title[16]= {'U','S','B',' ','F','i','r','m','w','a','r','e',' ','L','a','b'};

   T1CON = 0;

   TRISA = 0;
   TRISB = 0;
   TRISC = 0;
   TRISD = 0;
   TRISE = 0;
   TRISA = 0;
   PORTB = 0;
   PORTC = 0;
   PORTD = 0;
   PORTE = 0;
   ADCON1 = 0x0F;

   SCI_Init();
	
	SCI_newline();

	for(i=0; i<16; i++){
		while (!TXSTAbits.TRMT); TXREG = title[i];
	}

	SCI_newline();

	USB_Init();
	
   while(1) {
      }  

}


void set_device_descriptor(void){
	Data_IN[0] = 0x12;
	Data_IN[1] = 0x01;
	Data_IN[2] = 0x00;
	Data_IN[3] = 0x02;
	Data_IN[4] = 0x00;
	Data_IN[5] = 0x00;
	Data_IN[6] = 0x00;
	Data_IN[7] = 0x40;
}

void SCI_newline(void){
	while (!TXSTAbits.TRMT); TXREG = 10;
	while (!TXSTAbits.TRMT); TXREG = 13;
}

void SCI_Init(void)
{
   TRISC = TRISC | 0xC0;
   PIE1bits.TXIE = 0;
   PIE1bits.RCIE = 0;
   SPBRG = 26; //48MHz clock, 115200 baud rate
   TXSTA = 0x26; //BRGH=1
   RCSTA = 0x90;
   BAUDCONbits.BRG16 = 0;
}

void USB_Init(void) {
	INTCONbits.GIE = 1;
	INTCONbits.PEIE = 1;
	UCFGbits.UPUEN = 1;
	UCFGbits.FSEN = 1;
	UCFG &= 0xFC; //disable ping-pong buffers
	PIE2bits.USBIE = 1;
	PIR2bits.USBIF = 0;
	UIRbits.URSTIF = 0;
	UIRbits.TRNIF = 0;
	UIEbits.TRNIE = 1;
	UIEbits.URSTIE = 1;
	UIRbits.SOFIF = 0;
	UIEbits.SOFIE = 1;
	UCONbits.PKTDIS = 0;
	
	UIRbits.UERRIF = 0;
	UIEbits.UERRIE = 1;
	
	BD0CNT_OUT = 64;
	BD0ADRL_OUT	= 0x00;
	BD0ADRH_OUT = 0x05;
	BD0STAT_OUT = 0x00;

	BD0CNT_IN = 64;
	BD0ADRL_IN	= 0x00;
	BD0ADRH_IN = 0x06;
	BD0STAT_IN = 0x00;

	BD0STAT_OUT |= 0x80;
	BD0STAT_IN |= 0x80;

	UEP0 = 0x16;

	UCONbits.USBEN = 1;
}

void display_data_rx() {
	int i;
	for(i = 0; i < 8; i++) {
		while (!TXSTAbits.TRMT); TXREG = ascii(Data_OUT[i]>>4);
		while (!TXSTAbits.TRMT); TXREG = ascii(Data_OUT[i] & 0x0F);
		while (!TXSTAbits.TRMT); TXREG = ' ';
	}
	
}

void display_data_tx() {
	//Implement
		int i;
	for(i = 0; i < 8; i++) {
		while (!TXSTAbits.TRMT); TXREG = ascii(Data_IN[i]>>4);
		while (!TXSTAbits.TRMT); TXREG = ascii(Data_IN[i] & 0x0F);
		while (!TXSTAbits.TRMT); TXREG = ' ';
	}
	
}

void dump_info() {
		
	int i;
	//Display Title here
	
	SCI_newline();

	//Display BuffRX here 
	//name first
	display_data_rx();
	SCI_newline();

	//Display BuffTX
	//name first
	display_data_tx();
	SCI_newline();

	//Display UCON
	//name first
	while (!TXSTAbits.TRMT); TXREG = ascii(UCON>>4);
	while (!TXSTAbits.TRMT); TXREG = ascii(UCON & 0x0F); 

	while (!TXSTAbits.TRMT); TXREG = ' ';

	// Display USTAT (similar method to UCON)
	//name first
	//implement here
	while (!TXSTAbits.TRMT); TXREG = ascii(USTAT>>4);
	while (!TXSTAbits.TRMT); TXREG = ascii(USTAT & 0x0F); 

	while (!TXSTAbits.TRMT); TXREG = ' ';


	//Display UEP0
	//name first
	while (!TXSTAbits.TRMT); TXREG = ascii(UEP0>>4);
	while (!TXSTAbits.TRMT); TXREG = ascii(UEP0 & 0x0F); 
	SCI_newline();

	//Display BD0STAT_OUT
	//name first
	while (!TXSTAbits.TRMT); TXREG = ascii(BD0STAT_OUT>>4);
	while (!TXSTAbits.TRMT); TXREG = ascii(BD0STAT_OUT & 0x0F);
	
	while (!TXSTAbits.TRMT); TXREG = ' ';

	//Display BD0CNT_OUT
	//name first 
	//implement here (similar to BDOSTAT_OUT)
	while (!TXSTAbits.TRMT); TXREG = ascii(BD0CNT_OUT>>4);
	while (!TXSTAbits.TRMT); TXREG = ascii(BD0CNT_OUT & 0x0F);
	
	while (!TXSTAbits.TRMT); TXREG = ' ';
	SCI_newline(); 

	//Display BD0STAT_IN
	//name 
	//implement (similar to BDOSTAT_OUT)
	while (!TXSTAbits.TRMT); TXREG = ascii(BD0STAT_IN>>4);
	while (!TXSTAbits.TRMT); TXREG = ascii(BD0STAT_IN & 0x0F);
	
	while (!TXSTAbits.TRMT); TXREG = ' ';

	//BD0CNT_IN
	//implement similar here as BDOCNT_out
	while (!TXSTAbits.TRMT); TXREG = ascii(BD0CNT_IN>>4);
	while (!TXSTAbits.TRMT); TXREG = ascii(BD0CNT_IN & 0x0F);
	
	while (!TXSTAbits.TRMT); TXREG = ' ';

	SCI_newline(); 
	
}

unsigned char ascii(unsigned char c) 
{
   c = c & 0x0F;
   if (c < 10) return (c+48);
   else return(c + 55);
}
