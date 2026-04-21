#include "../include/COMM_PROTOCOL.h"
#include <Wire.h>

static uint8_t lastResponse = NACK;
static uint8_t lastPayload  = 0x00;

//void onReceive(int numBytes);
//void onRequest();

void setup() {
  Serial.begin(9600);
  Wire.begin(I2C_ADDR_SLAVE_1);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);

}

void loop() {

}

void onReceive(int numBytes) {
  uint8_t msgType = Wire.read();
  uint8_t msg = Wire.read();

  Serial.print("type=0x"); Serial.println(msgType, HEX);

  switch ((MsgType)msgType) {
    case MSG_HEARTBEAT:
      lastResponse = ACK;
      break;
    case MSG_START:
      lastResponse = ACK;
      break;
    case MSG_STOP:
      
      break;
    case MSG_ERROR:

      break;
    case MSG_DATA_READY:
      lastResponse = ACK;
      lastPayload = 0x01; //HARDCODED FOR NOW, THIS IS SENSOR DATA
      handleMessage(MSG_DATA_READY);
      break;
    default:
      lastResponse = NACK;
      break;
  }
}

void onRequest() {
  Wire.write(lastResponse);
  Wire.write(lastPayload);
  Serial.print("Payload=0x"); Serial.println(lastPayload, HEX);
  lastPayload = 0x00; //RESETS THE RESPONSE, DATA CAN BE READ ONLY ONCE UNTIL RE-REQUEST
  
}

void handleMessage(MsgType type){
  switch (type) {
    case MSG_START:
    case MSG_STOP:
    case MSG_HEARTBEAT:
    case MSG_DATA_READY:
    case MSG_ERROR:
    default:
      break;
  }
}

/* Polling is continuous checks, interrupts are instant.

void isr()     

isr(interrupt service routine) will be ran when an intterupt is called
before continuing. You have to map the function to when a pin is 
high for example. 
You use attachInterrupt(digitalPinToInterrupt(pin), function, mode);
for using a button it would be attachInterrupt(digitalPinToInterrupt(pin), isr, RISING);

an interrupt is meant to be short
RISING = pin is from LOW to HIGH
FALLING = pin is from LOW to HIGH

consider debounce (ISR function cant be interrupted by ISR)

use volatile so that the complier doesn't assume variables can't be changed.
volatile means it can change, so it doesn't optimize the usage of the int.
Its a way of warnign the complier that a variable can suddenly change, 
for example by an isr.
everything changed by the isr should be volatile.

REGISTERS:
there are special registers for enabling interrupts, which need to be enabled

a vector is a address of a function basically.
theres an interrupt vector table, which is where the functions are stored
and the start of the ISR's. For example reset interrupt is vector #1 
with address 0x0000. 

You define your own ISR by doing ISR(INTERRUPT VECTOR FROM INTERRUPT VECTOR TABLE){code}
ex. ISR(RESET_VECT), syntax is func(INTERRUPTNAME_VECT)
ISR is a macro which will find the address

types of interrupts: 
external interrupts (pin change interrupts)     $13
- PCINT pins 0-23 except 15
- INT0, INT1 pins (same as PCINT 18 and PCINT19 with more functionality)
Internal interrupt
- Timer 0,1,2 interrupts     $15

PCIE is pinchange interrupt e..  pin change interrupt occurs on 
falling and rising edge

PCICR register pinchange interrupt control register

PCMSK0, pin change mask (set which are allowed)



special ones (INT0, INT1/pin 18/19)

EIMSK (external interrupt mask register)

EIFR (External interupt glag register)

EIFR (external interrupt control register A)

configurable for ONLY rising or falling


enable interupt for sepcific PCINT# pin
ex. PCMSK# |= (1<<PCINT#);

enable an itnerrupt in PCICR register for set of PCINT# pins (check datasheet for 
group #, depends on which pin it is on)
PCICR |= (1<<PCIE#);

enable the global detach
SREG |= (1<<SREG_I);

ISR(PCINT#_vect){ CHECK PIN }



cli() means clear interrupts and sei() is enable all. This is good when they 
are still being set for example.



instead of writing PCICR |= (1<<PCIE0), PCICR |= _BV(PCIE0) can be used.
instead of writing PCICR |= 
theres another macro in the presentation

*/