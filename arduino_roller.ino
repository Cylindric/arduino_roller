#include "easing.h"
#include "eyes.h"
#include "motors.h"

#define DEBUG
#define LED_PIN 13


Easing easing;
Motors motors;
Eyes eyes;

#define STATE_PARKED 0
#define STATE_ADVANCING 1
#define STATE_BACKING_OFF 2

int current_state = STATE_PARKED;
unsigned long time_state_changed = 0;
bool easing_running = false;

void setup() {
	Serial.begin(9600);
	Serial.println("Wheelie starting up...");
	pinMode(LED_PIN, OUTPUT);
	
	// Set motors to debug mode
	motors.Live = true;
	
	// delay a bit before starting to run about.
	delay(5000);
	
	// start off by advancing!
	updateState(STATE_ADVANCING);
}

int initialspeed = 0;
unsigned long initialtime;
int speeddelta = 0;

void loop() {

	bool obstacle_detected = eyes.CheckCollision();
	
	switch(current_state)
	{
		case STATE_PARKED:
			// Parked just means do nothing!
			motors.SetSpeed(0);
			break;
			
		case STATE_ADVANCING:
			if(obstacle_detected)
			{
				// An obstacle was detected, so back off.
				updateState(STATE_BACKING_OFF);
			}
			else
			{
				if(!easing.IsRunning())
				{
					easing.Reset();
					easing.SetDuration(5000);
					easing.SetValues(motors.GetSpeed(), 255);
					easing.Start();
				}
				int newspeed = easing.EaseInOut();
				
				//Serial.print("Easing at "); Serial.println(newspeed);
				motors.SetSpeed(newspeed);
				motors.SetTurn(0);
			}
			break;
			
		case STATE_BACKING_OFF:
			if(timeInState() >= 3000)
			{
				if(obstacle_detected)
				{
					// If after backing off we are still detecting an obstacle, we're probably stuck!
					updateState(STATE_PARKED);
				}
				else
				{
					// We've backed off a bit, so start advancing again.
					updateState(STATE_ADVANCING);
				}
			} 
			else 
			{
				motors.SetSpeed(-255); // Backwards!
				motors.SetTurn(-255); // Turn to one side.
			}
			break;
	}

	motors.Update();
	
	#ifdef DEBUG
	delay(200);
	#endif
}

void updateState(int new_state)
{
	time_state_changed = millis();
	current_state = new_state;
	easing_running = false; // Reset any easing in progress
	
	Serial.print("Changed state to ");
	switch(current_state) 
	{
		case STATE_PARKED: Serial.println("parked."); break;
		case STATE_ADVANCING: Serial.println("advancing."); break;
		case STATE_BACKING_OFF: Serial.println("backing off."); break;
		default: Serial.print("something else ("); Serial.print(current_state); Serial.println(")."); break;
	}
}

unsigned long timeInState()
{
	return millis() - time_state_changed;
}