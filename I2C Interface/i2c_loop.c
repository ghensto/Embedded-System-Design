#include <plib.h>          
//#include "i2c_slave.h"
#include <p32xxxx.h>
#include "i2c_master_noint.h"
// Demonstrate I2C by having the I2C1 talk to I2C5 on the same PIC32
// Master will use SDA1 (D9) and SCL1 (D10).  Connect these through resistors to
// Vcc (3.3 V) (2.4k resistors recommended, but around that should be good enough)
// Slave will use SDA5 (F4) and SCL5 (F5)
// SDA5 -> SDA1
// SCL5 -> SCL1
// Two bytes will be written to the slave and then read back to the slave.
#define SLAVE_ADDR 0x50

int main() {                      // buffer for sending messages to the user
  unsigned char master_write0 = 0xCD;       // first byte that master writes
  unsigned char master_write1 = 0x91;       // second byte that master writes
  unsigned char master_read0  = 0x00;       // first received byte
  unsigned char master_read1  = 0x00;       // second received byte

                   // cache on, interrupts on, LED/button init, UART init
                                            //  (comment out if slave is on another pic)
  i2c_master_setup();                       // init I2C2, which we use as a master  
  
  while(1) {
    
    i2c_master_start();                     // Begin the start sequence
    
    // Select the slave according to the address (Control byte)
    i2c_master_send(0xA0);                  
    i2c_master_send(0); 
    i2c_master_send(0);
    
    // Writes the data 
    i2c_master_send(master_write0);         // send a byte to the slave       
    i2c_master_send(master_write1);         // send another byte to the slave
    i2c_master_stop();
    
    // Select the slave which to read from
    i2c_master_start();                         // Begin the start sequence
    i2c_master_send(0xA0);
    i2c_master_send(0);
    i2c_master_send(0);
    
    // Starts reading
    i2c_master_start();
    i2c_master_send((SLAVE_ADDR << 1) | 1);             // send slave address, left shifted by 1,

                                            // and then a 1 in lsb, indicating read
    master_read0 = i2c_master_recv();            // receive a byte from the bus
    i2c_master_ack(0);                             // send ACK (0): master wants another byte!
    master_read1 = i2c_master_recv(); // receive another byte from the bus
    i2c_master_ack(1); // send NACK (1): master needs no more bytes
    i2c_master_stop(); // send STOP: end transmission, give up bus

    ++master_write0;                        // change the data the master sends
    ++master_write1;
  }
  return 0;
}
