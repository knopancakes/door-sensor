#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "io.h"

void led_init(void)
{
  /* setup LED port */
  PORTB &= ~LED;
}

void switch_init(void)
{
  /* setup input pin for door sensor switch */
  PORTB &= ~SWITCH;
  DDRB |= SWITCH;
}

/* pin interrupt */
ISR(PCINT0_vect)
{
  // usiserial_send_byte(0x12);
}
