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

void display_time(uint32_t hours, uint32_t minutes) {
	LCD_Clear();
	LCD_GotoXY(0, 0);
	LCD_PrintString("Time: ");
	if (hours < 10) LCD_PrintString("0");
	LCD_PrintInteger(hours);
	LCD_PrintString(":");
	if (minutes < 10) LCD_PrintString("0");
	LCD_PrintInteger(minutes);
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

	uint8_t alarmState = 0;
	uint16_t alarmTime = 0; // Time in minutes

	uint16_t minuteOffset = 0;
	uint32_t clockHours = 0;
	uint32_t clockMinutes = 0;

	display_time(clockHours, clockMinutes);

	while (1) {
		switch (getIRCommandRec()) {
			case PLAY_PAUSE:
				alarmState = alarmState != 1;
				break;
			case VOLUME_DOWN:
				minuteOffset--;
				break;
			case VOLUME_UP:
				minuteOffset++;
				break;
			case PREV:
				alarmTime--;
				break;
			case NEXT:
				alarmTime++;
				break;
			case ZERO_TEN:
				//TODO: beesechurger
				break;
			default: break;
		}

		// Calculate time
		uint64_t millis = e_100microseconds/10;
		uint64_t seconds = millis/1000;
		uint64_t minutes = seconds/60 + minuteOffset;
		uint64_t hours = minutes/60;

		// Skip display if time doesn't change
		minutes %= 60;
		if (minutes == clockMinutes) continue;

		clockMinutes = minutes;
		clockHours = hours % 24;
		display_time(clockHours, clockMinutes);
	};

	return 0;
}
