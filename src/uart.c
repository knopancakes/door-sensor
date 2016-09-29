#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "uart.h"

void uart_init(void)
{

  /* initialize state machine */
  usiserial_send_state = AVAILABLE;
  
  /* initialize timer */
  TCCR0A = 2<<WGM00;      // CTC mode
  //TCCR0B = CLOCKSELECT;   // Set prescaler to clk
  GTCCR |= 1 << PSR0;     // Reset prescaler
  TCNT0 = 0;               // Reset counter
  OCR0A = FULL_BIT_TICKS; // Trigger every full bit width

  /* initialize USI module */
  USICR = 
    (0<<USIWM1) | 
    (1<<USIWM0) | // Select three wire mode so USI output on PB1/DO
    (1<<USIOIE) | // Enable USI Counter OVF interrupt
    (0<<USICS1) |
    (1<<USICS0) |
    (0<<USICLK)   // Timer0 Compare Match as USI clock source
    ;

  /* set DO pin as output */
  DDRB |= (1<<DDB1);

  /* load usi data register with start bit & first 7 data bits */
  USIDR = 0x00 | (usiserial_tx_data >> 1);

  /* clear OVF flag & set counter to 8 bits */
  USISR = (1<<USIOIF) | (16-8);
}

static uint8_t reverse_byte(uint8_t x)
{
  x = ((x>>1) & 0x55) | ((x<<1) & 0xaa);
  x = ((x>>2) & 0x33) | ((x<<1) & 0xcc);
  x = ((x>>4) & 0x0f) | ((x<<1) & 0xf0);
  return x;
}

void usiserial_send_byte(uint8_t data)
{
 
  usiserial_send_state = FIRST;
  
  usiserial_tx_data = reverse_byte(data);

}

ISR (USI_OVF_vect)
{
  if(usiserial_send_state == FIRST)
    {
      usiserial_send_state = SECOND;

      /* send last 1 bit of data + stop bits */
      USIDR = (usiserial_tx_data << 7) | 0x7F;
      
      USISR = (1<<USIOIF) | (16-(1+(STOPBITS)));

    }
  else // usiserial_send_state = SECOND
    {
      PORTB |= (1<<PB1); // ensure output is high
      DDRB |= (1<<PB1);  // ensure configured as output
      USICR = 0;         // disable USI

      /* clear flag */
      USISR |= (1<<USIOIF);
       
      usiserial_send_state = AVAILABLE;
      
    }
}
