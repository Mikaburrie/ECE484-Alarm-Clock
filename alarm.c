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
void setup_speaker(uint8_t period)
{
	// Set compare
	OCR0A = period;

	// Reset counter and toggle output on match
	TCCR0A |= _BV(COM0A0);
	// Set CTC mode
	TCCR0A |= _BV(WGM01);
	// Set prescaler to 8
	TCCR0B |= _BV(CS02);
}

void set_speaker_enabled(uint8_t on) {
	if (on) DDRD |= _BV(DDD6);
	else DDRD &= ~_BV(DDD6);
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
	// Setup PWM for speaker
	setup_speaker(120);
	
	// Setup IR remote
	init100usTimer(_16MHZ);
	initIR(4, CARMP3_ADDRESS, CARMP3_REMOTE);

	// Setup LCD screen
	LCD_Setup();
	LCD_Clear();
	LCD_GotoXY(0, 0);

	// Define variables
	uint8_t alarmState = 0;
	uint8_t alarmStatePrev = 0;
	uint8_t alarmHours = 0;
	uint8_t alarmMinutes = 0;
	uint8_t alarmChanged = 1;

	uint16_t minuteOffset = 0;
	uint32_t clockHours = 1;
	uint32_t clockMinutes = 1;
	
	// Inital render
	display_time(clockHours, clockMinutes);

	while (1) {
		// Handle IR button press
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

		// Redisplay clock time if changed
		if ((minutes != clockMinutes) || (hours != clockHours)) {
			// Sound alarm if enabled and time matches
			if (alarmState == 1 && minutes == alarmMinutes && hours == alarmHours) alarmState = 2; 
			
			clockMinutes = minutes;
			clockHours = hours % 24;
			LCD_GotoXY(0, 0);
			LCD_PrintString("Clock Time ");
			display_time(clockHours, clockMinutes);
		}

		// Redisplay alarm time if changed
		if (alarmState != alarmStatePrev || alarmChanged) {
			LCD_GotoXY(0, 1);
			LCD_PrintString("Alarm ");
			LCD_PrintString(alarmState > 0 ? "On   " : "Off  ");

			if (alarmState != 3) display_time(alarmHours, alarmMinutes);
			else LCD_PrintString("     ");

			set_speaker_enabled(alarmState == 2);
			alarmChanged = 0;
		}

		alarmStatePrev = alarmState;

	};

	return 0;
}
