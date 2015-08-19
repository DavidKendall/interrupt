#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

void timer0Init(uint32_t tickHz, void (*handler)());
void timer1Init(uint32_t tickHz, void (*handler)());

#endif


