#ifndef _UART_H_
#define _UART_H_

#define BAUD 9600
#define STOPBITS 1
#define CYCLES_PER_BIT (F_CPU/BAUD)

#if (CYCLES_PER_BIT > 255)
# define DIVISOR            8
# define PRESCALE           2
#else
# define DIVISOR            1
# define PRESCALE           1
#endif

#define FULL_BIT_TICKS      (CYCLES_PER_BIT / DIVISOR)

enum USISERIAL_SEND_STATE
  {
    AVAILABLE,
    FIRST,
    SECOND
  };

/* variables */
static volatile enum USISERIAL_SEND_STATE usiserial_send_state;
static volatile uint8_t usiserial_tx_data;

/* function headers */
void uart_init(void);
void usiserial_send_byte(uint8_t data);

#endif //_UART_H_
