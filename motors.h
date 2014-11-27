// ensure this library description is only included once
#ifndef Motors_h
#define Motors_h

// Which motors are connected
#define MOTOR_LEFT 2
#define MOTOR_RIGHT 3

// Arduino pins for the shift register
// These are hard-wired into the Adafruit motor shield, so shouldn't be changed.
#define MOTORLATCH 12
#define MOTORCLK 4
#define MOTORENABLE 7
#define MOTORDATA 8

// 8-bit bus after the 74HC595 shift register 
// (not Arduino pins)
// These are used to set the direction of the bridge driver.
// These are hard-wired into the Adafruit motor shield, so shouldn't be changed.
#define MOTOR1_A 2
#define MOTOR1_B 3
#define MOTOR2_A 1
#define MOTOR2_B 4
#define MOTOR3_A 5
#define MOTOR3_B 7
#define MOTOR4_A 0
#define MOTOR4_B 6
 
// Arduino pins for the PWM signals.
// These are hard-wired into the Adafruit motor shield, so shouldn't be changed.
#define MOTOR1_PWM 11
#define MOTOR2_PWM 3
#define MOTOR3_PWM 6
#define MOTOR4_PWM 5
#define SERVO1_PWM 10
#define SERVO2_PWM 9

// Codes for the motor function.
#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3
#define RELEASE 4

class Motors
{
	public:
		Motors();
		void SetSpeed(int speed);
		int GetSpeed();
		void SetTurn(int turn);
		void Update();
		bool Live = true;
		
	private:
		int current_speed = 0;
		int current_turn = 0;
		int target_speed = 0;
		int target_turn = 0;
		
		void updateState();
		void updateMotors();
		void motor(int nMotor, int command, int speed);
		void motorOutput (int output, int high_low, int speed);
		void shiftWrite(int output, int high_low);
};

#endif
