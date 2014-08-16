#ifndef TwiState_h
#define TwiState_h
/****************************************************************************
  Edited version of Atmel TWI State Codes from:
  AppNote           : AVR315 - TWI Master Implementation
  
  Modified 2013 by William Greiman for RTOS use
  Undefined State Codes: 0XD8, 0XE0, 0XE8, 0XF0
****************************************************************************/
 /**
 * @file    TwiState.h
 * @brief   Two Wire state codes.
 *
 * @defgroup twoWire TwiMaster
 * @details Nil RTOS Two Wire Interface library.
 * @{
 */
// General TWI Master state codes

/** START has been transmitted. */
#define TWI_START                  0x08

/** Repeated START has been transmitted. */
#define TWI_REP_START              0x10

/** Arbitration lost. */
#define TWI_ARB_LOST               0x38

// TWI Master Transmitter state codes

/** SLA+W has been transmitted and ACK received. */
#define TWI_MTX_ADR_ACK            0x18

/** SLA+W has been transmitted and NACK received. */
#define TWI_MTX_ADR_NACK           0x20

/** Data byte has been transmitted and ACK received. */
#define TWI_MTX_DATA_ACK           0x28

/** Data byte has been transmitted and NACK received. */
#define TWI_MTX_DATA_NACK          0x30

// TWI Master Receiver state codes

/** SLA+R has been transmitted and ACK received. */
#define TWI_MRX_ADR_ACK            0x40

/** SLA+R has been transmitted and NACK received. */
#define TWI_MRX_ADR_NACK           0x48

/** Data byte has been received and ACK transmitted. */
#define TWI_MRX_DATA_ACK           0x50

/** Data byte has been received and NACK transmitted. */
#define TWI_MRX_DATA_NACK          0x58

// TWI Slave Transmitter state codes

/** Own SLA+R has been received; ACK has been returned. */
#define TWI_STX_ADR_ACK            0xA8

/** Arbitration lost in SLA+R/W as Master; own SLA+R has
    been received; ACK has been returned. */
#define TWI_STX_ADR_ACK_M_ARB_LOST 0xB0

/** Data byte in TWDR has been transmitted; ACK has been received. */
#define TWI_STX_DATA_ACK           0xB8

/** Data byte in TWDR has been transmitted; NOT ACK has been received. */
#define TWI_STX_DATA_NACK          0xC0

/** Last data byte in TWDR has been transmitted (TWEA = “0”);
    ACK has been received. */
#define TWI_STX_DATA_ACK_LAST_BYTE 0xC8

// TWI Slave Receiver state codes

/** Own SLA+W has been received ACK has been returned. */
#define TWI_SRX_ADR_ACK            0x60

/** Arbitration lost in SLA+R/W as Master; own SLA+W has been received;
    ACK has been returned. */
#define TWI_SRX_ADR_ACK_M_ARB_LOST 0x68

/** General call address has been received; ACK has been returned. */
#define TWI_SRX_GEN_ACK            0x70

/** Arbitration lost in SLA+R/W as Master; General call address has
    been received; ACK has been returned. */
#define TWI_SRX_GEN_ACK_M_ARB_LOST 0x78

/** Previously addressed with own SLA+W; data has been received;
    ACK has been returned. */
#define TWI_SRX_ADR_DATA_ACK       0x80

/** Previously addressed with own SLA+W; data has been received;
    NOT ACK has been returned. */
#define TWI_SRX_ADR_DATA_NACK      0x88

/** Previously addressed with general call; data has been received;
    ACK has been returned. */
#define TWI_SRX_GEN_DATA_ACK       0x90

/** Previously addressed with general call; data has been received;
    NOT ACK has been returned. */
#define TWI_SRX_GEN_DATA_NACK      0x98

/** A STOP condition or repeated START condition has been received
    while still addressed as Slave. */
#define TWI_SRX_STOP_RESTART       0xA0

// TWI Miscellaneous state codes

/** No relevant state information available; TWINT = “0”. */
#define TWI_NO_STATE               0xF8

/** Bus error due to an illegal START or STOP condition. */
#define TWI_BUS_ERROR              0x00

// Software error states

/** Timeout occurred for a start consition command. */
#define TWI_START_TIMEOUT          0xD0  // 
#endif  // TwiState_h
/** @} */