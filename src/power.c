#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "power.h"

void low_power_mode(void)
{
  // sleep_mode();
}

/* watchdog for low power mode */
ISR(WDT_vect)
{

}
