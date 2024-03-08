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
}

void set_frequency(uint16_t f) {
	if (f == 0) DDRD &= ~_BV(DDD6);
	else DDRD |= DDRD |= _BV(DDD6);
	OCR0A = (uint16_t)(31100)/f;
}

void display_time(uint32_t hours, uint32_t minutes) {
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

	uint8_t alarmState = 0;
	uint8_t alarmStatePrev = 0;
	uint8_t alarmHours = 0;
	uint8_t alarmMinutes = 0;
	uint8_t alarmChanged = 1;

	uint16_t minuteOffset = 0;
	uint32_t clockHours = 1;
	uint32_t clockMinutes = 1;

	display_time(clockHours, clockMinutes);

	while (1) {
		switch (getIRCommandRec()) {
			case PLAY_PAUSE:
				alarmState = alarmState != 1;
				break;
			case PREV:
				if (minuteOffset == 0) minuteOffset = 24*60;
				minuteOffset--;
				break;
			case NEXT:
				minuteOffset++;
				break;
			case VOLUME_DOWN:
				if (alarmMinutes == 0) alarmHours = (alarmHours + 23)%24;
				alarmMinutes = (alarmMinutes + 59)%60;
				alarmChanged = 1;
				break;
			case VOLUME_UP:
				if (alarmMinutes == 59) alarmHours = (alarmHours + 1)%24;
				alarmMinutes = (alarmMinutes + 1)%60;
				alarmChanged = 1;
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
		uint64_t hours = (minutes/60)%24;
		minutes %= 60;

		// Flip state 2 and 3 every 500ms
		if (alarmState == 2 && e_100microseconds%10000 > 5000) {
			alarmState = 3;
		} else if (alarmState == 3 && e_100microseconds%10000 <= 5000) {
			alarmState = 2;
		}

		// Display time if changed
		if ((minutes != clockMinutes) || (hours != clockHours)) {
			// Sound alarm if enabled and time matches
			if (alarmState == 1 && minutes == alarmMinutes && hours == alarmHours) alarmState = 2; 
			
			clockMinutes = minutes;
			clockHours = hours % 24;
			LCD_GotoXY(0, 0);
			LCD_PrintString("Clock Time ");
			display_time(clockHours, clockMinutes);
		}

		// Display time if changed
		if (alarmState != alarmStatePrev || alarmChanged) {
			LCD_GotoXY(0, 1);
			LCD_PrintString("Alarm ");
			LCD_PrintString(alarmState > 0 ? "On   " : "Off  ");

			if (alarmState != 3) display_time(alarmHours, alarmMinutes);
			else LCD_PrintString("     ");

			set_frequency(260*(alarmState == 2));
			alarmChanged = 0;
		}

		alarmStatePrev = alarmState;

	};

	return 0;
}
