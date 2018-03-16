/*
** RWTest.c
**
*/
// configuration bit settings, Fcy=72 MHz, Fpb=36 MHz
#pragma config POSCMOD=XT, FNOSC=PRIPLL
#pragma config FPLLIDIV=DIV_2, FPLLMUL=MUL_18, FPLLODIV=DIV_1
#pragma config FPBDIV=DIV_2, FWDTEN=OFF, CP=OFF, BWP=OFF
#include <p32xxxx.h>
#include "SDMMC.h"
#include <string.h>
#define B_SIZE 512 // data block size
char data[B_SIZE];
char buffer[B_SIZE];
#define START_ADDRESS 1000 // start block address
#define N_BLOCKS 10 // number of blocks
#define LEDWORK _LATD0
#define LEDWRITE _LATD1
#define LEDREAD _LATD2

void delayms(int ms)
{
    while(ms != 0)
        ms--;
}

main(void)
{
    _TRISD0 = 0;
    _TRISD1 = 0;
    _TRISD2 = 0;
    LBA addr;
    int i, j, r;
   
    // 1. initializations
    initSD(); // init SD/MMC module
        
    // 2. fill the buffer with pattern
    for( i=0; i<B_SIZE; i++)
        data[i]= i+'A';
    
    // 3. wait for the card to be inserted
    while( !getCD()); // check CD switch
    delayms(100); // wait contacts de-bounce
    
    if ( initMedia()) // init card
    { // if error code returned
        goto End;
    }

    // 4. fill 16 groups of N_BLOCK sectors with data
    LEDWORK = 1; // SD card in use

    while(1){
    addr = START_ADDRESS;
    for( j=0; j<16; j++)
    {
        for( i=0; i<N_BLOCKS; i++)
        {
            if (!writeSECTOR(addr+i*j, data))
            { // writing failed
                 goto End;
            }
            LEDWRITE = 1;
        } // i
    } // j
    LEDWRITE = 0;
    // 5. verify the contents of each sector written
   
    addr = START_ADDRESS;
    for( j=0; j<16; j++)
    {
        for( i=0; i<N_BLOCKS; i++)
        { // read back one block at a time
            if (!readSECTOR( addr+i*j, buffer))
            { // reading failed
                goto End;
            }
            LEDREAD = 1;
            // verify each block content
            if ( memcmp( data, buffer, B_SIZE))
            { // mismatch
                goto End;
            }
        } // i
    } // j
    LEDREAD = 0;

    // 7. indicate successful execution
    End:
    LEDWORK = 0; // SD card not in use
    }

} // main