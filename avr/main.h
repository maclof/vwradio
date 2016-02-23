#ifndef MAIN_H
#define MAIN_H
#define F_CPU 20000000UL
#define BAUD 115200

#include <stdint.h>

/*************************************************************************
 * Run Mode
 *************************************************************************/

#define RUN_MODE_NORMAL 0
#define RUN_MODE_TEST 1

volatile uint8_t run_mode;


// key data bytes that will be transmitted if the radio sends
// a read key data command
volatile uint8_t upd_tx_key_data[4];

#endif
