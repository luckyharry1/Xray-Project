#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "../../Interface_PatAdmin_CentralAcq/Protocol_PatientAdmin_CentralAcq.h"
#include "COMM_PROTOCOL.h"

#define MAX_RETRIES 3

#define GRNLED 5
#define REDLED 6
#define SAN_PIN 2
#define PREPARE_BUTTON_PIN 8
#define XRAY_BUTTON_PIN 9

static ExamType currentExamType = EXAM_TYPE_NONE;
bool slaveOneReady = false;
bool slaveTwoReady = false;
uint8_t lastPayload = 0x00;

typedef enum {
	EV_CONNECT_MSG_RECEIVED, 
	EV_DISCONNECT_MSG_RECEIVED, 
	EV_IDLE,
    EV_EXAM_SINGLE_SHOT,
    EV_EXAM_SERIES,
    EV_EXAM_SERIES_WITH_MOTION,
    EV_EXAM_FLUORO,
    EV_PREPARE_BUTTON_PRESSED,
    EV_XRAY_BUTTON_PRESSED,
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
            if (event == EV_DISCONNECT_MSG_RECEIVED) {
                centralAcqState = STATE_DISCONNECTED;
                writeMsgToSerialPort(DISCONNECT_MSG);
            } else if (event == EV_IDLE){
                centralAcqState = STATE_PREPARING;
                currentExamType = EXAM_TYPE_NONE;
            } 
            else if (event == EV_EXAM_SINGLE_SHOT) {
                centralAcqState = STATE_PREPARING;
                currentExamType = EXAM_TYPE_SINGLE_SHOT;
            } 
            else if (event == EV_EXAM_SERIES) {
                centralAcqState = STATE_PREPARING;
                currentExamType = EXAM_TYPE_SERIES;
            }
            else if (event == EV_EXAM_SERIES_WITH_MOTION) {
                centralAcqState = STATE_PREPARING;
                currentExamType = EXAM_TYPE_SERIES_WITH_MOTION;
                
            }
            else if (event == EV_EXAM_FLUORO) {
                centralAcqState = STATE_PREPARING;
                currentExamType = EXAM_TYPE_FLUORO;
            }
            else if (event == EV_PREPARE_BUTTON_PRESSED && currentExamType != EXAM_TYPE_NONE){
                pinMode(SAN_PIN, OUTPUT);

                analogWrite(GRNLED, 255);
                analogWrite(REDLED, 0);

                sendMessage(I2C_ADDR_SLAVE_1, MSG_START, currentExamType);

                centralAcqState = STATE_PREPARING;
            }
            break;
        case STATE_PREPARING:
            analogWrite(GRNLED, 100);
            analogWrite(REDLED, 100);
            if (sendMessage(I2C_ADDR_SLAVE_1, MSG_START, currentExamType) == true){
                slaveOneReady = true;
            }
            if (sendMessage(I2C_ADDR_SLAVE_2, MSG_START, currentExamType) == true){
                slaveTwoReady = true;
            }
            if (slaveOneReady || slaveTwoReady){
                centralAcqState = STATE_ACQUIRING;
            }
            if (event == EV_PREPARE_BUTTON_PRESSED){
                if (event == EV_IDLE){
                    analogWrite(GRNLED, 0);
                    analogWrite(REDLED, 0);
                    currentExamType = EXAM_TYPE_SERIES;
                    centralAcqState = STATE_CONNECTED;
                    break;
                }
                centralAcqState = STATE_ACQUIRING;
            }
            break;

        case STATE_ACQUIRING:
            pinMode(SAN_PIN, OUTPUT);
            analogWrite(REDLED, 255);
            analogWrite(GRNLED, 0);
            switch(currentExamType)
            {
                case EXAM_TYPE_NONE: // MANAGE ERROR HANDLING TO THE REST
                break;
                case EXAM_TYPE_SINGLE_SHOT:
                    if (digitalRead(XRAY_BUTTON_PIN) == LOW)
                    {
                        digitalWrite(SAN_PIN, LOW);
                        delay(50);
                        digitalWrite(SAN_PIN, HIGH);
                    }
                    break;
                case EXAM_TYPE_SERIES:
                    while (digitalRead(XRAY_BUTTON_PIN) == LOW)
                    {
                        digitalWrite(SAN_PIN, LOW);
                    }
                    digitalWrite(SAN_PIN, HIGH);
                    break;
                case EXAM_TYPE_SERIES_WITH_MOTION: // FIGURE OUT LOGIC FOR WHEN DONE
                    pinMode(SAN_PIN, INPUT);
                    
                    break;
                case EXAM_TYPE_FLUORO:
                    while (digitalRead(XRAY_BUTTON_PIN) == LOW)
                    {
                        digitalWrite(SAN_PIN, LOW);
                    }
                    digitalWrite(SAN_PIN, HIGH);
                    break;
                break;

            }
            
            sendMessage(I2C_ADDR_SLAVE_1, MSG_STOP, 0x00);
            pinMode(SAN_PIN, INPUT);
            while (lastPayload == 0x00){
                sendMessage(I2C_ADDR_SLAVE_1, MSG_DATA_READY, 0x00);
            }
            writeMsgToSerialPort("DOSE:"); // INT TO STRING
            Serial.println("DOSE:" + lastPayload);
            centralAcqState = STATE_CONNECTED; // LOGIC FOR RETURNING READINGS
            slaveOneReady = false;
            slaveTwoReady = false;
            break;
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
    if (debounceButton(XRAY_BUTTON_PIN) == true){
        return EV_XRAY_BUTTON_PRESSED;
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
    pinMode(PREPARE_BUTTON_PIN, INPUT_PULLUP);
    pinMode(XRAY_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
    handleEvent(getEvent());

    // below some dummy code that sends dose data to the patient admin. Remove this dummy code asap.
    static unsigned long timeOut = millis();
    static int doseCnt = 0;
    unsigned long curTime = millis();
    if (curTime > timeOut) {
        Serial.print("$DOSE:"); Serial.print(doseCnt); Serial.println("#");
        timeOut = curTime + 5000;
        doseCnt++;
    }
}

bool debounceButton(uint8_t pin) {
    if (digitalRead(pin) == LOW) {
        delay(20);
        return true;
    }
    return false;
}


bool sendMessage(uint8_t slaveAddr, MsgType msgType, uint8_t msg) {
    for (uint8_t i = 0; i < MAX_RETRIES; i++){
    //Serial.print("ATTEMPTING #"); Serial.println(i);
        Serial.print("Type: "); Serial.print(msgType, HEX);
        //Serial.print("Message: "); Serial.println(msg);
        
        Wire.beginTransmission(slaveAddr);
        Wire.write((uint8_t)msgType);
        Wire.write(msg);
        Wire.endTransmission();

        delay(100);

        bool recieveStatus = false;
        while(recieveStatus == false){

            Wire.requestFrom((uint8_t)slaveAddr, (uint8_t)2);

            if (Wire.available() >= 2) {
                uint8_t response = Wire.read();
                lastPayload = Wire.read();
                if (response == ACK){
                    Serial.println("\tACK");
                    recieveStatus = true;

                    if (lastPayload != 0x00){
                        Serial.print("MESSAGE: 0x"); Serial.println(lastPayload, HEX);
                    }
                    return true;
                } else if (response == NAK){
                    Serial.println("NAK, slave not ready yet");
                    recieveStatus = false;
                } else {
                    Serial.println("UNKNOWN RESPONSE");
                    recieveStatus = false;
                }

            Serial.println("ACK NOT RECIEVED");
            
            } else {
                Serial.println("NO RESPONSE, DEVICE MAY BE UNAVALIABLE");
            }

            delay(100);
        }
    }

    Serial.println("Failed after retries exceeded");
    return false;
}