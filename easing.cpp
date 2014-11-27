#include "Arduino.h"
#include "Easing.h"


Easing::Easing()
{
	Reset();
}

void Easing::Reset()
{
	t_start = -1;
	t_end = -1;
	t_delta = 0;
	v_start = 0;
	v_end = 0;
	v_delta = 0;
}

void Easing::SetDuration(unsigned long duration)
{
	t_delta = duration;
	// Setting a t_delta has to cause the times to be recalculated
	t_start = -1;
	t_end = -1;
}

void Easing::SetValues(int start, int end)
{
	v_start = start;
	v_end = end;
	v_delta = (v_end - v_start);
	v_delta_2 = (float)v_delta/2;
}


void Easing::Start()
{
	t_start = millis();
	t_end = t_start + t_delta;	
}


int Easing::EaseInOut()
{
	unsigned long t_now = millis();
	
	if((t_now <= t_start) || (t_start == t_end) || (v_start == v_end) || (v_delta == 0))
	{
		return v_start;
	}
	if(t_now >= t_end)
	{
		return v_end;
	}

	float t_progress = (float)(t_now - t_start) / t_delta;
	
	return (sin(t_progress * M_PI - M_PI_2) * v_delta_2) + (v_delta_2) + v_start;
}


bool Easing::IsRunning()
{
	return (t_start != -1);
}
