#ifndef UART1_H
#define UART1_H

#include "ringbuf.h"

/*************************************************************************
 * UART
 *************************************************************************/

void uart1_init();
void uart1_flush_tx();
void uart1_put(uint8_t c);
void uart1_puts(uint8_t *str);

volatile ringbuffer_t uart1_rx_buffer;
volatile ringbuffer_t uart1_tx_buffer;

#endif