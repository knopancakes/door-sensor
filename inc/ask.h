#ifndef _ASK_H_
#define _ASK_H_

#include <stdbool.h>

/*
 * Amplitude-Shift Keying for 433MHZ
 */

// Number of maximum High/Low changes per packet.
// We can handle up to (unsigned long) => 32 bit * 2 H/L changes per bit + 2 for sync
#define MAX_TIMINGS 67
#define XMIT_PIN 1


typedef bool boolean;

/* variables */
extern unsigned long nReceivedValue;
extern unsigned int nReceivedBitlength;
extern unsigned int nReceivedDelay;
extern unsigned int nReceivedProtocol;
extern unsigned int timings[MAX_TIMINGS];

/* function headers */
void ask_init(void);

// public

void switchOn(int nGroupNumber, int nSwitchNumber);
void switchOff(int nGroupNumber, int nSwitchNumber);

void sendTriState(char* Code);
void send(char* Code);
  
void enableTransmit(int nTransmitterPin);
void disableTransmit();
void setPulseLength(int nPulseLength);
void setRepeatTransmit(int nRepeatTransmit);
void setReceiveTolerance(int nPercent);
void setProtocol(int nProtocol);

char* getCodeWordB(int nGroupNumber, int nSwitchNumber, boolean bStatus);
void sendT0();
void sendT1();
void sendTF();
void send0();
void send1();
void sendSync();
void transmit(int nHighPulses, int nLowPulses);


int nTransmitterPin;
int nPulseLength;
int nRepeatTransmit;
char nProtocol;




#endif //_ASK_H_
