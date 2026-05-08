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

typedef enum {
	EV_CONNECT_MSG_RECEIVED, 
	EV_DISCONNECT_MSG_RECEIVED, 
	EV_IDLE,
    EV_EXAM_SINGLE_SHOT,
    EV_EXAM_SERIES,
    EV_EXAM_SERIES_WITH_MOTION,
    EV_EXAM_FLUORO,
    EV_NO_EVENT,
} EVENTS;

typedef enum {
    STATE_DISCONNECTED,
    STATE_CONNECTED,
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
                EXAM_IDLE();
            } 
            else if (event == EV_EXAM_SINGLE_SHOT) {
                EXAM_SINGLE_SHOT();
            } 
            else if (event == EV_EXAM_SERIES) {
                EXAM_SERIES();
            }
            else if (event == EV_EXAM_SERIES_WITH_MOTION) {
                EXAM_SERIES_WITH_MOTION();
            }
            else if (event == EV_EXAM_FLUORO) {
                EXAM_FLOURO();
            }
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
    return EV_IDLE;
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

void EXAM_IDLE(){
    if (sendMessage(I2C_ADDR_SLAVE_1, MSG_START, EXAM_TYPE_NONE) != true){
        return;
    }
    //sendMessage(I2C_ADDR_SLAVE_2, MSG_START, EXAM_TYPE_NONE);
    analogWrite(GRNLED, 255);
    analogWrite(REDLED, 0);
}

void EXAM_SINGLE_SHOT(){
    analogWrite(GRNLED, 0);
    analogWrite(REDLED, 255);
    if (sendMessage(I2C_ADDR_SLAVE_1, MSG_START, EXAM_TYPE_SINGLE_SHOT) != true){
        return;
    }

    analogWrite(GRNLED, 0);
    analogWrite(REDLED, 255);
    delay(5000);
}

void EXAM_SERIES(){
    analogWrite(GRNLED, 0);
    analogWrite(REDLED, 255);
    if (sendMessage(I2C_ADDR_SLAVE_1, MSG_START, EXAM_TYPE_SERIES) != true){
        return;
    }

    analogWrite(GRNLED, 0);
    analogWrite(REDLED, 255);
}

void EXAM_SERIES_WITH_MOTION(){
    analogWrite(GRNLED, 0);
    analogWrite(REDLED, 255);
    if (sendMessage(I2C_ADDR_SLAVE_1, MSG_START, EXAM_TYPE_SERIES_WITH_MOTION) != true){
        return;
    }

    analogWrite(GRNLED, 0);
    analogWrite(REDLED, 255);
}

void EXAM_FLOURO(){
    analogWrite(GRNLED, 0);
    analogWrite(REDLED, 255);
    if (sendMessage(I2C_ADDR_SLAVE_1, MSG_START, EXAM_TYPE_FLUORO) != true){
        return;
    }

    analogWrite(GRNLED, 0);
    analogWrite(REDLED, 255);
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
                uint8_t payload  = Wire.read();
                if (response == ACK){
                    Serial.println("\tACK");
                    recieveStatus = true;

                    if (payload != 0x00){
                        Serial.print("MESSAGE: 0x"); Serial.println(payload, HEX);
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