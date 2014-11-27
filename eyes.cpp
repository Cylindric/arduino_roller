#include "Arduino.h"
#include "Eyes.h"

Eyes::Eyes()
{
	pinMode(EYE_PIN, INPUT);
}

bool Eyes::CheckCollision()
{
	int vA =  analogRead(EYE_PIN);
	bool collision = true;
	
	if(vA < EYE_TRIGGER_LEVEL)
	{
		collision = true;
		Serial.println("Eyes: Obstacle detected!");
	} 
	else
	{
		collision = false;
	}
	
	return collision;
}