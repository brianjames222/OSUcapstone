#include <stdio.h>
#include <stdint.h>

// APU structure
typedef struct {
    uint8_t pulse1_enable;      // Pulse 1 channel
    uint8_t pulse2_enable;      // Pulse 2 channel
    uint8_t triangle_enable;    // Triangle channel
    uint8_t noise_enable;       // Noise channel
    uint8_t dmc_enable;         // DMC channel
    int frame_counter;          // Tracks audio timing
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


// Simulates one APU cycle
void apu_clock(APU* apu) {
    apu->frame_counter++;
    // Simulates frame sync to audio
    if (apu->frame_counter % 60 == 0) {  // 60hz == 60 frames
        apu_play_sound(apu);
    }
}


// Sound placeholder idea
void apu_play_sound(APU* apu) {
    printf("APU Sound: BOING BOOP BEEP BOP Frame count: %d\n", apu->frame_counter);
}


// Main() created just for testing
int main() {
    APU apu;
    apu_init(&apu);

    // Simulates 120 clock cycles
    for (int i = 0; i < 120; i++) {
        apu_clock(&apu);
    }

    return 0;
}