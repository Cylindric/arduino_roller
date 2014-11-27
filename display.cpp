#include "Arduino.h"
#include "display.h"

#define LED_PIN A1

Display::Display()
{
	led_state = false;
	this->Update();
}


void Display::Blink(int n, int delay)
{
	last_change_time = millis();
	blinks_remaining = n + n;
	blink_delay = delay;
}


void Display::Update()
{
	if(blinks_remaining > 0)
	{
		if(last_change_time <= (millis()-blink_delay))
		{
			// If the light is on, turn it off.
			// If the light is off, turn it on.
			led_state = !led_state;
			last_change_time = millis();
			blinks_remaining--;
		}
	}
	digitalWrite(LED_PIN, led_state);
}
