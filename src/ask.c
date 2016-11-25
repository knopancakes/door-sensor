#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "ask.h"

unsigned long nReceivedValue = 0;
unsigned int nReceivedBitlength = 0;
unsigned int nReceivedDelay = 0;
unsigned int nReceivedProtocol = 0;
unsigned int timings[MAX_TIMINGS];
int nReceiveTolerance = 60;



static inline void delay_us(uint16_t count) {
  while(count--) {
    _delay_us(1);

  }
}

void ask_init(void)
{
  nTransmitterPin = -1;
  setPulseLength(350);
  setRepeatTransmit(10);
  setProtocol(1);
}


/**
 * Sets the protocol to send.
 */

void setProtocol(int Protocol) {
  nProtocol = Protocol;
  if (nProtocol == 1){
    setPulseLength(350);
  }
  else if (nProtocol == 2) {
    setPulseLength(650);
  }
}


/**
 * Sets pulse length in microseconds
 */
void setPulseLength(int PulseLength) {
  nPulseLength = PulseLength;
}

/**
 * Sets Repeat Transmits
 */
void setRepeatTransmit(int RepeatTransmit) {
  nRepeatTransmit = RepeatTransmit;
}


/**
 * Enable transmissions
 *
 * @param nTransmitterPin    Pin to which the sender is connected
 */
void enableTransmit(int TransmitterPin) {
  nTransmitterPin = TransmitterPin;
  //pinMode(this->nTransmitterPin, OUTPUT);
  DDRB |= _BV(nTransmitterPin);
}

/**
 * Disable transmissions
 */
void disableTransmit() {
  nTransmitterPin = -1;
  DDRB &= ~_BV(nTransmitterPin);
}

/**
 * Switch a remote switch on (Type B with two rotary/sliding switches)
 *
 * @param nAddressCode  Number of the switch group (1..4)
 * @param nChannelCode  Number of the switch itself (1..4)
 */
void switchOn(int AddressCode, int ChannelCode) {
  enableTransmit(XMIT_PIN);
  sendTriState( getCodeWordB(AddressCode, ChannelCode, true) );
  disableTransmit();
}

/**
 * Switch a remote switch off (Type B with two rotary/sliding switches)
 *
 * @param nAddressCode  Number of the switch group (1..4)
 * @param nChannelCode  Number of the switch itself (1..4)
 */
void switchOff(int AddressCode, int ChannelCode) {
  enableTransmit(XMIT_PIN);
  sendTriState( getCodeWordB(AddressCode, ChannelCode, false) );
  disableTransmit();
}

/**
 * Returns a char[13], representing the Code Word to be send.
 * A Code Word consists of 9 address bits, 3 data bits and one sync bit but in our case only the first 8 address bits and the last 2 data bits were used.
 * A Code Bit can have 4 different states: "F" (floating), "0" (low), "1" (high), "S" (synchronous bit)
 *
 * +-------------------------------+--------------------------------+-----------------------------------------+-----------------------------------------+----------------------+------------+
 * | 4 bits address (switch group) | 4 bits address (switch number) | 1 bit address (not used, so never mind) | 1 bit address (not used, so never mind) | 2 data bits (on|off) | 1 sync bit |
 * | 1=0FFF 2=F0FF 3=FF0F 4=FFF0   | 1=0FFF 2=F0FF 3=FF0F 4=FFF0    | F                                       | F                                       | on=FF off=F0         | S          |
 * +-------------------------------+--------------------------------+-----------------------------------------+-----------------------------------------+----------------------+------------+
 *
 * @param nAddressCode  Number of the switch group (1..4)
 * @param nChannelCode  Number of the switch itself (1..4)
 * @param bStatus       Wether to switch on (true) or off (false)
 *
 * @return char[13]
 */
char* getCodeWordB(int nAddressCode, int nChannelCode, boolean bStatus) {
  int nReturnPos = 0;
  int i;
  static char sReturn[13];
   
  char* code[5] = { "FFFF", "0FFF", "F0FF", "FF0F", "FFF0" };
  if (nAddressCode < 1 || nAddressCode > 4 || nChannelCode < 1 || nChannelCode > 4) {
    return '\0';
  }

  for (i = 0; i<4; i++) {
    sReturn[nReturnPos++] = code[nAddressCode][i];
  }

  for (i = 0; i<4; i++) {
    sReturn[nReturnPos++] = code[nChannelCode][i];
  }
   
  sReturn[nReturnPos++] = 'F';
  sReturn[nReturnPos++] = 'F';
  sReturn[nReturnPos++] = 'F';
   
  if (bStatus) {
    sReturn[nReturnPos++] = 'F';
  } else {
    sReturn[nReturnPos++] = '0';
  }
   
  sReturn[nReturnPos] = '\0';
   
  return sReturn;
}

/**
 * Sends a Code Word
 * @param sCodeWord   -> see getCodeWord
 */
void sendTriState(char* sCodeWord) {
  int nRepeat;
  for (nRepeat=0; nRepeat<nRepeatTransmit; nRepeat++) {
    int i = 0;
    while (sCodeWord[i] != '\0') {
      switch(sCodeWord[i]) {
      case '0':
	sendT0();
        break;
      case 'F':
	sendTF();
        break;
      case '1':
	sendT1();
        break;
      }
      i++;
    }
    sendSync();    
  }
}

void transmit(int nHighPulses, int nLowPulses) {
  if (nTransmitterPin != -1) {
    PORTB |= _BV(nTransmitterPin);
    delay_us(nPulseLength * nHighPulses);
    PORTB &= ~_BV(nTransmitterPin);    
    delay_us(nPulseLength * nHighPulses);
  }
}

/**
 * Sends a "0" Bit
 *                       _    
 * Waveform Protocol 1: | |___
 *                       _  
 * Waveform Protocol 2: | |__
 */
void send0() {
  if (nProtocol == 1){
    transmit(1,3);
  }
  else if (nProtocol == 2) {
    transmit(1,2);
  }
}

/**
 * Sends a "1" Bit
 *                       ___  
 * Waveform Protocol 1: |   |_
 *                       __  
 * Waveform Protocol 2: |  |_
 */
void send1() {
  if (nProtocol == 1){
    transmit(3,1);
  }
  else if (nProtocol == 2) {
    transmit(2,1);
  }
}


/**
 * Sends a Tri-State "0" Bit
 *            _     _
 * Waveform: | |___| |___
 */
void sendT0() {
  transmit(1,3);
  transmit(1,3);
}

/**
 * Sends a Tri-State "1" Bit
 *            ___   ___
 * Waveform: |   |_|   |_
 */
void sendT1() {
  transmit(3,1);
  transmit(3,1);
}

/**
 * Sends a Tri-State "F" Bit
 *            _     ___
 * Waveform: | |___|   |_
 */
void sendTF() {
  transmit(1,3);
  transmit(3,1);
}

/**
 * Sends a "Sync" Bit
 *                       _
 * Waveform Protocol 1: | |_______________________________
 *                       _
 * Waveform Protocol 2: | |__________
 */
void sendSync() {

  if (nProtocol == 1){
    transmit(1,31);
  }
  else if (nProtocol == 2) {
    transmit(1,10);
  }
}
