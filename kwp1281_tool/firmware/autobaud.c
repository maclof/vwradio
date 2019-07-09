#include "main.h"
#include "autobaud.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "uart.h"

/*
 *                         Sync Byte (0x55)
 *
 *   Idle   Start  0   1   2   3   4   5   6   7  Stop     Idle
 *
 *   ------------+   +---+   +---+   +---+   +---+   +---------
 *               |   |   |   |   |   |   |   |   |   |
 *               |   |   |   |   |   |   |   |   |   |
 *               +---+   +---+   +---+   +---+   +---+
 *            0    1   0   1   0   1   0   1   0   1
 *
 *               |<->|
 *                 A = Negative edge to positive edge (1 bit)
 *
 *               |<----->|
 *                 B = Negative edge to negative edge (2 bits)
 *
 *         |<----------- Total (10 bits) ----------->|
 *
 *
 *              A          B            Total
 *  1200 baud   833.33 us  1666.66 us   8333.30 us
 *  2400 baud   416.67 us   833.34 us   4166.70 us
 *  4800 baud   208.33 us   416.66 us   2083.30 us
 *  9600 baud   104.17 us   208.34 us   1041.70 us
 * 10400 baud    96.15 us   192.30 us    961.50 us
 */

volatile uint8_t i = 0;
volatile uint16_t edges = 0;
volatile uint16_t starting_cnt = 0;
volatile uint16_t ending_cnt = 0;

void autobaud_start()
{
    i = 0;

    TCCR1A = 0;
    TIFR1 = (1<<ICF1);		// clear input capture flag
    TCCR1B = _BV(CS11);   // 20 MHz clock / 8 prescaler = 8 MHz clock

    // enable capture interrupt
    TIMSK1 |= _BV(ICIE1);

    while(i == 0);
    _delay_ms(5);

    uint16_t diff = ending_cnt - starting_cnt;
    float period = diff * 0.4;
    float frequency = 2000000 / period;
    uint16_t baud = (uint16_t)frequency;

    char msg[60];
    sprintf(msg, "\r\nEDGES: %d\r\n", edges);
    uart_puts(UART_DEBUG, msg);
    sprintf(msg, "START: 0x%04x\r\n", starting_cnt);
    uart_puts(UART_DEBUG, msg);
    sprintf(msg, "END:   0x%04x\r\n", ending_cnt);
    uart_puts(UART_DEBUG, msg);
    sprintf(msg, "DIFF:  0x%04x\r\n", diff);
    uart_puts(UART_DEBUG, msg);
    sprintf(msg, "BAUD:  %d\r\n", baud);
    uart_puts(UART_DEBUG, msg);

    while(1);
}

ISR(TIMER1_CAPT_vect)
{
    if (i == 0) {
      starting_cnt = ICR1;
      i = 1;

      PORTA |= _BV(PA0);     // PA0 = on
      PORTA &= ~_BV(PA0);    // PA0 = off

    } else if (i == 1) {
      ending_cnt = ICR1;
      i = 2;

      PORTA |= _BV(PA0);     // PA0 = on
      PORTA &= ~_BV(PA0);    // PA0 = off

    }


    edges++;
}


// 20 MHz / 8 prescaler = 2.5 MHz = period of 0.4 us
// premium 5 10400 baud
//   count 0x01dd = (477 * 0.4) = 190.8 us
// technisat 9600 baud
//   count = 0x207 = (519 * 0.4) = 207.6 us

//  4800 baud / 2 edges = 2400 = 416.66 us
//  9600 baud / 2 edges = 4800 = 208.33 us
// 10400 baud / 2 edges = 5200 = 192.30 us
