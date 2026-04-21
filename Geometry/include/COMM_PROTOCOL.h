#ifndef COMM_PROTOCOL_H
#define COMM_PROTOCOL_H


//I2C ADRESSES
#define I2C_ADDR_MASTER   0x01
#define I2C_ADDR_SLAVE_1  0x02
#define I2C_ADDR_SLAVE_2  0x03

#define ACK   0x06
#define NAK   0x07

typedef enum {
  MSG_HEARTBEAT = 0x1A, //sender is alive
  MSG_START     = 0x1B, 
  MSG_STOP      = 0x1C,
  MSG_ERROR     = 0x1D,
  MSG_ACK       = 0x1E,
  MSG_DATA_READY= 0x1F,
  MSG_UNKNOWN   = 0xFF,
} MsgType;

typedef enum {
  ERR_UNKOWN   = 0x01
} ErrorCode;

#endif COMM_PROTOCOL_H