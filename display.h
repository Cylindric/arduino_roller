// ensure this library description is only included once
#ifndef Display_h
#define Display_h

class Display
{
	public:
		Display();
		void Blink(int n, int delay);
		// Should be called on every world-update cycle to facilitate effects.
		void Update();
		
	private:
		int blinks_remaining;
		int blink_delay;
		bool led_state;
		unsigned long last_change_time;
};

#endif
