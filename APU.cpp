#include <stdio.h>
#include <stdint.h>

class APU {
public:
	// APU structure
	// All write-only, except $4015, which is read/write
	// $4015 is a special status register
	struct {
		uint8_t pulse1_enable;      // Pulse 1 channel:  	$4000-$4003
		uint8_t pulse2_enable;      // Pulse 2 channel:  	$4004-4007
		uint8_t triangle_enable;    // Triangle channel: 	$4008-$400B
		uint8_t noise_enable;       // Noise channel:    	$400C-$400F
		uint8_t dmc_enable;         // DMC channel:		 	$4010-$4013
		int frame_counter;          // Tracks audio timing: $4017
	} APU;


	// Initializes APU sound busses/channels
	void apu_init(APU* apu) {
		apu->pulse1_enable = 0;
		apu->pulse2_enable = 0;
		apu->triangle_enable = 0;
		apu->noise_enable = 0;
		apu->dmc_enable = 0;
		apu->frame_counter = 0;
		printf("APU initialized.\n");
	}


/*
	// Simulates one APU cycle
	void apu_clock(APU* apu) {
		apu->frame_counter++;
		// Simulates frame sync to audio
		if (apu->frame_counter % 60 == 0) {  // 60hz == 60 frames
		    apu_play_sound(apu);
		}
	}
*/
};
