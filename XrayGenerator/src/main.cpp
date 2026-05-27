#include "../../Interface_CentralAcq_Devices/COMM_PROTOCOL.h"
#include <Wire.h>
#include <Arduino.h>

#define SAN_PIN 2
#define SINGLE_SHOT_COUNT 1
#define SERIES_MOTION_SHOT_COUNT 10
#define FLOURO_SHOT_COUNT 4

#define EXAM_ARMED_TIMEOUT_MS    60000   // time between MSG_START and first SAN LOW
#define EXAM_FIRING_TIMEOUT_MS   10000   // hard cap on actual firing once SAN first goes LOW for all besides single shot

// Variables shared between the Wire ISR and loop().
volatile bool      examActive    = false;
volatile ExamType  requestedExam = EXAM_TYPE_NONE;
volatile uint8_t   lastResponse         = NAK;
volatile uint8_t   lastPayloadOutgoing  = 0x00;
volatile uint8_t   lastPayloadIncoming  = 0x00;


bool sanLow = false;

void onReceive(int numBytes);
void onRequest();

void setup() {
  cli();
  Serial.begin(9600);
  Wire.begin(I2C_ADDR_SLAVE_1);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);

  DDRB = DDRB | 0x01;       // pin 8 = X-ray fire output
  PORTB = PORTB & ~0x01;    // ensure OFF
  pinMode(SAN_PIN, INPUT);  // idle: listen for master / Geometry

  sei();
}

// Exam state machine

void loop() {
  static ExamType    currentExam   = EXAM_TYPE_NONE;
  static unsigned long examStartMs = 0;   // when MSG_START arrived (armed)
  static int         shotCount     = 0;

  // if exam starts set start parameters
  if (examActive && currentExam == EXAM_TYPE_NONE) {
    currentExam = requestedExam;
    examStartMs = millis();
    shotCount   = 0;
    pinMode(SAN_PIN, INPUT);
  }

  if (currentExam != EXAM_TYPE_NONE && millis() - examStartMs >= EXAM_ARMED_TIMEOUT_MS) {
    examActive = false;
  }

  // System reset when exam is done
  if (examActive != true && currentExam != EXAM_TYPE_NONE) {
    PORTB = PORTB & ~0x01;        // ensure LED is OFF
    pinMode(SAN_PIN, OUTPUT);     // slave drives SAN to signal data-ready
    currentExam = EXAM_TYPE_NONE;
    return;
  }

  if (examActive == false) {
    return;
  }
 // RETURN SET, below is only if exam is set.
  if (digitalRead(SAN_PIN == LOW)){
   sanLow = true;
  } else {
    sanLow = false;
  }

  switch (currentExam) {
    case EXAM_TYPE_SINGLE_SHOT:
      if (sanLow) {
        PORTB = PORTB | 0x01;
        delay(10);
        PORTB = PORTB & ~0x01;
        lastPayloadOutgoing = 0x01;  // dose result (hardcoded for now)
        examActive = false;
      }
      break;

    case EXAM_TYPE_SERIES:
      if (sanLow) {
        PORTB = PORTB | 0x01;
        delay(10);
        PORTB = PORTB & ~0x01;
        delay(490);
        shotCount++;
      } else if (shotCount > 0) {
        lastPayloadOutgoing = 0x01;
        examActive = false;
      }
      break;

    case EXAM_TYPE_SERIES_WITH_MOTION:
      if (shotCount >= SERIES_MOTION_SHOT_COUNT) {
        lastPayloadOutgoing = 0x01;
        examActive = false;
      }
      if (sanLow) {
        PORTB = PORTB | 0x01;
        delay(10);
        PORTB = PORTB & ~0x01;
        delay(490);
        shotCount++;
      }
      break;

    case EXAM_TYPE_FLUORO:
      if (shotCount >= FLOURO_SHOT_COUNT) {
        lastPayloadOutgoing = 0x01;
        examActive = false;
      }
      if (sanLow) {
        PORTB = PORTB | 0x01;
        delay(10);
        PORTB = PORTB & ~0x01;
        delay(240);
        shotCount++;
      } 
      break;

    default:
      examActive = false;
      break;
  }
}

void onReceive(int numBytes) {
  uint8_t msgType = Wire.read();
  uint8_t msg     = Wire.read();
  lastPayloadIncoming = msg;

  switch ((MsgType)msgType) {
    case MSG_HEARTBEAT:
      lastResponse = ACK;
      break;

    case MSG_START:
      if ((ExamType)msg != EXAM_TYPE_NONE) {
        requestedExam = (ExamType)msg;
        lastPayloadOutgoing = 0x00;   // clear old dose
        examActive = true;   // for loop()
      }
      lastResponse = ACK;
      break;

    case MSG_STOP:
      // Any MSG_STOP returns to idle.
      examActive          = false;
      lastPayloadOutgoing = 0x00;
      lastResponse        = ACK;
      break;

    case MSG_DATA_READY:
      // lastPayloadOutgoing is currently hard coded.
      lastResponse = ACK;
      break;

    case MSG_ERROR:
      // future expansion
      break;

    default:
      lastResponse = NAK;

      break;
  }
}

void onRequest() {
  Wire.write(lastResponse);
  Wire.write(lastPayloadOutgoing);
  lastResponse = NAK;
}
