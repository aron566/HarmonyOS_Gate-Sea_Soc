/*
 * Copyright (c) 2022 Hunan OpenValley Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http:// www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __FM17622_REG_H__
#define __FM17622_REG_H__

#define FM17622_DEV_ADDR 0x50 // FM17622设备地址

#define TX1_TX2_CW_DISABLE 0
#define TX1_CW_ENABLE 1
#define TX2_CW_ENABLE 2
#define TX1_TX2_CW_ENABLE 3

#define JREG_PAGE0 0x00      // Page register in page 0
#define JREG_COMMAND 0x01    // Contains Command bits, PowerDown bit and bit to switch receiver off.
#define JREG_COMMIEN 0x02    // Contains Communication interrupt enable bits andbit for Interrupt inversion.
// Contains RfOn, RfOff, CRC and Mode Interrupt enable and bit to switch Interrupt pin to PushPull mode.
#define JREG_DIVIEN 0x03
#define JREG_COMMIRQ 0x04    // Contains Communication interrupt request bits.
#define JREG_DIVIRQ 0x05     // Contains RfOn, RfOff, CRC and Mode Interrupt request.
// Contains Protocol, Parity, CRC, Collision, Buffer overflow, Temperature and RF error flags.
#define JREG_ERROR 0x06
// Contains status information about Lo- and HiAlert, RF-field on, Timer, Interrupt request and CRC status.
#define JREG_STATUS1 0x07
// Contains information about internal states (Modemstate),
// Mifare states and possibility to switch Temperature sensor off.
#define JREG_STATUS2 0x08
// Gives access to FIFO. Writing to register increments theFIFO level (register 0x0A), reading decrements it.
#define JREG_FIFODATA 0x09
#define JREG_FIFOLEVEL 0x0A  // Contains the actual level of the FIFO.
#define JREG_WATERLEVEL 0x0B // Contains the Waterlevel value for the FIFO
// Contains information about last received bits, Initiator mode bit,
// bit to copy NFCID to FIFO and to Start and stopthe Timer unit.
#define JREG_CONTROL 0x0C
// Contains information of last bits to send, to align received bits in FIFO and activate sending in Transceive
#define JREG_BITFRAMING 0x0D
#define JREG_COLL 0x0E       // Contains all necessary bits for Collission handling
#define JREG_RFU0F 0x0F      // Currently not used.

#define JREG_PAGE1 0x10       // Page register in page 1
// Contains bits for auto wait on Rf, to detect SYNC byte in NFC mode and MSB first for CRC calculation
#define JREG_MODE 0x11
#define JREG_TXMODE 0x12      // Contains Transmit Framing, Speed, CRC enable, bit for inverse mode and TXMix bit.
// Contains Transmit Framing, Speed, CRC enable, bit for multiple receive and to filter errors.
#define JREG_RXMODE 0x13
// Contains bits to activate and configure Tx1 and Tx2 and bit to activate 100% modulation.
#define JREG_TXCONTROL 0x14
// Contains bits to automatically switch on/off the Rf and to do the collission avoidance and the initial rf-on.
#define JREG_TXAUTO 0x15
#define JREG_TXSEL 0x16       // Contains SigoutSel, DriverSel and LoadModSel bits.
#define JREG_RXSEL 0x17       // Contains UartSel and RxWait bits.
#define JREG_RXTRESHOLD 0x18  // Contains MinLevel and CollLevel for detection.
#define JREG_DEMOD 0x19       // Contains bits for time constants, hysteresis and IQ demodulator settings.
// Contains bits for minimum FeliCa length received and for FeliCa syncronisation length.
#define JREG_FELICANFC 0x1A
#define JREG_FELICANFC2 0x1B  // Contains bits for maximum FeliCa length received.
#define JREG_MIFARE 0x1C      // Contains Miller settings, TxWait settings and MIFARE halted mode bit.
#define JREG_MANUALRCV 0x1D   // Currently not used.
#define JREG_RFU1E 0x1E       // Currently not used.
#define JREG_SERIALSPEED 0x1F // Contains speed settings for serila interface.

#define JREG_PAGE2 0x20         // Page register in page 2
#define JREG_CRCRESULT1 0x21    // Contains MSByte of CRC Result.
#define JREG_CRCRESULT2 0x22    // Contains LSByte of CRC Result.

// Contains the conductance and the modulation settings for the N-MOS transistor
// only for load modulation (See difference to JREG_GSN!).
#define JREG_GSNLOADMOD 0x23
#define JREG_MODWIDTH 0x24      // Contains modulation width setting.
#define JREG_TXBITPHASE 0x25    // Contains TxBitphase settings and receive clock change.
// Contains sensitivity of Rf Level detector, the receiver gain factor and the RfLevelAmp.
#define JREG_RFCFG 0x26
// Contains the conductance and the modulation settings for
// the N-MOS transistor during active modulation (no load modulation setting!).
#define JREG_GSN 0x27
#define JREG_CWGSP 0x28         // Contains the conductance for the P-Mos transistor.
#define JREG_MODGSP 0x29        // Contains the modulation index for the PMos transistor.
#define JREG_TMODE 0x2A         // Contains all settings for the timer and the highest 4 bits of the prescaler.
#define JREG_TPRESCALER 0x2B    // Contais the lowest byte of the prescaler.
#define JREG_TRELOADHI 0x2C     // Contains the high byte of the reload value.
#define JREG_TRELOADLO 0x2D     // Contains the low byte of the reload value.
#define JREG_TCOUNTERVALHI 0x2E // Contains the high byte of the counter value.
#define JREG_TCOUNTERVALLO 0x2F // Contains the low byte of the counter value.

#define JREG_PAGE3 0x30         // Page register in page 3
#define JREG_TESTSEL1 0x31      // Test register
#define JREG_TESTSEL2 0x32      // Test register
#define JREG_TESTPINEN 0x33     // Test register
#define JREG_TESTPINVALUE 0x34  // Test register
#define JREG_TESTBUS 0x35       // Test register
#define JREG_AUTOTEST 0x36      // Test register
#define JREG_VERSION 0x37       // Contains the product number and the version .
#define JREG_ANALOGTEST 0x38    // Test register
#define JREG_TESTSUP1 0x39      // Test register
#define JREG_TESTSUP2 0x3A      // Test register
#define JREG_TESTADC 0x3B       // Test register
#define JREG_ANALOGUETEST1 0x3C // Test register
#define JREG_ANALOGUETEST0 0x3D // Test register
#define JREG_ANALOGUETPD_A 0x3E // Test register
#define JREG_ANALOGUETPD_B 0x3F // Test register

#define CMD_MASK 0xF0

#define CMD_IDLE 0x00
#define CMD_CALC_CRC 0x03
#define CMD_TRANSMIT 0x04
#define CMD_NOCMDCHANGE 0x07
#define CMD_RECEIVE 0x08
#define CMD_TRANSCEIVE 0x0C
#define CMD_AUTOCOLL 0x0D
#define CMD_AUTHENT 0x0E
#define CMD_SOFT_RESET 0x0F

/* // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // /
 * Bit Definitions
 * // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // */
/* name FM175XX Bit definitions of Page 0 */
/* Command Register (01) */
#define JBIT_RCVOFF 0x20    /* Switches the receiver on/off. */
#define JBIT_POWERDOWN 0x10 /* Switches FM175XX to Power Down mode. */

/* CommIEn Register (02) */
#define JBIT_IRQINV 0x80 /* Inverts the output of IRQ Pin. */

/* DivIEn Register  (03) */
#define JBIT_IRQPUSHPULL 0x80 /* Sets the IRQ pin to Push Pull mode. */

/* CommIEn and CommIrq Register         (02, 04) */
#define JBIT_TXI 0x40      /* Bit position for Transmit Interrupt Enable/Request. */
#define JBIT_RXI 0x20      /* Bit position for Receive Interrupt Enable/Request. */
#define JBIT_IDLEI 0x10    /* Bit position for Idle Interrupt Enable/Request.    */
#define JBIT_HIALERTI 0x08 /* Bit position for HiAlert Interrupt Enable/Request. */
#define JBIT_LOALERTI 0x04 /* Bit position for LoAlert Interrupt Enable/Request. */
#define JBIT_ERRI 0x02     /* Bit position for Error Interrupt Enable/Request.   */
#define JBIT_TIMERI 0x01   /* Bit position for Timer Interrupt Enable/Request.   */

/* DivIEn and DivIrq Register           (03, 05) */
#define JBIT_SIGINACT 0x10 /* Bit position for SiginAct Interrupt Enable/Request. */
#define JBIT_MODEI 0x08    /* Bit position for Mode Interrupt Enable/Request. */
#define JBIT_CRCI 0x04     /* Bit position for CRC Interrupt Enable/Request. */
#define JBIT_RFONI 0x02    /* Bit position for RF On Interrupt Enable/Request. */
#define JBIT_RFOFFI 0x01   /* Bit position for RF Off Interrupt Enable/Request. */

/* CommIrq and DivIrq Register          (04, 05) */
#define JBIT_SET 0x80 /* Bit position to set/clear dedicated IRQ bits. */

/* Error Register        (06) */
#define JBIT_WRERR 0x40      /* Bit position for Write Access Error. */
#define JBIT_TEMPERR 0x40    /* Bit position for Temerature Error. */
#define JBIT_RFERR 0x20      /* Bit position for RF Error. */
#define JBIT_BUFFEROVFL 0x10 /* Bit position for Buffer Overflow Error. */
#define JBIT_COLLERR 0x08    /* Bit position for Collision Error. */
#define JBIT_CRCERR 0x04     /* Bit position for CRC Error. */
#define JBIT_PARITYERR 0x02  /* Bit position for Parity Error. */
#define JBIT_PROTERR 0x01    /* Bit position for Protocol Error. */

/* Status 1 Register       (07) */
#define JBIT_CRCOK 0x40    /* Bit position for status CRC OK. */
#define JBIT_CRCREADY 0x20 /* Bit position for status CRC Ready. */
#define JBIT_IRQ 0x10      /* Bit position for status IRQ is active. */
#define JBIT_TRUNNUNG 0x08 /* Bit position for status Timer is running. */
#define JBIT_RFON 0x04     /* Bit position for status RF is on/off. */
#define JBIT_HIALERT 0x02  /* Bit position for status HiAlert. */
#define JBIT_LOALERT 0x01  /* Bit position for status LoAlert. */

/* Status 2 Register         (08) */
#define JBIT_TEMPSENSOFF 0x80 /* Bit position to switch Temperture sensors on/off. */
#define JBIT_I2CFORCEHS 0x40  /* Bit position to forece High speed mode for I2C Interface. */
#define JBIT_MFSELECTED 0x10  /* Bit position for card status Mifare selected. */
#define JBIT_CRYPTO1ON 0x08   /* Bit position for reader status Crypto is on. */

/* FIFOLevel Register         (0A) */
#define JBIT_FLUSHFIFO 0x80 /* Clears FIFO buffer if set to 1 */

/* Control Register          (0C) */
#define JBIT_TSTOPNOW 0x80      /* Stops timer if set to 1. */
#define JBIT_TSTARTNOW 0x40     /* Starts timer if set to 1. */
#define JBIT_WRNFCIDTOFIFO 0x20 /* Copies internal stored NFCID3 to actual position of FIFO. */
#define JBIT_INITIATOR 0x10     /* Sets Initiator mode. */

/* BitFraming Register         (0D) */
#define JBIT_STARTSEND 0x80 /* Starts transmission in transceive command if set to 1. */

/* BitFraming Register         (0E) */
#define JBIT_VALUESAFTERCOLL 0x80 /* Activates mode to keep data after collision. */

/* name FM175XX Bit definitions of Page 1
 *  Below there are useful bit definition of the FM175XX register set of Page 1. */
/* Mode Register       (11) */
#define JBIT_MSBFIRST 0x80   /* Sets CRC coprocessor with MSB first. */
#define JBIT_DETECTSYNC 0x40 /* Activate automatic sync detection for NFC 106kbps. */
#define JBIT_TXWAITRF 0x20   /* Tx waits until Rf is enabled until transmit is startet, else \
                                  transmit is started immideately. */
#define JBIT_RXWAITRF 0x10   /* Rx waits until Rf is enabled until receive is startet, else \
                                  receive is started immideately. */
#define JBIT_POLSIGIN 0x08   /* Inverts polarity of SiginActIrq, if bit is set to 1 IRQ occures \
                                  when Sigin line is 0. */
#define JBIT_MODEDETOFF 0x04 /* Deactivates the ModeDetector during AutoAnticoll command. The settings \
                                  of the register are valid only. */

/* TxMode Register       (12) */
#define JBIT_TXCRCEN 0x80 /* enables the CRC generation during data transmissio. */
#define JBIT_INVMOD 0x08  /* Activates inverted transmission mode. */
#define JBIT_TXMIX 0x04   /* Activates TXMix functionality. */

/* RxMode Register       (13) */
#define JBIT_RXCRCEN 0x80    /* enables the CRC generation during reception.. */
#define JBIT_RXNOERR 0x08    /* If 1, receiver does not receive less than 4 bits. */
#define JBIT_RXMULTIPLE 0x04 /* Activates reception mode for multiple responses. */

/* Definitions for Tx and Rx      (12, 13) */
#define JBIT_106KBPS 0x00 /* Activates speed of 106kbps. */
#define JBIT_212KBPS 0x10 /* Activates speed of 212kbps. */
#define JBIT_424KBPS 0x20 /* Activates speed of 424kbps. */
#define JBIT_848KBPS 0x30 /* Activates speed of 848kbps. */
#define JBIT_1_6MBPS 0x40 /* Activates speed of 1.6Mbps. */
#define JBIT_3_2MBPS 0x50 /* Activates speed of 3_3Mbps. */

#define JBIT_MIFARE 0x00 /* Activates Mifare communication mode. */

#define JBIT_CRCEN 0x80 /* Activates transmit or receive CRC. */

/* TxControl Register      (14) */
#define JBIT_INVTX2ON 0x80  /* Inverts the Tx2 output if drivers are switched on. */
#define JBIT_INVTX1ON 0x40  /* Inverts the Tx1 output if drivers are switched on. */
#define JBIT_INVTX2OFF 0x20 /* Inverts the Tx2 output if drivers are switched off. */
#define JBIT_INVTX1OFF 0x10 /* Inverts the Tx1 output if drivers are switched off. */
#define JBIT_TX2CW 0x08     /* Does not modulate the Tx2 output, only constant wave. */

/* Does not activate the driver if an external RF is detected.
Only valid in combination with JBIT_TX2RFEN and JBIT_TX1RFEN. */

#define JBIT_TX2RFEN 0x02 /* Switches the driver for Tx2 pin on. */
#define JBIT_TX1RFEN 0x01 /* Switches the driver for Tx1 pin on. */

/* TxAuto Register       (15) */
#define JBIT_AUTORFOFF 0x80   /* Switches the RF automatically off after transmission is finished. */
#define JBIT_FORCE100ASK 0x40 /* Activates 100%ASK mode independent of driver settings. */

#define JBIT_CAON 0x08        /* Activates the automatic time jitter generation by switching \
                                   on the Rf field as defined in ECMA-340. */
#define JBIT_INITIALRFON 0x04 /* Activate the initial RF on procedure as defined iun ECMA-340. */
#define JBIT_TX2RFAUTOEN 0x02 /* Switches on the driver two automatically according to the \
                                   other settings. */
#define JBIT_TX1RFAUTOEN 0x01 /* Switches on the driver one automatically according to the \
                                   other settings. */

/* Demod Register        (19) */
#define JBIT_FIXIQ 0x20
/* If set to 1 and the lower bit of AddIQ is set to 0, the receiving is fixed to I channel.
   If set to 1 and the lower bit of AddIQ is set to 1, the receiving is fixed to Q channel. */

/* Felica/NFC 2 Register         (1B) */
#define JBIT_WAITFORSELECTED 0x80
/* If this bit is set to one, only passive communication modes are possible.
   In any other case the AutoColl Statemachine does not exit. */
#define JBIT_FASTTIMESLOT 0x40
/* If this bit is set to one, the response time to the polling command is half as normal. */

/* Mifare Register        (1C) */
#define JBIT_MFHALTED 0x04 /* Configures the internal state machine only to answer to \
                                Wakeup commands according to ISO 14443-3. */

/* RFU 0x1D Register          (1D) */
#define JBIT_PARITYDISABLE 0x10 /* Disables the parity generation and sending independent from the mode. */
#define JBIT_LARGEBWPLL 0x08
#define JBIT_MANUALHPCF 0x04

/* FM175XX Bit definitions of Page 2
 *  Below there are useful bit definition of the FM175XX register set.
 */
/* TxBitPhase Register          (25) */
#define JBIT_RCVCLKCHANGE 0x80 /* If 1 the receive clock may change between Rf and oscilator. */

/* RfCFG Register        (26) */
#define JBIT_RFLEVELAMP 0x80 /* Activates the RF Level detector amplifier. */

/* TMode Register        (2A) */
#define JBIT_TAUTO 0x80        /* Sets the Timer start/stop conditions to Auto mode. */
#define JBIT_TAUTORESTART 0x10 /* Restarts the timer automatically after finished \
                                    counting down to 0. */

/* FM175XX Bit definitions of Page 3
 *  Below there are useful bit definition of the FM175XX register set.
 */
/* AutoTest Register          (36) */
#define JBIT_AMPRCV 0x40

/* // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // /
 * Bitmask Definitions
 * // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // */

/* Command register                 (0x01) */
#define JMASK_COMMAND 0x0F     /* Bitmask for Command bits in Register JREG_COMMAND. */
#define JMASK_COMMAND_INV 0xF0 /* Inverted bitmask of JMASK_COMMAND. */

/* Waterlevel register              (0x0B) */
#define JMASK_WATERLEVEL 0x3F /* Bitmask for Waterlevel bits in register JREG_WATERLEVEL. */

/* Control register                 (0x0C) */
#define JMASK_RXBITS 0x07 /* Bitmask for RxLast bits in register JREG_CONTROL. */

/* Mode register                    (0x11) */
#define JMASK_CRCPRESET 0x03 /* Bitmask for CRCPreset bits in register JREG_MODE. */

/* TxMode register                  (0x12, 0x13) */
#define JMASK_SPEED 0x70   /* Bitmask for Tx/RxSpeed bits in register JREG_TXMODE and JREG_RXMODE. */
#define JMASK_FRAMING 0x03 /* Bitmask for Tx/RxFraming bits in register JREG_TXMODE and JREG_RXMODE. */

/* TxSel register                   (0x16) */
#define JMASK_LOADMODSEL 0xC0 /* Bitmask for LoadModSel bits in register JREG_TXSEL. */
#define JMASK_DRIVERSEL 0x30  /* Bitmask for DriverSel bits in register JREG_TXSEL. */
#define JMASK_SIGOUTSEL 0x0F  /* Bitmask for SigoutSel bits in register JREG_TXSEL. */

/* RxSel register                   (0x17) */
#define JMASK_UARTSEL 0xC0 /* Bitmask for UartSel bits in register JREG_RXSEL. */
#define JMASK_RXWAIT 0x3F  /* Bitmask for RxWait bits in register JREG_RXSEL. */

/* RxThreshold register             (0x18) */
#define JMASK_MINLEVEL 0xF0 /* Bitmask for MinLevel bits in register JREG_RXTHRESHOLD. */
#define JMASK_COLLEVEL 0x07 /* Bitmask for CollLevel bits in register JREG_RXTHRESHOLD. */

/* Demod register                   (0x19) */
#define JMASK_ADDIQ 0xC0   /* Bitmask for ADDIQ bits in register JREG_DEMOD. */
#define JMASK_TAURCV 0x0C  /* Bitmask for TauRcv bits in register JREG_DEMOD. */
#define JMASK_TAUSYNC 0x03 /* Bitmask for TauSync bits in register JREG_DEMOD. */

/* Mifare register                  (0x1C) */

#define JMASK_TXWAIT 0x03 /* Bitmask for TxWait bits in register JREG_MIFARE. */

/* Manual Rcv register    (0x1D) */
#define JMASK_HPCF 0x03 /* Bitmask for HPCF filter adjustments. */

/* TxBitPhase register              (0x25) */
#define JMASK_TXBITPHASE 0x7F /* Bitmask for TxBitPhase bits in register JREG_TXBITPHASE. */

/* RFCfg register                   (0x26) */
#define JMASK_RXGAIN 0x70  /* Bitmask for RxGain bits in register JREG_RFCFG. */
#define JMASK_RFLEVEL 0x0F /* Bitmask for RfLevel bits in register JREG_RFCFG. */

/* GsN register                     (0x27) */
#define JMASK_CWGSN 0xF0  /* Bitmask for CWGsN bits in register JREG_GSN. */
#define JMASK_MODGSN 0x0F /* Bitmask for ModGsN bits in register JREG_GSN. */

/* CWGsP register                   (0x28) */
#define JMASK_CWGSP 0x3F /* Bitmask for CWGsP bits in register JREG_CWGSP. */

/* ModGsP register                  (0x29) */
#define JMASK_MODGSP 0x3F /* Bitmask for ModGsP bits in register JREG_MODGSP. */

/* TMode register                   (0x2A) */
#define JMASK_TGATED 0x60        /* Bitmask for TGated bits in register JREG_TMODE. */
#define JMASK_TPRESCALER_HI 0x0F /* Bitmask for TPrescalerHi bits in register JREG_TMODE. */

#define JREG_EXT_REG_ENTRANCE 0x0F // ext register entrance
#define JBIT_EXT_REG_WR_ADDR 0X40  // wrire address cycle
#define JBIT_EXT_REG_RD_ADDR 0X80  // read address cycle
#define JBIT_EXT_REG_WR_DATA 0XC0  // write data cycle
#define JBIT_EXT_REG_RD_DATA 0X00  // read data cycle

#define JREG_LPCDDELTA_HI 0x26
#define JREG_LPCDDELTA_LO 0x27

#define JREG_LPCDAUX 0x30
#define IBN2U 0x00
#define TEST_BG 0x01
#define TEST_SAMPLE_I 0x02
#define TEST_AMP_OUT_IN 0x03
#define TEST_AMP_OUT_IP 0x04
#define TEST_AMP_OUT_QN 0x05
#define TEST_AMP_OUT_QP 0x06
#define OSC_64K 0x07
#define VBN2 0x08
#define VBN1 0x09
#define TEST_BG_VREF 0x0A
#define AVSS 0x0B
#define TEST_SAMPLE_Q 0x0C
#define DVDD 0x0D
#define TEST_CRYSTAL_VDD 0x0E
#define AVDD 0x0F
#define FLOAT_IN 0x10

#define JREG_LPCDSLEEPTIMER 0x33
#define JREG_LPCDTHRESH_H 0x34
#define JREG_LPCDTHRESH_L 0x35
#define JREG_LPCDREQATIMER 0x37

#define LPCD_REQA_TIME_80us 0x00
#define LPCD_REQA_TIME_100us 0x01
#define LPCD_REQA_TIME_120us 0x02
#define LPCD_REQA_TIME_150us 0x03
#define LPCD_REQA_TIME_200us 0x04
#define LPCD_REQA_TIME_250us 0x05
#define LPCD_REQA_TIME_300us 0x06
#define LPCD_REQA_TIME_400us 0x07
#define LPCD_REQA_TIME_500us 0x08
#define LPCD_REQA_TIME_600us 0x09
#define LPCD_REQA_TIME_800us 0x0A
#define LPCD_REQA_TIME_1ms 0x0B
#define LPCD_REQA_TIME_1ms2 0x0C
#define LPCD_REQA_TIME_1ms6 0x0D
#define LPCD_REQA_TIME_2ms 0x0E
#define LPCD_REQA_TIME_2ms5 0x0F
#define LPCD_REQA_TIME_3ms 0x10
#define LPCD_REQA_TIME_3ms5 0x11
#define LPCD_REQA_TIME_4ms 0x12
#define LPCD_REQA_TIME_5ms 0x13
#define LPCD_REQA_TIME_7ms 0x14

#define JREG_LPCDREQAANA 0x38
#define LPCD_RXGAIN_23DB 0x00
#define LPCD_RXGAIN_33DB 0x10 // default
#define LPCD_RXGAIN_38DB 0x20
#define LPCD_RXGAIN_43DB 0x30

#define LPCD_MINLEVEL_3 0x00
#define LPCD_MINLEVEL_6 0x04
#define LPCD_MINLEVEL_9 0x08
#define LPCD_MINLEVEL_C 0x0C

#define LPCD_MODWIDTH_32 0x00
#define LPCD_MODWIDTH_38 0x02

#define JREG_LPCDDETECTMODE 0x39
#define LPCD_TXSCALE_0 0x00 // 1/8 of the TX power configured by CWGSP/CWGSNON
#define LPCD_TXSCALE_1 0x08 // 1/4 of the TX power configured by CWGSP/CWGSNON
#define LPCD_TXSCALE_2 0x10 // 1/2 of the TX power configured by CWGSP/CWGSNON
#define LPCD_TXSCALE_3 0x18 // 3/4 of the TX power configured by CWGSP/CWGSNON
#define LPCD_TXSCALE_4 0x20 // equal to of the TX power configured by CWGSP/CWGSNON
#define LPCD_TXSCALE_5 0x28 // twice of the TX power configured by CWGSP/CWGSNON
#define LPCD_TXSCALE_6 0x30 // 3 times of the TX power configured by CWGSP/CWGSNON
#define LPCD_TXSCALE_7 0x31 // 4 times of the TX power configured by CWGSP/CWGSNON

#define LPCD_RX_CHANGE_MODE 0x00
#define LPCD_REQA_MODE 0x01
#define LPCD_COMBINE_MODE 0x02

#define JREG_LPCDCTRLMODE 0x3A
#define RF_DET_ENABLE 0x20
#define RF_DET_DISABLE 0x00

#define RF_DET_SEN_00 0x00 // 高灵敏
#define RF_DET_SEN_01 0x08
#define RF_DET_SEN_10 0x10
#define RF_DET_SEN_11 0x18 // 低灵敏

#define LPCD_DISABLE 0x00
#define LPCD_ENABLE 0x02

#define JREG_LPCDIRQ 0x3B

#define JREG_LPCDRFTIMER 0x3C
#define LPCD_IRQINV_ENABLE 0x20
#define LPCD_IRQINV_DISABLE 0x00

#define LPCD_IRQ_PUSHPULL 0x10
#define LPCD_IRQ_OD 0x00

#define LPCD_RFTIME_5us 0x00
#define LPCD_RFTIME_10us 0x01
#define LPCD_RFTIME_15us 0x02
#define LPCD_RFTIME_20us 0x03
#define LPCD_RFTIME_25us 0x04 // default
#define LPCD_RFTIME_30us 0x05
#define LPCD_RFTIME_35us 0x06
#define LPCD_RFTIME_40us 0x07
#define LPCD_RFTIME_50us 0x08
#define LPCD_RFTIME_60us 0x09
#define LPCD_RFTIME_70us 0x0A
#define LPCD_RFTIME_80us 0x0B
#define LPCD_RFTIME_100us 0x0C
#define LPCD_RFTIME_120us 0x0D
#define LPCD_RFTIME_150us 0x0E
#define LPCD_RFTIME_200us 0x0F

#define JREG_LPCDTXCTRL1 0x3D
#define LPCD_TX2_ENABLE 0x20
#define LPCD_TX2_DISABLE 0x00

#define LPCD_TX1_ENABLE 0x10
#define LPCD_TX1_DISABLE 0x00

#define LPCD_TX2ON_INV_ENABLE 0x08
#define LPCD_TX2ON_INV_DISABLE 0x00

#define LPCD_TX1ON_INV_ENABLE 0x04
#define LPCD_TX1ON_INV_DISABLE 0x00

#define LPCD_TX2OFF_INV_ENABLE 0x02
#define LPCD_TX2OFF_INV_DISABLE 0x00

#define LPCD_TX1OFF_INV_ENABLE 0x01
#define LPCD_TX1OFF_INV_DISABLE 0x00

#define JREG_LPCDTXCTRL2 0x3E
#define JREG_LPCDTXCTRL3 0x3F

#endif
