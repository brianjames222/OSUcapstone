#include <stdio.h>
#include <stdint.h>

// Define the APU structure
typedef struct {
    uint8_t pulse1_enable;      // Pulse 1 channel
    uint8_t pulse2_enable;      // Pulse 2 channel
    uint8_t triangle_enable;    // Triangle channel
    uint8_t noise_enable;       // Noise channel
    uint8_t dmc_enable;         // DMC channel
    int frame_counter;          // Tracks audio timing
} APU;

// Initialize the APU
void apu_init(APU* apu) {
    apu->pulse1_enable = 0;
    apu->pulse2_enable = 0;
    apu->triangle_enable = 0;
    apu->noise_enable = 0;
    apu->dmc_enable = 0;
    apu->frame_counter = 0;
    printf("APU initialized.\n");
}