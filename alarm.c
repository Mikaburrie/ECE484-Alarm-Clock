#define __DELAY_BACKWARD_COMPATIBLE__
#define _16MHZ 16000000

#include <avr/io.h>
#include <util/delay.h>

#include "IRremote/commonTimer.h"

#include "IRremote/remotes.h"
#include "lib/HD44780.h"

/*
 * Sets up PWM timer 0
 */
void setup_speaker()
{
	// set compare to 255
	OCR0A = 255;

	// reset counter and toggle output on match
	TCCR0A |= _BV(COM0A0);
	// set CTC mode
	TCCR0A |= _BV(WGM01);
	// set prescaler to 8
	TCCR0B |= _BV(CS02);

	// Output on pin 6
	DDRD |= _BV(DDD6);
}

void set_frequency(uint16_t f) {
	OCR0A = (uint16_t)(31100) /f;
}

int main(void)
{
	//Setup
	setup_speaker();
	
	init100usTimer(_16MHZ);
	initIR(4, CARMP3_ADDRESS, CARMP3_REMOTE);

	LCD_Setup();
	LCD_Clear();
	LCD_GotoXY(0, 0);
	LCD_PrintString("Beesechurger");
	set_frequency(260);

	while (1) {
		switch (getIRCommandRec()) {
			case CH:
				LCD_GotoXY(0, 1);
				LCD_PrintString("Gaaga");
				set_frequency(440);
				_delay_ms(100);
				break;
			default: break;
		}
	};

	return 0;
}
