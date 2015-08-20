#include <LPC407x_8x_177x_8x.h>
#include "timer.h"

static void (*timer0UserDefinedHandler)();

void timer0Init(uint32_t tickHz, void (*handler)()) {
	LPC_SC->PCONP |= (1UL << 1); /* ensure power to TIMER0 */
	LPC_TIM0->TCR = 0; /* disable the timer during configuration */
	LPC_TIM0->PR = 0; /* don't scale peripheral clock */
	LPC_TIM0->CTCR = 0; /* select timer mode, not counter mode */
	LPC_TIM0->MR0 = PeripheralClock / tickHz - 1; /* set match register for required rate */
	LPC_TIM0->MCR = 0x03UL; /* interrupt and reset on match */
	timer0UserDefinedHandler = handler; /* install the user-defined handler */
	LPC_TIM0->IR = 0x3F; /* reset all TIMER0 interrupts */
	NVIC_EnableIRQ(TIMER0_IRQn); /* enable the TIMER0 interrupt in the NVIC */
	LPC_TIM0->TCR |= (1UL << 0); /* enable the timer */
}

void TIMER0_IRQHandler(void) {
	timer0UserDefinedHandler(); /* call the user-defined handler */
	LPC_TIM0->IR |= (1UL << 0); /* clear the interrupt on MR0 */
}

