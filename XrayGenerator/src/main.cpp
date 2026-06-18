#include "../../Interface_CentralAcq_Devices/COMM_PROTOCOL.h"
#include <Wire.h>
#include <Arduino.h>

#define SAN_PIN 2
#define LDR_PIN A0
#define SINGLE_SHOT_COUNT 3
#define SERIES_MOTION_SHOT_COUNT 10
#define FLOURO_SHOT_COUNT 4
#define EXAM_TIMEOUT_MS 10000

static uint8_t lastResponse = NAK;
static uint16_t lastPayloadOutgoing  = 0x00;
static uint8_t lastPayloadIncoming = 0x00;
static uint8_t currentExam = 0x00;
static volatile bool startRequested = false;
static volatile bool dataReadRequested = false;


void onReceive(int numBytes);
void onRequest();

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
  if (startRequested) {
    startRequested = false;
  }

  if (lastPayloadOutgoing > 0 && currentExam == EXAM_TYPE_NONE){
    pinMode(SAN_PIN, OUTPUT);
    digitalWrite(SAN_PIN, LOW);
  } else {
    pinMode(SAN_PIN, INPUT);
  }

  switch (currentExam){

    case EXAM_TYPE_NONE:{
      PORTB = PORTB & ~0x01;
    break;
    }
    case EXAM_TYPE_SINGLE_SHOT: { //ADD TIMER FOR TIMEOUT
      pinMode(SAN_PIN, INPUT);
      int shotCount = 0;
      unsigned long waitTime = millis();
      int baseline = analogRead(LDR_PIN);

      while(shotCount < SINGLE_SHOT_COUNT && (millis() - waitTime) < EXAM_TIMEOUT_MS){
        if (digitalRead(SAN_PIN)==LOW){
          PORTB = PORTB | 0x01;
          delay(10);
          PORTB = PORTB & ~0x01;
          shotCount++;
        }
      }
      lastPayloadOutgoing = analogRead(LDR_PIN) - baseline;
    break;
    } 

    case EXAM_TYPE_SERIES: {
      pinMode(SAN_PIN, INPUT);
      int baseline = analogRead(LDR_PIN);
      unsigned long waitTime = millis();

      while(digitalRead(SAN_PIN) == HIGH){
        if ((millis() - waitTime) >= EXAM_TIMEOUT_MS){
          analogWrite(LED_BUILTIN, 255);
          delay(50);
          analogWrite(LED_BUILTIN, 0);
          break;
        }
      }
      while(digitalRead(SAN_PIN) == LOW){
        PORTB = PORTB ^ 0x01;
        delay(10);
        PORTB = PORTB ^ 0x01;
        delay(490);
        if(lastPayloadOutgoing < analogRead(LDR_PIN)){ //find maximum value
          lastPayloadOutgoing = analogRead(LDR_PIN) - baseline;
        }
      }
      break;
    }


    case EXAM_TYPE_SERIES_WITH_MOTION: {
      pinMode(SAN_PIN, INPUT);
      int shotCount = 0;
      int baseline = analogRead(LDR_PIN);

      while(shotCount < SERIES_MOTION_SHOT_COUNT || lastResponse == NAK){ // ADD TIMER FOR TIMEOUT
        if (digitalRead(SAN_PIN) == LOW){
          PORTB = PORTB ^ 0x01;
          delay(10);
          PORTB = PORTB ^ 0x01;
          delay(490);
          shotCount++;
        }
        if(lastPayloadOutgoing < analogRead(LDR_PIN)){ //find maximum value
            lastPayloadOutgoing = analogRead(LDR_PIN) - baseline;
        }
      }
      break;
    }


    case EXAM_TYPE_FLUORO: {
      int shotCount = 0;
      int baseline = analogRead(LDR_PIN);
      pinMode(SAN_PIN, INPUT);

      unsigned long waitTime = millis();

      if ((millis() - waitTime) >= EXAM_TIMEOUT_MS){
          analogWrite(LED_BUILTIN, 255);
          delay(50);
          analogWrite(LED_BUILTIN, 0);
          break;
        }

      while(shotCount < FLOURO_SHOT_COUNT && digitalRead(SAN_PIN) == LOW){ // ADD TIMEOUT
        PORTB = PORTB ^ 0x01;
        delay(10);
        PORTB = PORTB ^ 0x01;
        delay(240);
        shotCount++;
        if(lastPayloadOutgoing < analogRead(LDR_PIN)){ //find maximum value
          lastPayloadOutgoing = analogRead(LDR_PIN) - baseline;
        }
      }
      break;
    }
    currentExam = EXAM_TYPE_NONE; 
  }
}


void onReceive(int numBytes) {
  uint8_t msgType = Wire.read();
  uint8_t msg     = Wire.read();

  lastPayloadIncoming = msg;

  Serial.print("type=0x"); Serial.println(msgType, HEX);

  if (msg != 0x00){
    Serial.print("Incoming=0x"); Serial.println(msg, HEX);
  }

  switch ((MsgType) msgType) {
    case MSG_HEARTBEAT:
      lastResponse = ACK;
      break;
    case MSG_START:
      startRequested = true;
      currentExam = lastPayloadIncoming;
      lastResponse = ACK;
      break;
    case MSG_STOP:
      currentExam = EXAM_TYPE_NONE;
      lastResponse = ACK;
      break;
    case MSG_ERROR:
    //FUTURE PROOFING
      break;
    case MSG_DATA_READY:
      dataReadRequested = true;
      lastResponse = ACK;
      break;
    default:
      lastResponse = NAK;
      break;
  }
}

void onRequest() {
  Wire.write(lastResponse);
  Wire.write((uint8_t)(lastPayloadOutgoing >> 8)); // high byte
  Wire.write((uint8_t)(lastPayloadOutgoing & 0xFF)); //low byte
  Serial.print("Response=0x"); Serial.print(lastResponse, HEX);
  Serial.print(" Payload=0x"); Serial.println(lastPayloadOutgoing, HEX);
  lastResponse = NAK;
  if (dataReadRequested){
    lastPayloadOutgoing = 0;
    dataReadRequested = false;
  }
}