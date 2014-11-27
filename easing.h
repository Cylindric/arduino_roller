// ensure this library description is only included once
#ifndef Easing_h
#define Easing_h

class Easing
{
	public:
		Easing();
		void Reset();
		void SetDuration(unsigned long duration);
		void SetValues(int start, int end);
		bool IsRunning();
		int EaseInOut();
		void Start();
		
	private:
		unsigned long t_start;
		unsigned long t_delta;
		unsigned long t_end;
		int v_start;
		int v_end;
		int v_delta;
		float v_delta_2;
};

#endif
