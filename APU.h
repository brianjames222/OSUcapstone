#ifndef APU_H
#define APU_H

#include <array>
#include <cstdint>
#include <vector>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

// Sound channels
struct PulseChannel {
    bool enabled = false;
    uint8_t duty = 0;       // 0-3 decides shape of waveform
    uint16_t timer = 0;     // frequency timer
    uint8_t length_counter = 0;
    uint8_t envelope = 0;
    uint8_t volume = 0;
    bool constant_volume = false;

    uint16_t timer_counter = 0;     // internal timer counter
    uint8_t envelope_counter = 0;
    uint8_t envelope_divider = 0;
    bool envelope_loop = false;
    bool envelope_start = false;

    uint8_t duty_position = 0;

    void update_timer();            // processes timer decrement
    void update_envelope();         // handles envelope volume
    void update_length_counter();   // handles length countdown
    uint8_t generate_sample();      // generates square wave output
};

// TODO: Implement other sound channels here --> Triangle, Noise, DMC

class APU {
public:
    APU();                                                 // Constructor
    ~APU();                                                // Destructor
    void reset();                                          // Resets all registers
    void write_register(uint16_t address, uint8_t value);  // Write to APU registers
    uint8_t read_register(uint16_t address);               // Read from APU registers
    void clock();                                          // Syncs APU to frames
    uint8_t get_pulse_output();                            // retrieves pulse 1 output
    void mixer(std::vector<int16_t>& output_buffer);

    static void audio_callback(void* userdata, Uint8* stream, int len); // SDL2 audio callback

private:
    std::array<uint8_t, 0x18> registers{};                 // APU memory-mapped registers
    int frame_counter = 0;                                 // Used for sequencing APU operations
    int frame_counter_mode = 4;                            // Default --> 4-step sequence

    // Sound channels
    PulseChannel pulse1;
    PulseChannel pulse2;
    // TODO: More channels here --> Triangle, Noise, DMC

    // Audio Buffer
    std::vector<int16_t> audio_buffer;

    // SDL2 Audio System
    SDL_AudioDeviceID audio_device;
    SDL_AudioSpec audio_spec;

    // Frame sequencer functions
    void step_envelope();
    void step_sweep();
    void step_length_counter();
};

#endif

