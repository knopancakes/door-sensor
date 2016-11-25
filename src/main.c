#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "ask.h"
#include "io.h"
#include "power.h"

/* wrapper for all initialization calls */
void chip_init(void);

/* main routine */
int main(void)
{
  
  chip_init();

  while(1)
    {
      /* periodic LED flash */
      led_on();
      switchOn(24,10);
      _delay_ms(100);
      led_off();
      switchOff(24,10);
      _delay_ms(1900);
    }

  return 0;

}

/* function definitions */
void chip_init(void)
{

  /* setup LED */
  led_init();

  /* setup door switch */
  switch_init();

  /* initialize amplitude-shift keying driver */
  ask_init();

  /* begin low-power mode */
  //low_power_mode();
}
