#ifndef COMM_PROTOCOL_H
#define COMM_PROTOCOL_H

// Shared I2C protocol between the master (CentralAcquisition) and the slave
// devices (XrayGenerator, Geometry). Include this from each side via a
// relative path — it is the single source of truth for message types, exam
// types, and I2C addresses. The Linux PatientAdmin protocol header also
// pulls this in so EXAM_TYPE_* constants stay in sync.

// I2C ADDRESSES
#define I2C_ADDR_MASTER   0x01
#define I2C_ADDR_SLAVE_1  0x02   // XrayGenerator
#define I2C_ADDR_SLAVE_2  0x03   // Geometry

#define ACK 0x06
#define NAK 0x07

typedef enum {
  MSG_HEARTBEAT  = 0x1A, // sender is alive
  MSG_START      = 0x1B,
  MSG_STOP       = 0x1C,
  MSG_ERROR      = 0x1D,
  MSG_ACK        = 0x1E,
  MSG_DATA_READY = 0x1F,
  MSG_UNKNOWN    = 0xFF,
} MsgType;

typedef enum {
  EXAM_TYPE_NONE               = 0x00,
  EXAM_TYPE_SINGLE_SHOT        = 0x01,
  EXAM_TYPE_SERIES             = 0x02,
  EXAM_TYPE_SERIES_WITH_MOTION = 0x03,
  EXAM_TYPE_FLUORO             = 0x04,
} ExamType;

typedef enum {
  ERR_UNKOWN    = 0x01,
  ERR_NOT_READY = 0x02,
} ErrorCode;

#endif // COMM_PROTOCOL_H
