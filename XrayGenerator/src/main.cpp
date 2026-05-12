#include "COMM_PROTOCOL.h"
#include <Wire.h>
#include <Arduino.h>

#define SAN_PIN 2
#define SINGLE_SHOT_COUNT 1
#define SERIES_MOTION_SHOT_COUNT 10
#define FLOURO_SHOT_COUNT 4

static uint8_t lastResponse = NAK;
static uint8_t lastPayloadOutgoing  = 0x00;
static uint8_t lastPayloadIncoming = 0x00;


void onReceive(int numBytes);
void onRequest();
void handleMessage(MsgType type);

void setup() {
  cli(); //disable all interrupts
  Serial.begin(9600);
  Wire.begin(I2C_ADDR_SLAVE_1);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);

  DDRB = DDRB | 0x01; // Set pin 8 as output pin, 9-12 to input

  sei(); //enable all interrupts
}

void loop() {
 // ADD BOOL IF DATA READY SAN = LOW
}

void onReceive(int numBytes) {
  uint8_t msgType = Wire.read();
  uint8_t msg = Wire.read();
  lastPayloadIncoming = msg;

  Serial.print("type=0x"); Serial.println(msgType, HEX);

  if (msg != 0x00){
    Serial.print("Incoming=0x"); Serial.println(msg, HEX);
  }

  switch ((MsgType) msgType) {
    case MSG_HEARTBEAT:
      handleMessage(MSG_HEARTBEAT);
      lastResponse = ACK;
      break;
    case MSG_START:
      handleMessage(MSG_START);
      lastResponse = ACK;
      break;
    case MSG_STOP:
      handleMessage(MSG_STOP);
      lastResponse = ACK;
      break;
    case MSG_ERROR:
    //FUTURE PROOFING
      break;
    case MSG_DATA_READY:
      handleMessage(MSG_DATA_READY);
      lastResponse = ACK;
      break;
    default:
      lastResponse = NAK;
      break;
  }
}

void onRequest() {
  Wire.write(lastResponse);
  Wire.write(lastPayloadOutgoing);
  Serial.print("Response=0x"); Serial.print(lastResponse, HEX);
  Serial.print(" Payload=0x"); Serial.println(lastPayloadOutgoing, HEX);
  lastResponse = NAK;
  
}

void handleMessage(MsgType type){
  switch (type) {
    case MSG_START:
      switch(lastPayloadIncoming) {
        case EXAM_TYPE_NONE:
          PORTB = PORTB & ~0x01;
        break;

        case EXAM_TYPE_SINGLE_SHOT: //ADD TIMER FOR TIMEOUT
          pinMode(SAN_PIN, INPUT);
            int shotCount = 0;

            while(shotCount < SINGLE_SHOT_COUNT){
              if (digitalRead(SAN_PIN)==LOW){
                PORTB = PORTB | 0x01;
                delay(10);
                PORTB = PORTB & ~0x01;
                shotCount++;
              }
            }
            pinMode(SAN_PIN,OUTPUT);

        break;

        case EXAM_TYPE_SERIES:
          pinMode(SAN_PIN, INPUT);

          while(digitalRead(SAN_PIN) == HIGH){
            //ADD TIMEOUT
          }
          while(digitalRead(SAN_PIN) == LOW){
            PORTB = PORTB ^ 0x01;
            delay(10);
            PORTB = PORTB ^ 0x01;
            delay(490);
          }
          pinMode(SAN_PIN,OUTPUT);
        break;

        case EXAM_TYPE_SERIES_WITH_MOTION:
          pinMode(SAN_PIN, INPUT);
          int shotCount = 0;

          while(shotCount < SERIES_MOTION_SHOT_COUNT || lastResponse == NAK){ // ADD TIMER FOR TIMEOUT
            if (digitalRead(SAN_PIN) == LOW){
              PORTB = PORTB ^ 0x01;
              delay(10);
              PORTB = PORTB ^ 0x01;
              delay(490);
              shotCount++;
            }
          }
          pinMode(SAN_PIN,OUTPUT);
        break;

        case EXAM_TYPE_FLUORO:
          int shotCount = 0;
          pinMode(SAN_PIN, INPUT);

          while(digitalRead(SAN_PIN) == HIGH){
            //ADD TIMEOUT
          }
          while(shotCount < FLOURO_SHOT_COUNT || digitalRead(SAN_PIN) == LOW){ // ADD TIMEOUT
            PORTB = PORTB ^ 0x01;
            delay(10);
            PORTB = PORTB ^ 0x01;
            delay(240);
            shotCount++;
          }
          pinMode(SAN_PIN,OUTPUT);
          
        break;
      }
      break;
    case MSG_STOP:
      lastPayloadOutgoing = 0x00; //RESETS THE RESPONSE, DATA CAN BE READ ONLY ONCE UNTIL RE-REQUEST
      break;
    case MSG_HEARTBEAT:
    case MSG_DATA_READY:
      lastPayloadOutgoing = 0x01; //HARDCODED FOR NOW, THIS WOULD BE SENSOR DATA
      break;
    case MSG_ERROR:
    default:
      break;
  }
}
