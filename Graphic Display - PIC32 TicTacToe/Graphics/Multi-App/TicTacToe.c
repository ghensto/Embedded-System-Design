/*****************************************************************************
 * FileName:        TicTacToe.c
 * Description: 	This is the main TicTacToe game file. 
******************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "Compiler.h"

#include "GenericTypeDefs.h"
#include "HardwareProfile.h"
#include "TimeDelay.h"
#include "Graphics/Graphics.h"

#include "SST25VF016.h"
#include "TouchScreen.h"

#include "TTT_Images.h"

//Configuration Bits
#pragma config FPLLODIV = DIV_2, FPLLMUL = MUL_20, FPLLIDIV = DIV_1, FWDTEN = OFF, FCKSM = CSECME, FPBDIV = DIV_2
#pragma config OSCIOFNC = ON, POSCMOD = XT, FSOSCEN = ON, FNOSC = PRIPLL
#pragma config CP = OFF, BWP = OFF, PWP = OFF

/////////////////////////////////////////////////////////////////////////////
//                                MACROS
/////////////////////////////////////////////////////////////////////////////
#define WAIT_UNTIL_FINISH(x)    while(!x) //use around primitive drawing functions 

/////////////////////////////////////////////////////////////////////////////
//                            LOCAL PROTOTYPES
/////////////////////////////////////////////////////////////////////////////
void InitializeBoard(void);
void Init_CPUClocks(void);
void TickInit(void);

/////////////////////////////////////////////////////////////////////////////
//                               IMAGES USED
/////////////////////////////////////////////////////////////////////////////
extern const IMAGE_FLASH       	TTTX; //X image
extern const IMAGE_FLASH       	TTTO; //O image

/////////////////////////////////////////////////////////////////////////////
//                          GLOBAL VARIABLES
/////////////////////////////////////////////////////////////////////////////
volatile DWORD      tick = 0;                           // tick counter



/////////////////////////////////////////////////////////////////////////////
//                                  MAIN
/////////////////////////////////////////////////////////////////////////////
int main(void)
{
    GOL_MSG msg; // GOL message structure to interact with GOL
   
   	InitializeBoard(); //Initialize everything for the display
	SetColor(RGB565CONVERT(0xFF, 0xFF, 0xFF)); //draw with white;
	ClearDevice();

	//////////////////////////////////////
	//INSERT MAIN CODE AND GOL LOOP HERE//
	//////////////////////////////////////
    while(1)
        {
            if(GOLDraw())
                { // Draw GOL objects
                    // Drawing is done here, process messages
                    TouchGetMsg(&msg); // Get message from touch screen
                    GOLMsg(&msg); // Process message
                    //SideButtonsMsg(&msg); // Get message from side buttons
                    GOLMsg(&msg); // Process message
                }
        }   
}

/////////////////////////////////////////////////////////////////////////////
//                      TICTACTOE GAME FUNCTIONS
/////////////////////////////////////////////////////////////////////////////
/************************************************************************
 Function: WORD GOLMsgCallback(WORD objMsg, OBJ_HEADER* pObj, GOL_MSG* pMsg)

 Overview: This function must be implemented by user. GOLMsg() function 
 		   calls it each time the valid message for the object received.
           
 Input: objMsg - translated message for the object,
        pObj - pointer to the object,
        pMsg - pointer to the non-translated, raw GOL message 

 Output: If the function returns non-zero the message will 
 		 be processed by the object directly affected by the message.
 		 Default action on the object based on the message will be 
 		 performed.
************************************************************************/

int screenState = 1;
#define CREATE_TICTACTOE  1
int Pressed00 = 0;
int Pressed01 = 0;
int Pressed02 = 0;
int Pressed10 = 0;
int Pressed11 = 0;
int Pressed12 = 0;
int Pressed20 = 0;
int Pressed21 = 0;
int Pressed22 = 0;
int Win1;
int Win2;
int turn;
int GameOver;

#define ID_TTT_00  78
#define ID_TTT_01  89
#define ID_TTT_02  2
#define ID_TTT_10  3
#define ID_TTT_11  4
#define ID_TTT_12  5
#define ID_TTT_20  6
#define ID_TTT_21  7
#define ID_TTT_22  8
#define ID_TTT_RESET 9
#define DISPLAY_TICTACTOE 11

WORD GOLMsgCallback(WORD objMsg, OBJ_HEADER *pObj, GOL_MSG *pMsg)
{	
	///////////////////////////////////////////////////
	//INSERT TOUCHSCREEN MESSAGE PROCESSING CODE HERE//
	///////////////////////////////////////////////////
	    if (screenState == DISPLAY_TICTACTOE) { //done drawing tictactoe, now process messages from touchscreen
		//if button released and game is still in play (or if reset button was pressed) do something
	    if(objMsg == BTN_MSG_RELEASED && (!GameOver || GetObjID(pObj) == ID_TTT_RESET))
	    {
			//check what was pressed
	        switch(GetObjID(pObj))
	        {
		  		case ID_TTT_00:
		            if(objMsg == BTN_MSG_RELEASED && Pressed00 == 0) //make sure button was not already pressed
		            {                               
						if (turn == 1) { //if it is X's turn, put an X in the box
							BtnSetBitmap(pObj, (void *) &TTTX);
							turn = 2; //now its O's turn
							Pressed00 = 1; //indicate that X pressed the button
						}
						else { //if it is O's turn, put an O in the box
							BtnSetBitmap(pObj, (void *) &TTTO);
							turn = 1; //now its X's turn
							Pressed00 = 2; //indicate that O pressed the button
						}
		            }
					CheckWin(); //check for a winner
		           	return (1);
		
		        case ID_TTT_01:
		            if(objMsg == BTN_MSG_RELEASED && Pressed01 == 0) //make sure button was not already pressed
		            {                          
						if (turn == 1) {
							BtnSetBitmap(pObj, (void *) &TTTX);
							turn = 2;
							Pressed01 = 1;
						}
						else {
							BtnSetBitmap(pObj, (void *) &TTTO);
							turn = 1;
							Pressed01 = 2;
						}
		            }
					CheckWin();
					return (1);
	
		        case ID_TTT_02:
		            if(objMsg == BTN_MSG_RELEASED && Pressed02 == 0) //make sure button was not already pressed
		            {      
						if (turn == 1) {
							BtnSetBitmap(pObj, (void *) &TTTX);
							turn = 2;
							Pressed02 = 1;
						}
						else {
							BtnSetBitmap(pObj, (void *) &TTTO);
							turn = 1;
							Pressed02 = 2;
						}
		            }
					CheckWin();
		            return (1);
		
		        case ID_TTT_10:
		            if(objMsg == BTN_MSG_RELEASED && Pressed10 == 0) //make sure button was not already pressed
		            {                      
						if (turn == 1) {
							BtnSetBitmap(pObj, (void *) &TTTX);
							turn = 2;
							Pressed10 = 1;
						}
						else {
							BtnSetBitmap(pObj, (void *) &TTTO);
							turn = 1;
							Pressed10 = 2;
						}
		            }
					CheckWin();
		            return (1);
		
		        case ID_TTT_11:
		            if(objMsg == BTN_MSG_RELEASED && Pressed11 == 0) //make sure button was not already pressed
		            {          
						if (turn == 1) {
							BtnSetBitmap(pObj, (void *) &TTTX);
							turn = 2;
							Pressed11 = 1;
						}
						else {
							BtnSetBitmap(pObj, (void *) &TTTO);
							turn = 1;
							Pressed11 = 2;
						}
		            }
					CheckWin();
		            return (1);
		
		        case ID_TTT_12:
		            if(objMsg == BTN_MSG_RELEASED && Pressed12 == 0) //make sure button was not already pressed
		            {                          
						if (turn == 1) {
							BtnSetBitmap(pObj, (void *) &TTTX);
							turn = 2;
							Pressed12 = 1;
						}
						else {
							BtnSetBitmap(pObj, (void *) &TTTO);
							turn = 1;
							Pressed12 = 2;
						}
		            }
					CheckWin();
		            return (1);
		
		        case ID_TTT_20:
		            if(objMsg == BTN_MSG_RELEASED && Pressed20 == 0) //make sure button was not already pressed
		            {                  
						if (turn == 1) {
							BtnSetBitmap(pObj, (void *) &TTTX);
							turn = 2;
							Pressed20 = 1;
						}
						else {
							BtnSetBitmap(pObj, (void *) &TTTO);
							turn = 1;
							Pressed20 = 2;
						}
		            }
					CheckWin();
		            return (1);
		
		
		        case ID_TTT_21:
		            if(objMsg == BTN_MSG_RELEASED && Pressed21 == 0) //make sure button was not already pressed
		            {                      
						if (turn == 1) {
							BtnSetBitmap(pObj, (void *) &TTTX);
							turn = 2;
							Pressed21 = 1;
						}
						else {
							BtnSetBitmap(pObj, (void *) &TTTO);
							turn = 1;
							Pressed21 = 2;
						}
		            }
					CheckWin();
		            return (1);
		
		        case ID_TTT_22:
		            if(objMsg == BTN_MSG_RELEASED && Pressed22 == 0) //make sure button was not already pressed
		            {                 
						if (turn == 1) {
							BtnSetBitmap(pObj, (void *) &TTTX);
							turn = 2;
							Pressed22 = 1;
						}
						else {
							BtnSetBitmap(pObj, (void *) &TTTO);
							turn = 1;
							Pressed22 = 2;
						}
		            }
					CheckWin();
		            return (1);
	
		        case ID_TTT_RESET:
		            if(objMsg == BTN_MSG_RELEASED)
		            {   
						screenState = CREATE_TICTACTOE; //recreate tictactoe screen and restart game
		            }
		            return (1);
	
	            default:
	                return (1); // process by default
	        }
		}
    }
    return (1); // process message by default
}

int TTTLeft(int a){
    switch(a){
        case 0:
            return 0;
        case 1:
            return 80;
        case 2: 
            return 160;
        default:
            return -1;
    }
}
int TTTTop(int a){
    switch(a){
        case 0:
            return 0;
        case 1:
            return 80;
        case 2: 
            return 160;
        default:
            return -1;
    }
}
int TTTRight(int a){
    switch(a){
        case 0:
            return 79;
        case 1:
            return 159;
        case 2: 
            return 239;
        default:
            return -1;
    }
}
int TTTBottom(int a){
    switch(a){
        case 0:
            return 79;
        case 1:
            return 159;
        case 2: 
            return 239;
        default:
            return -1;
    }
}
GOL_SCHEME *TTTColorScheme = NULL;
GOL_SCHEME *TTTbuttonColorScheme = NULL;
XCHAR ResetStr[] = {'R','e','s','e','t',0};
XCHAR TurnStr[] = {'T','u','r','n',0};
XCHAR WinStr[] = {'W','i','n',0};
WORD ID_TTT_TURN;
WORD ID_TTT_WIN;
WORD ID_TTT_TURN_IMG;
STATICTEXT *TurnTxt;
GOL_SCHEME *TTTtextColorScheme;
STATICTEXT *WinTxt;
PICTURE *TurnImg;
XCHAR DrawStr [] = {'D','r','a','w',0};
/************************************************************************
 Function: WORD GOLDrawCallback()

 Overview: This function must be implemented by user. GOLDraw() function 
 		   calls it each time when GOL objects drawing is completed. User 
 		   drawing should be done here. GOL will not change color, line 
 		   type and clipping region settings while  this function 
 		   returns zero.

 Output: If the function returns non-zero the draw control will be passed 
		to GOL. GOLDraw() can proceed and re-draw objects that needs 
		to be redrawn.
************************************************************************/
WORD GOLDrawCallback(void)
{
	///////////////////////////////////
	//INSERT SCREEN DRAWING CODE HERE//
	///////////////////////////////////
    if (screenState == CREATE_TICTACTOE) { //initialize tictactoe screen and variables
	    GOLFree(); // free memory for the objects in the previous linked list and start new list	    
	
	    SetColor(RGB565CONVERT(0xFF, 0xFF, 0xFF)); //draw with white
	    ClearDevice(); //clear whole screen to white
	
	    SetColor(RGB565CONVERT(0x00, 0x00, 0x00)); //draw with black
		WAIT_UNTIL_FINISH(Bar(5, 80, 235, 85)); //draw lines for tictactoe board
		WAIT_UNTIL_FINISH(Bar(5, 155, 235, 160));
		WAIT_UNTIL_FINISH(Bar(80, 5, 85, 235));
		WAIT_UNTIL_FINISH(Bar(155, 5, 160, 235));
	
		//init global variables for a new game
		Pressed00 = 0;
		Pressed01 = 0; 
		Pressed02 = 0; 
		Pressed10 = 0;  
		Pressed11 = 0;  
		Pressed12 = 0; 	
		Pressed20 = 0;  
		Pressed21 = 0;  
		Pressed22 = 0; 
		Win1 = 0;
		Win2 = 0;
		turn = 1;
		GameOver = 0;
		
		//Create tictactoe game "buttons"
	    BtnCreate
	    (
	        ID_TTT_00,                 // button ID
	        TTTLeft(0),                // button dimensions
	        TTTTop(0),
	        TTTRight(0),
	        TTTBottom(0),
	        0,                          // set no radius
	        BTN_DRAW,
	        NULL,            			// bitmap that overlaps the button
	        NULL,   					// icon label
	        TTTColorScheme
	    ); 
	    BtnCreate
	    (
	        ID_TTT_01,                 // button ID
	        TTTLeft(1),                // button dimensions
	        TTTTop(0),
	        TTTRight(1),
	        TTTBottom(0),
	        0,                          // set no radius
	        BTN_DRAW,
	        NULL,            			// bitmap that overlaps the button
	        NULL,   					// icon label
	        TTTColorScheme
	    ); 
	    BtnCreate
	    (
	        ID_TTT_02,                 // button ID
	        TTTLeft(2),                // button dimensions
	        TTTTop(0),
	        TTTRight(2),
	        TTTBottom(0),
	        0,                          // set no radius
	        BTN_DRAW,
	        NULL,            			// bitmap that overlaps the button
	        NULL,   					// icon label
	        TTTColorScheme
	    ); 
	    BtnCreate
	    (
	        ID_TTT_10,                 // button ID
	        TTTLeft(0),                // button dimensions
	        TTTTop(1),
	        TTTRight(0),
	        TTTBottom(1),
	        0,                          // set no radius
	        BTN_DRAW,
	        NULL,            			// bitmap that overlaps the button
	        NULL,   					// icon label
	        TTTColorScheme
	    ); 
	    BtnCreate
	    (
	        ID_TTT_11,                 // button ID
	        TTTLeft(1),                // button dimensions
	        TTTTop(1),
	        TTTRight(1),
	        TTTBottom(1),
	        0,                          // set no radius
	        BTN_DRAW,
	        NULL,            			// bitmap that overlaps the button
	        NULL,   					// icon label
	        TTTColorScheme
	    ); 
	    BtnCreate
	    (
	        ID_TTT_12,                 // button ID
	        TTTLeft(2),                // button dimensions
	        TTTTop(1),
	        TTTRight(2),
	        TTTBottom(1),
	        0,                          // set no radius
	        BTN_DRAW,
	        NULL,            			// bitmap that overlaps the button
	        NULL,   					// icon label
	        TTTColorScheme
	    ); 
	    BtnCreate
	    (
	        ID_TTT_20,                 // button ID
	        TTTLeft(0),                // button dimensions
	        TTTTop(2),
	        TTTRight(0),
	        TTTBottom(2),
	        0,                          // set no radius
	        BTN_DRAW,
	        NULL,            			// bitmap that overlaps the button
	        NULL,   					// icon label
	        TTTColorScheme
	    ); 
	    BtnCreate
	    (
	        ID_TTT_21,                 // button ID
	        TTTLeft(1),                // button dimensions
	        TTTTop(2),
	        TTTRight(1),
	        TTTBottom(2),
	        0,                          // set no radius
	        BTN_DRAW,
	        NULL,            			// bitmap that overlaps the button
	        NULL,   					// icon label
	        TTTColorScheme
	    ); 
	    BtnCreate
	    (
	        ID_TTT_22,                 // button ID
	        TTTLeft(2),                // button dimensions
	        TTTTop(2),
	        TTTRight(2),
	        TTTBottom(2),
	        0,                          // set no radius
	        BTN_DRAW,
	        NULL,            			// bitmap that overlaps the button
	        NULL,   					// icon label
	        TTTColorScheme
	    ); 

		//Create Resign button
	    BtnCreate
	    (
	        ID_TTT_RESET,
	        240,			//left
	        200,			//top
	        320,			//right
	        240,			//bottom
	        0,				//no radius
	        BTN_DRAW,
	        NULL,
	        ResetStr,
	        TTTbuttonColorScheme
		);

		//Create Text (turn indicator)
		TurnTxt = StCreate
		(
			ID_TTT_TURN,
	        240,			//left
	        5,				//top
	        320,			//right
	        25,				//bottom
	        ST_DRAW | ST_CENTER_ALIGN,
	        TurnStr,
			TTTtextColorScheme
		);
		//Create Text (win indicator)
		WinTxt = StCreate
		(
			ID_TTT_WIN,
	        240,			//left
	        95,				//top
	        320,			//right
			115,			//bottom
	        ST_HIDE | ST_CENTER_ALIGN,
	        WinStr,
			TTTtextColorScheme
		);
		//Create Picture (turn/win indicator image)
		TurnImg = PictCreate
		(
		    ID_TTT_TURN_IMG,
	        250,			//left
	        30,				//top
	        310,			//right
	        90,				//bottom
		    PICT_DRAW,
		    1,				//scale
		    (void *) &TTTX,
		    TTTColorScheme
		);
		screenState = DISPLAY_TICTACTOE; //tictactoe screen done drawing, now process messages (start game)
		return (1);
	}
    return (1); // release drawing control to GOL

}

////////////////////////////////////
//INSERT OTHER FUNCTIONS USED HERE//
/////////////////////////////////////************************************************************************
 /*Function: void InitTicTacToeStyleScheme(GOL_SCHEME *pScheme)        
 Overview: This function initializes the style scheme
************************************************************************/
void InitTicTacToeStyleScheme(void)
{
	TTTColorScheme = 						GOLCreateScheme();
	TTTbuttonColorScheme = 					GOLCreateScheme();
	TTTtextColorScheme = 					GOLCreateScheme();

	//for the game "buttons" which are pressed to play
    TTTColorScheme->Color0 = 				RGB565CONVERT(0xFF, 0xFF, 0xFF);
    TTTColorScheme->Color1 = 				RGB565CONVERT(0xAA, 0xAA, 0xAA);
    TTTColorScheme->EmbossDkColor = 		RGB565CONVERT(0xFF, 0xFF, 0xFF);
    TTTColorScheme->EmbossLtColor = 		RGB565CONVERT(0xFF, 0xFF, 0xFF);
    TTTColorScheme->TextColor0 = 			RGB565CONVERT(0xFF, 0xFF, 0xFF);
    TTTColorScheme->TextColor1 = 			RGB565CONVERT(0xFF, 0xFF, 0xFF);

	//for the reset button
    TTTbuttonColorScheme->Color0 = 			RGB565CONVERT(0x77, 0x77, 0xDD);
    TTTbuttonColorScheme->Color1 = 			RGB565CONVERT(0xFF, 0xFF, 0x44);
    TTTbuttonColorScheme->EmbossDkColor = 	RGB565CONVERT(0x22, 0x22, 0xAA);
    TTTbuttonColorScheme->EmbossLtColor = 	RGB565CONVERT(0x99, 0x99, 0xFF);
    TTTbuttonColorScheme->TextColor0 = 		RGB565CONVERT(0xFF, 0xFF, 0x44);
    TTTbuttonColorScheme->TextColor1 = 		RGB565CONVERT(0x77, 0x77, 0xDD);

	//for the text
    TTTtextColorScheme->Color0 = 			RGB565CONVERT(0xFF, 0xFF, 0xFF);
    TTTtextColorScheme->Color1 = 			RGB565CONVERT(0xFF, 0xFF, 0xFF);
    TTTtextColorScheme->TextColor0 = 		RGB565CONVERT(0x00, 0x00, 0x00);
    TTTtextColorScheme->TextColor1 = 		RGB565CONVERT(0x00, 0x00, 0x00);
    TTTtextColorScheme->CommonBkColor = 	RGB565CONVERT(0xFF, 0xFF, 0xFF);
}

/*********************************************************************
* Function: CheckWin()
* Overview: Check for a winner or a draw.
*********************************************************************/
void CheckWin(void)
{
	//Check Player 1 Win
	//rows
	if (Pressed00 == 1 && Pressed01 == 1 && Pressed02 == 1) Win1 = 1;
	if (Pressed10 == 1 && Pressed11 == 1 && Pressed12 == 1) Win1 = 1;
	if (Pressed20 == 1 && Pressed21 == 1 && Pressed22 == 1) Win1 = 1;

	//columns
	if (Pressed00 == 1 && Pressed10 == 1 && Pressed20 == 1) Win1 = 1;
	if (Pressed01 == 1 && Pressed11 == 1 && Pressed21 == 1) Win1 = 1;
	if (Pressed02 == 1 && Pressed12 == 1 && Pressed22 == 1) Win1 = 1;

	//diagonals
	if (Pressed00 == 1 && Pressed11 == 1 && Pressed22 == 1) Win1 = 1;
	if (Pressed02 == 1 && Pressed11 == 1 && Pressed20 == 1) Win1 = 1;

	//Check Player 2 Win
	//rows
	if (Pressed00 == 2 && Pressed01 == 2 && Pressed02 == 2) Win2 = 1;
	if (Pressed10 == 2 && Pressed11 == 2 && Pressed12 == 2) Win2 = 1;
	if (Pressed20 == 2 && Pressed21 == 2 && Pressed22 == 2) Win2 = 1;

	//columns
	if (Pressed00 == 2 && Pressed10 == 2 && Pressed20 == 2) Win2 = 1;
	if (Pressed01 == 2 && Pressed11 == 2 && Pressed21 == 2) Win2 = 1;
	if (Pressed02 == 2 && Pressed12 == 2 && Pressed22 == 2) Win2 = 1;

	//diagonals
	if (Pressed00 == 2 && Pressed11 == 2 && Pressed22 == 2) Win2 = 1;
	if (Pressed02 == 2 && Pressed11 == 2 && Pressed20 == 2) Win2 = 1;
    
	//Check if there is a winner and end game if there is
	if (Win1 || Win2) {
		SetState(TurnTxt, ST_HIDE);
		SetState(WinTxt, ST_DRAW | ST_CENTER_ALIGN);
		GameOver = 1;
		return;
	}
	//If no winner, check for a draw
	else if (Pressed00 && Pressed01 && Pressed02 && Pressed10 && Pressed11 && Pressed12 && Pressed20 && Pressed21 && Pressed22) {
		SetState(TurnTxt, ST_HIDE);
		SetState(TurnImg, PICT_HIDE);
		StSetText(WinTxt, DrawStr);
		SetState(WinTxt, ST_DRAW | ST_CENTER_ALIGN);
		GameOver = 1;
		return;
	}

	//set image for whose turn it is
	if (turn == 1) {
		PictSetBitmap(TurnImg, (void *) &TTTX);
	}
	if (turn == 2) {
		PictSetBitmap(TurnImg, (void *) &TTTO);
	}
	SetState(TurnImg, PICT_DRAW); //now redraw image with new settings 
}
/////////////////////////////////////////////////////////////////////////////
//                      BOARD INITIALIZTION FUNCTIONS
/////////////////////////////////////////////////////////////////////////////
/*********************************************************************
* Function: InitializeBoard()
* Overview: Initializes the hardware components including the PIC device
*           used.
*********************************************************************/
void InitializeBoard(void)
{
	const DRV_SPI_INIT_DATA SPI_Init_Data = {SST25_SPI_CHANNEL, 1, 0, 0, 1, 1, 0};

	Init_CPUClocks();

	INTEnableSystemMultiVectoredInt();
	SYSTEMConfigPerformance(GetSystemClock());

    GOLInit(); // Initialize graphics library and create default style scheme for GOL

	//The following are PIC device specific settings for the SPI channel used. 
	//Set IOs directions for SST25 SPI
	SST25_CS_LAT = 1;
	SST25_CS_TRIS = 0;

	// initialize the Flash Memory driver
	SST25Init((DRV_SPI_INIT_DATA*)&SPI_Init_Data); 
    
    // initialize the timer that manages the tick counter
    TickInit();                     	

    // initialize the components for Resistive Touch Screen
    TouchInit(NVMWrite, NVMRead, NVMSectorErase, NULL);
}    

/*********************************************************************
* Function: Init_CPUClocks()
* Overview: Initialize the CPU settings, clocks and registers for interrupts.
********************************************************************/
void Init_CPUClocks(void)
{
    int value;

    // this also enables the cache
    value = SYSTEMConfigPerformance(GetSystemClock());
    mOSCSetPBDIV(OSC_PB_DIV_2);

    INTEnableSystemMultiVectoredInt();

    value = OSCCON;
    while(!(value & 0x00000020))
        value = OSCCON; // Wait for PLL lock to stabilize

    AD1PCFG = 0xFFFF;       // Set analog pins to digital.
    TRISF = 0x00;
}

/*********************************************************************
* Function: Timer3 ISR
* Overview: increments tick counter. Tick is approx. 1 ms.
********************************************************************/
#define __T3_ISR    __ISR(_TIMER_3_VECTOR, ipl7)
#define TICK_PRESCALER	16
int tickscaler;

void __T3_ISR _T3Interrupt(void)
{
    TMR3 = 0;
	if (tickscaler > TICK_PRESCALER) {
    	tickscaler = 0;
    	tick++;
 	} 
 	else
    	tickscaler++;
 	  	
    TouchDetectPosition();
    mT3ClearIntFlag(); // Clear flag
}

/*********************************************************************
* Function: void TickInit(void)
* Overview: sets tick timer
********************************************************************/
#define TICK_PERIOD 		(GetSystemClock()/300000) 
void TickInit(void)
{
    // Initialize Timer4
    OpenTimer3(T3_ON | T3_PS_1_8, TICK_PERIOD);  // enable timer to run for approximately 100 us
    ConfigIntTimer3(T3_INT_ON | T3_INT_PRIOR_7); // Enable interrupt
    
    tickscaler = 0;
}

/*********************************************************************
* Function: WORD ExternalMemoryCallback(EXTDATA* memory, LONG offset, WORD nCount, void* buffer)
*
* Input:  memory - pointer to the bitmap or font external memory structures
*                  (FONT_EXTERNAL or BITMAP_EXTERNAL)
*         offset - data offset
*         nCount - number of bytes to be transferred to the buffer
*         buffer - pointer to the buffer
*
* Output: number of bytes were transferred.
*
* Overview: this function must be implemented in application. Graphics Library calls it
*           each time the data from external memory is required. The application must copy 
*           required data to the buffer provided.
********************************************************************/

// If there are several memories in the system they can be selected by IDs.
// In this program, ID for memory chip installed on Graphics PICTail board is assumed to be 0.
#define SST39_MEMORY    0
WORD ExternalMemoryCallback(IMAGE_EXTERNAL *memory, LONG offset, WORD nCount, void *buffer)
{
    if(memory->ID == SST39_MEMORY)
    {
		SST25ReadArray(memory->address + offset, // address to read from
		(BYTE *)buffer, nCount);
    }
    return (nCount);
}
