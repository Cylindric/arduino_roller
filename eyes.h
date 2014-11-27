// ensure this library description is only included once
#ifndef Eyes_h
#define Eyes_h

// Which pin is the sensor connected to
#define EYE_PIN 0

// Define the trigger level for the analogRead on the sensor, 
// seems to be < 13 when triggered, and > 780 when not triggered.
#define EYE_TRIGGER_LEVEL 400

class Eyes
{
	public:
		Eyes();
		bool CheckCollision();
		
	private:
};

#endif
