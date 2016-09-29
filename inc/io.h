#ifndef _IO_H_
#define _IO_H_

/* LED definitions */
#define LED _BV(0);
#define led_on()  { DDRB |= LED }
#define led_off() { DDRB &= ~LED }

/* switch definitions */
#define SWITCH _BV(2);
#define switch_status() { return PINB & SWITCH; }

void led_init(void);
void switch_init(void);

#endif // _IO_H_
