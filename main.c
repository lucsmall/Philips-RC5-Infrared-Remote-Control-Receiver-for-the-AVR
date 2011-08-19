#include "main.h"
#include "serial.h"

#define IR_PIN PIND
#define IR_IN  PD6

volatile uint8_t inttemp;
volatile uint8_t timerL;
volatile uint8_t timerH;


uint16_t detect(void);
void main_init(void);

int main(void)
{
  char sys[3];
  char cmd[3];
  uint16_t frame;
  uint8_t toggle;
  
  main_init();
  serial_init(9600);
  sei();
  serial_puts("RC5 IR Decoder\r\n");
  while(1) {
    frame = detect();
    if (frame != 0xFFFF) {
      utoa(((frame >> 6) & 0x1F), sys, 10);
      utoa((frame & 0x3F), cmd, 10);
      toggle = (frame >> 11) & 0x01;
      serial_puts("sys: ");
      serial_puts(sys);
      serial_puts(" cmd: ");
      serial_puts(cmd);
      serial_puts(" toggle: ");
      serial_putc(toggle + '0');
      serial_puts("\r\n");
    }
  }
  return 0;
}

void main_init(void)
{
  // clocked at CK @ 4MHZ
  TCCR0B = _BV(CS00);
  // enable over flow interrupt
  TIMSK = _BV(TOIE0);
}

ISR(TIMER0_OVF_vect)
{
  timerL++;
  inttemp++;
  if (inttemp == 0) {
    timerH++;    
  }
}

uint16_t detect(void)
{
  uint16_t frame;
  uint8_t i, temp, ref1, ref2;
  inttemp = 0;
  timerH = 0;
  
  detect1:
    timerL = 0;
  
    while(timerL < 55) { // idle high for 3.52ms
      if (timerH > 8) { // no success in 131.072ms, time to give up
        goto fault;
      }
  
      if (bit_is_clear(IR_PIN, IR_IN)) {
        goto detect1;
      }
    }
  
    while(bit_is_set(IR_PIN, IR_IN)) { // start1:
      if (timerH >= 8) { // no success in 131.072ms, time to give up
        goto fault;
      }
    }
    
    timerL = 0;
    while(bit_is_clear(IR_PIN, IR_IN)) { // start2:
      if (timerL >= 17) { // high for excess of 1088us
        goto fault;
      }
    }
    
    temp = timerL;
    timerL = 0;
    
    ref1 = temp;  // half bit time
    ref1 >>= 1;   // quarter bit time
    ref2 = ref1;
    ref1 += temp; // 3/4 bit time
    temp <<= 2;   // 1 bit time
    ref2 += temp; // 5/4 bit time
    
    while(bit_is_set(IR_PIN, IR_IN)) { // start3:
      if (timerL >= ref1) {
        goto fault;
      }
    }
    
    //timerL = 0;
    frame = 0;
    
    // sample the 12 data bits
    for(i=0;i<12;i++)
    {
      timerL = 0;
      
      while (timerL < ref1)
        ;
      
      frame <<= 1;
      if (bit_is_set(IR_PIN, IR_IN)) { 
        // store a 1
        frame |= 0x01;      
  
        while(bit_is_set(IR_PIN, IR_IN)) {
          if (timerL > ref2) {
            goto fault;
          }
        }
      } else {
        // no action required to store a 0
        while(bit_is_clear(IR_PIN, IR_IN)) {
           if (timerL > ref2) {
             goto fault;
           }
         }       
      }    
    }
    // frame == 12 bits stored.
    return frame;
  fault:
    // return fault code
    return 0xFFFF;
}