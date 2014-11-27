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

#define WARMUP_DELAY 5000

#define BACKOFF_TIME 3000
#define BACKOFF_SPEED -255
#define BACKOFF_EASE_TIME 2000

#define ADVANCE_SPEED 255
#define ADVANCE_EASE_TIME 3000


int current_state = STATE_PARKED;
unsigned long time_state_changed = 0;

void setup() {
	Serial.begin(9600);
	Serial.println("Wheelie starting up...");
	pinMode(LED_PIN, OUTPUT);
	
	// Set motors to debug mode
	motors.Live = true;
	
	// delay a bit before starting to run about.
	delay(WARMUP_DELAY);

	Serial.println("Startup complete. Off we go...");
	
	// start off by advancing!
	updateState(STATE_ADVANCING);
}


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
					// Set up the ease-in and ease-out for the motors.
					easing.SetDuration(ADVANCE_EASE_TIME);
					easing.SetValues(motors.GetSpeed(), ADVANCE_SPEED);
					easing.Start();
				}
				
				// Update the motor speeds.
				motors.SetSpeed(easing.EaseInOut());
				motors.SetTurn(0);
			}
			break;
			
		case STATE_BACKING_OFF:
			if(timeInState() >= BACKOFF_TIME) {
				// We have been backing up for longer than the BACKOFF_TIME, so 
				// should start to go forwards again.
				
				if(obstacle_detected) {
					// If after backing off we are still detecting an obstacle, we're probably 
					// stuck, so just park.
					updateState(STATE_PARKED);
				}
				else {
					// We've backed off a bit, are not still up against 
					// something, so can start advancing again.
					updateState(STATE_ADVANCING);
				}
			} 
			else {
				// We are backing away from an obstacle.
				
				if(!easing.IsRunning()){
					// Set up the ease-in and ease-out for the motors.
					easing.SetDuration(BACKOFF_EASE_TIME);
					easing.SetValues(motors.GetSpeed(), BACKOFF_SPEED);
					easing.Start();
				}

				// Update the motor speeds.
				motors.SetSpeed(easing.EaseInOut());
				motors.SetTurn(-255);
			}
			break;
	}

	// Perform all the World Update Steps
	motors.Update();
	
	#ifdef DEBUG
	delay(200);
	#endif
}

void updateState(int new_state)
{
	time_state_changed = millis();
	current_state = new_state;
	easing.Reset(); // Reset any easing in progress
	
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
