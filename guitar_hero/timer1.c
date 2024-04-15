/*
 * timer1.c
 *
 * Author: Peter Sutton
 *
 * timer 1 skeleton
 */

#include "timer1.h"
#include <avr/io.h>
#include <avr/interrupt.h>

/* Set up timer 1
 */
void init_timer1(void)
{
	TCNT1 = 0;
	
	DDRD |= (1<<4);
	
	TCCR1A = (1 << COM1B1) | (0 << COM1B0) | (1 << WGM11) | (1 << WGM10);
	TCCR1B = (1 << WGM13) | (1 << WGM12) | (0 << CS12) | (1 << CS11) | (0 << CS10);
}
