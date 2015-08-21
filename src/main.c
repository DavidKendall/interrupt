/*
 * @brief A program for the EA NXP LPC4088QSB to flash LED1, LED2, LED3 and LED4.
 *        LEDs are controlled by the joystick. LEFT -> LED1, DOWN -> LED2,
 *        UP -> LED3, RIGHT -> LED4, CENTER -> All LEDs
 *        This program uses a small, simple library of GPIO functions, and the 
 *        SysTick timer to control several soft timers.
 * @author David Kendall
 * @date August 2015
 */

#include <stdbool.h>
#include "gpioPin.h"
#include "timer.h"


typedef enum {
	LED1, LED2, LED3, LED4,
	JLEFT, JDOWN, JUP, JCENTER, JRIGHT
} deviceNames_t;

typedef enum {
	LED1_TASK, LED2_TASK, LED3_TASK, LED4_TASK, BUTTONS_TASK
} taskNames_t;

bool buttonPressedAndReleased(deviceNames_t button);
void delay(uint32_t ms);
void sysTickHandler(void);
void led1Task(void);
void led2Task(void);
void led3Task(void);
void led4Task(void);
void buttonsTask(void);

static gpioPin_t pin[9];
static bool flashing[4] = {false, false, false, false};
static softTimer_t timer[5];

int main() {
	taskNames_t t;
	
	gpioPinInit(&pin[LED1], 1, 18, OUTPUT_PIN);
	gpioPinInit(&pin[LED2], 0, 13, OUTPUT_PIN);
	gpioPinInit(&pin[LED3], 1, 13, OUTPUT_PIN);
	gpioPinInit(&pin[LED4], 2, 19, OUTPUT_PIN);
	gpioPinInit(&pin[JLEFT],   5, 0, INPUT_PIN);
	gpioPinInit(&pin[JDOWN],   5, 1, INPUT_PIN);
	gpioPinInit(&pin[JUP],     5, 2, INPUT_PIN);
	gpioPinInit(&pin[JCENTER], 5, 3, INPUT_PIN);
	gpioPinInit(&pin[JRIGHT],  5, 4, INPUT_PIN);

	sysTickInit(1000, sysTickHandler);
	
	softTimerInit(&timer[LED1_TASK], 1, led1Task);
	softTimerInit(&timer[LED2_TASK], 2, led2Task);
	softTimerInit(&timer[LED3_TASK], 4, led3Task);
	softTimerInit(&timer[LED4_TASK], 8, led4Task);
	softTimerInit(&timer[BUTTONS_TASK], 10, buttonsTask);
	
	while (true) {
		for (t = LED1_TASK; t <= BUTTONS_TASK; t++) {
			if (timer[t].count == 0) {
				timer[t].count = timer[t].reloadValue;
				timer[t].handler();
			}
		}
  }
}

/*
 * @brief buttonPressedAndReleased(button) tests to see if the button has
 *        been pressed then released.
 *        
 * @param button - the name of the button
 * @result - true if button pressed then released, otherwise false
 *
 * If the value of the button's pin is 0 then the button is being pressed,
 * just remember this in savedState.
 * If the value of the button's pin is 1 then the button is released, so
 * if the savedState of the button is 0, then the result is true, otherwise
 * the result is false.
 */
bool buttonPressedAndReleased(deviceNames_t button) {
	bool result = false;
	uint32_t state;
	static uint32_t savedState[9] = {1,1,1,1,1,1,1,1,1};
	
	state = gpioPinVal(&pin[button]);
  if ((savedState[button] == 0) && (state == 1)) {
		result = true;
	}
	savedState[button] = state;
	return result;
}

/*
 * @brief Waits approximately for a specified delay period
 * @param ms - specifies a delay period in milliseconds
 * @return - None
 *
 * delay(ms) causes a delay of ms milliseconds by looping repeatedly,
 * so using CPU cycles, e.g. delay(1000) should give approximately
 * a 1 second delay. Note this depends on the CPU clock speed and 
 * gives only an approximate delay. There are better ways of doing
 * this.
 */
void delay(uint32_t ms) {
	volatile uint32_t i;
	volatile uint32_t j;
	
	for (i=ms; i>0; i--) {
		for (j=11850; j>0; j--) {
			/* skip */
		}
	}	
}


/*
 * @brief Handler for the SysTick interrupt
 * Update the soft timers
 */
void sysTickHandler(void) {
	taskNames_t t;
   
	for (t = LED1_TASK; t <= BUTTONS_TASK; t++) {
		if (timer[t].count > 0) {
		  timer[t].count -= 1;
		}
	}
}

void led1Task(void) {
	if (flashing[LED1]) {
	  gpioPinToggle(&pin[LED1]);
	}
}

void led2Task(void) {
	if (flashing[LED2]) {
	  gpioPinToggle(&pin[LED2]);
	}
}

void led3Task(void) {
	if (flashing[LED3]) {
	  gpioPinToggle(&pin[LED3]);
	}
}

void led4Task(void) {
	if (flashing[LED4]) {
	  gpioPinToggle(&pin[LED4]);
	}
}

void buttonsTask(void) {
  if (buttonPressedAndReleased(JLEFT)) {
		flashing[LED1] = !flashing[LED1];
	}
	if (buttonPressedAndReleased(JDOWN)) {
		flashing[LED2] = !flashing[LED2];
	}
	if (buttonPressedAndReleased(JUP)) {
		flashing[LED3] = !flashing[LED3];
	}
	if (buttonPressedAndReleased(JRIGHT)) {
		flashing[LED4] = !flashing[LED4];
	}
	if (buttonPressedAndReleased(JCENTER)) {
		flashing[LED1] = !flashing[LED1];
		flashing[LED2] = !flashing[LED2];
		flashing[LED3] = !flashing[LED3];
		flashing[LED4] = !flashing[LED4];
	}
}

