#include "Arduino.h"
#include "motors.h"
#include "easing.h"

Motors::Motors()
{
	this->SetSpeed(0);
	this->SetTurn(0);
	this->Update();
}

void Motors::SetSpeed(int speed)
{
	this->target_speed = speed;
}


int Motors::GetSpeed()
{
	return this->current_speed;
}


void Motors::SetTurn(int turn)
{
	this->target_turn = turn;
}


void Motors::Update()
{
	this->updateState();
	this->updateMotors();
}

void Motors::updateState()
{	
	// Currently there is no special ease-in or ease-out, we just set the speed
	// to what was requested.
	this->current_speed = this->target_speed;
	this->current_turn = this->target_turn;
}

void Motors::updateMotors()
{
	if(this->current_speed == 0)
	{
		motor(MOTOR_LEFT, BRAKE, 0);
		motor(MOTOR_RIGHT, BRAKE, 0);
		return;
	}

	int left_speed = this->current_speed;
	int right_speed = this->current_speed;

	if(this->current_turn < 0) // Turn right
	{
		// left motor needs to speed up, right motor needs to slow down
		left_speed = left_speed + (left_speed*(this->current_turn/255));
		right_speed = right_speed - (right_speed*(this->current_turn/255));
	}
	else // Turn left
	{
		// right motor needs to speed up, left motor needs to slow down
		left_speed = left_speed - (left_speed*(this->current_turn/255));
		right_speed = right_speed + (right_speed*(this->current_turn/255));
	}

	// Clamp speeds
	left_speed = constrain(left_speed, -255, 255);
	right_speed = constrain(right_speed, -255, 255);

	// Apply the calculated values
	if(left_speed == 0)
	{
		motor(MOTOR_LEFT, BRAKE, 0);
	}
	else
	{
		if(left_speed > 0) // Forwards
		{
			motor(MOTOR_LEFT, FORWARD, left_speed);
		}
		else // Backwards
		{
			motor(MOTOR_LEFT, BACKWARD, left_speed);
		}
	}

	if(right_speed == 0)
	{
		motor(MOTOR_RIGHT, BRAKE, 0);
	}
	else
	{
		if (right_speed > 0) // Forwards
		{
			motor(MOTOR_RIGHT, FORWARD, right_speed);
		}
		else // Backwards
		{
			motor(MOTOR_RIGHT, BACKWARD, right_speed);
		}
	}
}


void Motors::motor(int nMotor, int command, int speed)
{
	if(!Live)
	{
		return;
	}
	int motorA;
	int motorB;
	
	if (nMotor >= 1 && nMotor <= 4)
	{  
		switch (nMotor)
		{
			case 1:
				motorA   = MOTOR1_A;
				motorB   = MOTOR1_B;
				break;
			case 2:
				motorA   = MOTOR2_A;
				motorB   = MOTOR2_B;
				break;
			case 3:
				motorA   = MOTOR3_A;
				motorB   = MOTOR3_B;
				break;
			case 4:
				motorA   = MOTOR4_A;
				motorB   = MOTOR4_B;
				break;
			default:
				break;
		}

		switch (command)
		{
			case FORWARD:
				motorOutput (motorA, HIGH, speed);
				motorOutput (motorB, LOW, -1);     // -1: no PWM set
				break;
			case BACKWARD:
				motorOutput (motorA, LOW, speed);
				motorOutput (motorB, HIGH, -1);    // -1: no PWM set
				break;
			case BRAKE:
				// The AdaFruit library didn't implement a brake.
				// The L293D motor driver ic doesn't have a good
				// brake anyway.
				// It uses transistors inside, and not mosfets.
				// Some use a software break, by using a short
				// reverse voltage.
				// This brake will try to brake, by enabling 
				// the output and by pulling both outputs to ground.
				// But it isn't a good break.
				motorOutput (motorA, LOW, 255); // 255: fully on.
				motorOutput (motorB, LOW, -1);  // -1: no PWM set
				break;
			case RELEASE:
				motorOutput (motorA, LOW, 0);  // 0: output floating.
				motorOutput (motorB, LOW, -1); // -1: no PWM set
				break;
			default:
				break;
		}
	}
}


// ---------------------------------
// motorOutput
//
// The function motor_ouput uses the motor driver to
// drive normal outputs like lights, relays, solenoids, 
// DC motors (but not in reverse).
//
// It is also used as an internal helper function 
// for the motor() function.
//
// The high_low variable should be set 'HIGH' 
// to drive lights, etc.
// It can be set 'LOW', to switch it off, 
// but also a 'speed' of 0 will switch it off.
//
// The 'speed' sets the PWM for 0...255, and is for 
// both pins of the motor output.
//   For example, if motor 3 side 'A' is used to for a
//   dimmed light at 50% (speed is 128), also the 
//   motor 3 side 'B' output will be dimmed for 50%.
// Set to 0 for completely off (high impedance).
// Set to 255 for fully on.
// Special settings for the PWM speed:
//    Set to -1 for not setting the PWM at all.
//
void Motors::motorOutput (int output, int high_low, int speed)
{
	int motorPWM;

	switch (output)
	{
		case MOTOR1_A:
		case MOTOR1_B:
			motorPWM = MOTOR1_PWM;
			break;
		case MOTOR2_A:
		case MOTOR2_B:
			motorPWM = MOTOR2_PWM;
			break;
		case MOTOR3_A:
		case MOTOR3_B:
			motorPWM = MOTOR3_PWM;
			break;
		case MOTOR4_A:
		case MOTOR4_B:
			motorPWM = MOTOR4_PWM;
			break;
		default:
			// Use speed as error flag, -3333 = invalid output.
			speed = -3333;
			break;
	}

	if (speed != -3333)
	{
		// Set the direction with the shift register 
		// on the MotorShield, even if the speed = -1.
		// In that case the direction will be set, but
		// not the PWM.
		shiftWrite(output, high_low);

		// set PWM only if it is valid
		if (speed >= 0 && speed <= 255)    
		{
			analogWrite(motorPWM, speed);
		}
	}
}


// ---------------------------------
// shiftWrite
//
// The parameters are just like digitalWrite().
//
// The output is the pin 0...7 (the pin behind 
// the shift register).
// The second parameter is HIGH or LOW.
//
// There is no initialization function.
// Initialization is automatically done at the first
// time it is used.
//
void Motors::shiftWrite(int output, int high_low)
{
	static int latch_copy;
	static int shift_register_initialized = false;

	// Do the initialization on the fly, 
	// at the first time it is used.
	if (!shift_register_initialized)
	{
		// Set pins for shift register to output
		pinMode(MOTORLATCH, OUTPUT);
		pinMode(MOTORENABLE, OUTPUT);
		pinMode(MOTORDATA, OUTPUT);
		pinMode(MOTORCLK, OUTPUT);

		// Set pins for shift register to default value (low);
		digitalWrite(MOTORDATA, LOW);
		digitalWrite(MOTORLATCH, LOW);
		digitalWrite(MOTORCLK, LOW);
		// Enable the shift register, set Enable pin Low.
		digitalWrite(MOTORENABLE, LOW);

		// start with all outputs (of the shift register) low
		latch_copy = 0;

		shift_register_initialized = true;
	}

	// The defines HIGH and LOW are 1 and 0.
	// So this is valid.
	bitWrite(latch_copy, output, high_low);

	// Use the default Arduino 'shiftOut()' function to
	// shift the bits with the MOTORCLK as clock pulse.
	// The 74HC595 shift register wants the MSB first.
	// After that, generate a latch pulse with MOTORLATCH.
	shiftOut(MOTORDATA, MOTORCLK, MSBFIRST, latch_copy);
	delayMicroseconds(5);    // For safety, not really needed.
	digitalWrite(MOTORLATCH, HIGH);
	delayMicroseconds(5);    // For safety, not really needed.
	digitalWrite(MOTORLATCH, LOW);
}
