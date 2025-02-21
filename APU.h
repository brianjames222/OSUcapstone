#ifndef APU_H
#define APU_H

#include <array>
#include <cstdint>

// Sound channels
struct PulseChannel {
    bool enabled = false;
    uint8_t duty = 0;
    uint16_t timer = 0;
    uint8_t length_counter = 0;
    uint8_t envelope = 0;
    uint8_t volume = 0;
    bool constant_volume = false;

    void update_timer();
    void update_envelope();
    void update_length_counter();
};

// TODO: Implement other sound channels here --> Triangle, Noise, DMC

class APU {
public:
    APU();                                                 // Constructor
    void reset();                                          // Resets all registers
    void write_register(uint16_t address, uint8_t value);  // Write to APU registers
    uint8_t read_register(uint16_t address);               // Read from APU registers
    void clock();                                          // Syncs APU to frames

private:
    std::array<uint8_t, 0x18> registers{};                 // APU memory-mapped registers
    int frame_counter = 0;                                 // Used for sequencing APU operations
    int frame_counter_mode = 4;                            // Default --> 4-step sequence

    // Sound channels
    PulseChannel pulse1;
    PulseChannel pulse2;
    // TODO: More channels here --> Triangle, Noise, DMC

    // Frame sequencer functions
    void step_envelope();
    void step_sweep();
    void step_length_counter();
};

#endif

