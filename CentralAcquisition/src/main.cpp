#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "../../Interface_PatAdmin_CentralAcq/Protocol_PatientAdmin_CentralAcq.h"
#include "../../Interface_CentralAcq_Devices/COMM_PROTOCOL.h"

#define MAX_RETRIES 3

#define GRNLED 5
#define REDLED 6
#define SAN_PIN 2
#define PREPARE_BUTTON_PIN 8
#define XRAY_BUTTON_PIN 10

#define I2C_RESPONSE_TIMEOUT_MS  200    // wait for slave to put bytes on the bus
#define PREPARE_TIMEOUT_MS       5000   // give up STATE_PREPARING if no slave responds
#define PREPARED_INTERVAL_MS     500    // ms between I2C broadcasts inside STATE_PREPARING
#define DATA_READY_TIMEOUT_MS    2000   // give up polling MSG_DATA_READY
#define BUTTON_TIMEOUT_MS        10000  // time to wait on button press

static ExamType currentExamType = EXAM_TYPE_NONE;
bool slaveOneReady = false;
bool slaveTwoReady = false;
uint16_t lastPayload = 0x00;

typedef enum {
	EV_CONNECT_MSG_RECEIVED, 
	EV_DISCONNECT_MSG_RECEIVED, 
	EV_IDLE,
    EV_EXAM_SINGLE_SHOT,
    EV_EXAM_SERIES,
    EV_EXAM_SERIES_WITH_MOTION,
    EV_EXAM_FLUORO,
    EV_PREPARE_BUTTON_PRESSED,
    EV_NO_EVENT,
} EVENTS;

typedef enum {
    STATE_DISCONNECTED,
    STATE_CONNECTED,
    STATE_PREPARING,
    STATE_ACQUIRING
} CENTRAL_ACQ_STATES;

typedef enum {
	WAITING_FOR_MSG_START_SYMBOL, 
	WAITING_FOR_MSG_END_SYMBOL
} MSG_RECEIVE_STATE;

// Forward declarations
static bool writeMsgToSerialPort(const char msg[MAX_MSG_SIZE]);
bool checkForMsgOnSerialPort(char recieved_msg[MAX_MSG_SIZE]);
void EXAM_IDLE();
void EXAM_SINGLE_SHOT();
void EXAM_SERIES();
void EXAM_SERIES_WITH_MOTION();
void EXAM_FLOURO();
bool sendMessage(uint8_t slaveAddr, MsgType msgType, uint8_t msg);
bool debounceButton(uint8_t pin);

void handleEvent(EVENTS event) //Check state and handle incoming events
{
    static CENTRAL_ACQ_STATES centralAcqState = STATE_DISCONNECTED;

    switch (centralAcqState) {
        case STATE_DISCONNECTED:
            if (event == EV_CONNECT_MSG_RECEIVED) {
                centralAcqState = STATE_CONNECTED;
                writeMsgToSerialPort(CONNECT_MSG);
            }
            break;
        case STATE_CONNECTED:
            if (event == EV_CONNECT_MSG_RECEIVED) {
                writeMsgToSerialPort(CONNECT_MSG);
                break;
            }
            if (event == EV_DISCONNECT_MSG_RECEIVED) {
                centralAcqState = STATE_DISCONNECTED;
                writeMsgToSerialPort(DISCONNECT_MSG);
            } else if (event == EV_IDLE){
                currentExamType = EXAM_TYPE_NONE;
            } 
            else if (event == EV_EXAM_SINGLE_SHOT) {
                currentExamType = EXAM_TYPE_SINGLE_SHOT;
            } 
            else if (event == EV_EXAM_SERIES) {
                currentExamType = EXAM_TYPE_SERIES;
            }
            else if (event == EV_EXAM_SERIES_WITH_MOTION) {
                currentExamType = EXAM_TYPE_SERIES_WITH_MOTION;
                
            }
            else if (event == EV_EXAM_FLUORO) {
                currentExamType = EXAM_TYPE_FLUORO;
            }
            else if (event == EV_PREPARE_BUTTON_PRESSED && currentExamType != EXAM_TYPE_NONE){
                centralAcqState = STATE_PREPARING;
            }
            break;
            
        case STATE_PREPARING: {
            static unsigned long prepStart     = 0;
            static unsigned long lastBroadcast = 0;

            // Initialise timers on first entry to this state.
            if (prepStart == 0) {
                prepStart     = millis();
                lastBroadcast = 0;   // force an immediate first broadcast
            }

            if (event == EV_IDLE) {
                // PatientAdmin cancelled the exam while we were preparing
                Serial.println("[PREPARING] Exam cancelled by PatientAdmin");
                currentExamType = EXAM_TYPE_NONE;
                slaveOneReady = slaveTwoReady = false;
                prepStart = lastBroadcast = 0;
                centralAcqState = STATE_CONNECTED;
                break;
            }


            analogWrite(GRNLED, 100);
            analogWrite(REDLED, 100);

            if (millis() - lastBroadcast >= PREPARED_INTERVAL_MS) {
                lastBroadcast = millis();

                Serial.print("[PREPARING] Broadcasting exam 0x");
                Serial.println((uint8_t)currentExamType, HEX);

                if (!slaveOneReady) {
                    slaveOneReady = sendMessage(I2C_ADDR_SLAVE_1, MSG_START, currentExamType);
                }
                // Uncomment when Geometry (SLAVE_2) is implemented:
                // if (!slaveTwoReady) {
                //     slaveTwoReady = sendMessage(I2C_ADDR_SLAVE_2, MSG_START, currentExamType);
                // }
            }

            // ── Transition checks ─────────────────────────────────────────
            if (slaveOneReady || slaveTwoReady) {
                Serial.println("[PREPARING] Slave ready – moving to ACQUIRING");
                analogWrite(GRNLED, 255);
                analogWrite(REDLED, 0);
                centralAcqState = STATE_ACQUIRING;
                prepStart = lastBroadcast = 0;
            } else if (millis() - prepStart >= PREPARE_TIMEOUT_MS) {
                Serial.println("[PREPARING] TIMEOUT – no slave responded");
                analogWrite(GRNLED, 0);
                analogWrite(REDLED, 0);
                currentExamType = EXAM_TYPE_NONE;
                slaveOneReady = slaveTwoReady = false;
                centralAcqState = STATE_CONNECTED;
                prepStart = lastBroadcast = 0;
            }
        } break;

        case STATE_ACQUIRING: {
            analogWrite(REDLED, 255);
            analogWrite(GRNLED, 0);

            switch(currentExamType)
            {
                case EXAM_TYPE_NONE: { // MANAGE ERROR HANDLING TO THE REST
                } break;
                case EXAM_TYPE_SINGLE_SHOT: {
                    unsigned long int timeOut = millis();
                    while (debounceButton(XRAY_BUTTON_PIN) == false && millis() - timeOut <= BUTTON_TIMEOUT_MS){
                    }
                    if (digitalRead(XRAY_BUTTON_PIN) == LOW)
                    {
                        pinMode(SAN_PIN, OUTPUT);
                        digitalWrite(SAN_PIN, LOW);
                        delay(50);
                        pinMode(SAN_PIN, INPUT);
                    }
                    sendMessage(I2C_ADDR_SLAVE_1, MSG_STOP, 0x00);
                } break;
                case EXAM_TYPE_SERIES: {
                    unsigned long timeOut = millis();
                    while (debounceButton(XRAY_BUTTON_PIN) == false && millis() - timeOut <= BUTTON_TIMEOUT_MS){}

                    if (digitalRead(XRAY_BUTTON_PIN) == LOW) {
                        pinMode(SAN_PIN, OUTPUT);
                        digitalWrite(SAN_PIN, LOW);
                        while (digitalRead(XRAY_BUTTON_PIN) == LOW) { }
                        pinMode(SAN_PIN, INPUT);
                    }
                    sendMessage(I2C_ADDR_SLAVE_1, MSG_STOP, 0x00);
                } break;
                case EXAM_TYPE_FLUORO:{
                    unsigned long timeOut = millis();
                    while (debounceButton(XRAY_BUTTON_PIN) == false && millis() - timeOut <= BUTTON_TIMEOUT_MS){}

                    if (digitalRead(XRAY_BUTTON_PIN) == LOW) {
                        pinMode(SAN_PIN, OUTPUT);
                        digitalWrite(SAN_PIN, LOW);
                        while (digitalRead(XRAY_BUTTON_PIN) == LOW) { }
                    }
                    pinMode(SAN_PIN, INPUT);
                    sendMessage(I2C_ADDR_SLAVE_1, MSG_STOP, 0x00);
                } break;
                case EXAM_TYPE_SERIES_WITH_MOTION: {
                    // Geometry drives SAN here — master stays released.
                } break;
            }
            
            analogWrite(REDLED, 0);
            analogWrite(GRNLED, 0);
            centralAcqState = STATE_CONNECTED;
            slaveOneReady = false;
            slaveTwoReady = false;
        } break;

    }   
}

EVENTS getEvent() //Only checks whether a connect/disconnect message is recieved, and update state
{
    char msg[MAX_MSG_SIZE];
    if (checkForMsgOnSerialPort(msg)) {
        if (strcmp(msg, CONNECT_MSG) == 0){
            return EV_CONNECT_MSG_RECEIVED;
        }
        else if (strcmp(msg, DISCONNECT_MSG) == 0){
            return EV_DISCONNECT_MSG_RECEIVED;
        }
        else if (strncmp(msg, "EXAM", 4) == 0){
            switch (msg[5])
            {
            case '0': //IDLE
                return EV_IDLE;
            case '1': //SINGLE SHOT
                return EV_EXAM_SINGLE_SHOT;
            case '2':
                return EV_EXAM_SERIES;
            case '3':
                return EV_EXAM_SERIES_WITH_MOTION;
            case '4':
                return EV_EXAM_FLUORO;
            }
        }
    }
    if (debounceButton(PREPARE_BUTTON_PIN) == true){
        return EV_PREPARE_BUTTON_PRESSED;
    }
    return EV_NO_EVENT;
}

static bool writeMsgToSerialPort(const char msg[MAX_MSG_SIZE]) //Write a string array to the serial port
{
	Serial.write(MSG_START_SYMBOL);
    for (unsigned int i = 0; i < MAX_MSG_SIZE && msg[i] != '\0'; i++) {
		Serial.write(msg[i]);
	}
	Serial.write(MSG_END_SYMBOL);
	return true;
}

bool checkForMsgOnSerialPort(char recieved_msg[MAX_MSG_SIZE])
{
    static int receiveIndex = 0;
    static MSG_RECEIVE_STATE msgRcvState = WAITING_FOR_MSG_START_SYMBOL;
    static char msg[MAX_MSG_SIZE] {0};

    if (receiveIndex >= MAX_MSG_SIZE - 1) {
        receiveIndex = 0;
        msgRcvState = WAITING_FOR_MSG_START_SYMBOL;
    }

    if (Serial.available() > 0) {
        char receivedChar = Serial.read(); 
		switch (msgRcvState) {
			case WAITING_FOR_MSG_START_SYMBOL:
				if (receivedChar == MSG_START_SYMBOL) {
					receiveIndex = 0;
					msgRcvState = WAITING_FOR_MSG_END_SYMBOL;
				}
				break;
			case WAITING_FOR_MSG_END_SYMBOL:
				if (receivedChar == MSG_END_SYMBOL) {
					msg[receiveIndex] = '\0';
                    receiveIndex = 0;
                    strncpy(recieved_msg, msg, MAX_MSG_SIZE);  
					msgRcvState = WAITING_FOR_MSG_START_SYMBOL;
					return true;
				}
				else msg[receiveIndex++] = receivedChar;
				break;
			default:
				break;
		}
	}
    return false;
}

void setup() {
    Serial.begin(9600);
    Wire.begin();

    pinMode(REDLED, OUTPUT);
    pinMode(GRNLED, OUTPUT);
    pinMode(PREPARE_BUTTON_PIN, INPUT);
    pinMode(XRAY_BUTTON_PIN, INPUT_PULLUP);
    pinMode(SAN_PIN, INPUT);
}

void loop() {
    handleEvent(getEvent());


    // The XrayGenerator pulls SAN LOW to flag a dose is ready; request it once.
    if (digitalRead(SAN_PIN) == LOW) {
        sendMessage(I2C_ADDR_SLAVE_1, MSG_DATA_READY, 0x00);

        if (lastPayload != 0x00){
            char doseMsg[16];
            snprintf(doseMsg, sizeof(doseMsg), "DOSE:%u", lastPayload);
            writeMsgToSerialPort(doseMsg);
            Serial.print("DOSE:"); Serial.println(lastPayload);
            lastPayload = 0x00;
        }
    }
}

// debounceButton – per-pin debounce.
// Each pin tracks its own last-press timestamp so PREPARE_BUTTON and
// XRAY_BUTTON never interfere with each other.
bool debounceButton(uint8_t pin) {
    static unsigned long lastPressMap[20] = {0};   // one slot per Arduino pin
    if (pin >= 20) return false;
    if (millis() - lastPressMap[pin] >= 20) {
        if (digitalRead(pin) == LOW) {
            lastPressMap[pin] = millis();
            return true;
        }
    }
    return false;
}


bool sendMessage(uint8_t slaveAddr, MsgType msgType, uint8_t msg) {
    for (uint8_t i = 0; i < MAX_RETRIES; i++) {
        Serial.print("Type: "); Serial.print(msgType, HEX);

        Wire.beginTransmission(slaveAddr);
        Wire.write((uint8_t)msgType);
        Wire.write(msg);
        Wire.endTransmission();

        delay(100);

        Wire.requestFrom((uint8_t)slaveAddr, (uint8_t)3);

        unsigned long waitStart = millis();
        while (Wire.available() < 3 && millis() - waitStart < I2C_RESPONSE_TIMEOUT_MS) {
            // wait shortly for the slave to send data
        }

        if (Wire.available() == 3) {
            uint8_t response  = Wire.read();   // byte 1: ACK / NAK
            uint8_t payloadHi = Wire.read();   // byte 2: payload high
            uint8_t payloadLo = Wire.read();   // byte 3: payload low

            if (msgType == MSG_DATA_READY){
                lastPayload = ((uint16_t)payloadHi << 8) | payloadLo;
            }
            if (response == ACK) {
                Serial.println("\tACK");
                if (lastPayload != 0x00) {
                    Serial.print("MESSAGE: 0x"); Serial.println(lastPayload, HEX);
                }
                return true;
            } else if (response == NAK) {
                Serial.println("NAK, slave not ready yet");
            } else {
                Serial.println("UNKNOWN RESPONSE");
            }
        } else {
            Serial.println("NO RESPONSE, DEVICE MAY BE UNAVAILABLE");
        }

        delay(100);
    }

    Serial.println("Failed after retries exceeded");
    return false;
}