/*
 * @brief A program for the EA NXP LPC4088QSB to flash LED1, LED2, LED3 and LED4.
 *        LEDs are controlled by the joystick. LEFT -> LED1, DOWN -> LED2,
 *        UP -> LED3, RIGHT -> LED4, CENTER -> All LEDs
 *        This program uses a small, simple library of GPIO functions, and the 
 *        TIMER0 and TIMER1 timers.
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

bool buttonPressedAndReleased(gpioPin_t *pin);
void delay(uint32_t ms);
void sysTickHandler(void);

static gpioPin_t pin[9];
static bool flashing[4] = {false, false, false, false};

int main() {
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
	
	while (true) {
		if (buttonPressedAndReleased(&pin[JLEFT])) {
			flashing[LED1] = !flashing[LED1];
		}
		if (buttonPressedAndReleased(&pin[JDOWN])) {
			flashing[LED2] = !flashing[LED2];
		}
		if (buttonPressedAndReleased(&pin[JUP])) {
			flashing[LED3] = !flashing[LED3];
		}
		if (buttonPressedAndReleased(&pin[JRIGHT])) {
			flashing[LED4] = !flashing[LED4];
		}
		if (buttonPressedAndReleased(&pin[JCENTER])) {
			flashing[LED1] = !flashing[LED1];
			flashing[LED2] = !flashing[LED2];
			flashing[LED3] = !flashing[LED3];
			flashing[LED4] = !flashing[LED4];
		}
	}
}

/*
 * @brief buttonPressedAndReleased(pin) tests the value of the pin.
 *        If the value is 0 then the pin is being pressed, so wait until the
 *        value is 1, i.e button has been released
 * @param pin - pointer to a gpioPin_t struct
 * @result - true if button pressed then released, otherwise false
 */
bool buttonPressedAndReleased(gpioPin_t *pin) {
	bool result = false;
	if (gpioPinVal(pin) == 0) {
		while (gpioPinVal(pin) == 0) {
			/* skip 
			 * This is not a good idea.
			 * The user can keep you in this loop
			 * by holding the button down.
			 */
		}
		result = true;
	}
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
 * Flash LED1 .. LED4
 */
void sysTickHandler(void) {
	uint32_t i;
	static uint32_t count = 0;
	
	count += 1;
	if (count == 1000) {
		count = 0;
    for (i = LED1; i <= LED4; i++) {
	    if (flashing[i]) {
	      gpioPinToggle(&pin[i]);
	    }
	  }
	}
}

