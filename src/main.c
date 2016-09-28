#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define LED _BV(0);
#define led_on()  { DDRB |= LED }
#define led_off() { DDRB &= ~LED }

void chip_init(void);

int main(void)
{
  
  chip_init();

  while(1)
    {
      /* periodic flash */
      led_on();
      _delay_ms(100);
      led_off();
      _delay_ms(1900);
    }

  return 0;

}

void chip_init(void)
{

  /* SETUP LED PORT */
  PORTB &= ~LED;

}
