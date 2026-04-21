#include "COMM_PROTOCOL.h"
#include <Wire.h>

#define MAX_RETRIES 3

static lastPayload = 0x00;

void setup() {
  Serial.begin(9600);
  Wire.begin();

}

//void sendMessage(uint8_t slaveAddr, MsgType, msg);

void loop() {
  sendMessage(I2C_ADDR_SLAVE_1, MSG_START, 0x00);
  delay(1000);
  sendMessage(I2C_ADDR_SLAVE_1, MSG_DATA_READY, 0x00);
  delay(1000);
}


bool sendMessage(uint8_t slaveAddr, MsgType msgType, uint16_t msg) {
  for (uint8_t i = 0; i < MAX_RETRIES; i++){
    //Serial.print("ATTEMPTING #"); Serial.println(i);
    Serial.print("Type: "); Serial.print(msgType, HEX);
    //Serial.print("Message: "); Serial.println(msg);
    
    Wire.beginTransmission(slaveAddr);
    Wire.write((uint8_t)msgType);
    Wire.write(msg);
    Wire.endTransmission();

    Wire.requestFrom(slaveAddr, 2);
    if (Wire.available() >= 2) {
      uint8_t response = Wire.read();
      uint8_t payload  = Wire.read();
      if (response == ACK){
        Serial.println("\t ACK");
        if (payload != 0x00){
          Serial.print("MESSAGE: 0x"); Serial.println(payload, HEX);
        }
        return true;
      } else {
        Serial.println("ACK NOT RECIEVED");
      }
    } else {
      Serial.println("NO RESPONSE, DEVICE MAY BE UNAVALIABLE");
    }

    delay(50);
  }

  Serial.println("Failed after retries exceeded");
  return false;
}


