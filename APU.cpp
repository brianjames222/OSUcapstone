#include "APU.h"
#include <cstdio>
#include <cstring>

APU::APU() {
	reset();

    // Initialize SDL2
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return;
    }

    // Init
    SDL_AudioSpec desired_spec{};
    desired_spec.freq = 44100;				// standard sample rate (44100hz)
    desired_spec.format = AUDIO_S16SYS;		// 16-bit audio
    desired_spec.channels = 1;				// mono audio
    desired_spec.samples = 512;				// buffer size
    desired_spec.callback = audio_callback;	// set audio callback
    desired_spec.userdata = this;			// pass APU instance to callback

    audio_device = SDL_OpenAudioDevice(nullptr, 0, &desired_spec, &audio_spec, 0);
    if (audio_device == 0) {
    	printf("Failed to open audio device: %s\n", SDL_GetError());
    } else{
    	SDL_PauseAudioDevice(audio_device, 0);
    }
}

APU::~APU() {
	SDL_CloseAudioDevice(audio_device);
    SDL_Quit();
}

void APU::reset() {
	registers.fill(0);  // Clear all registers
	frame_counter = 0;
    pulse1 = {}; 		// resets pulse channel 1
    audio_buffer.clear();
	printf("APU reset.\n");
}

// =======================================
// ============ READ & WRITE =============
// =======================================
void APU::write_register(uint16_t address, uint8_t value) {
	if (address >= 0x4000 && address <= 0x4017) {
		registers[address - 0x4000] = value; // Store value in register

        // Handle Pulse Channel 1 Writes Only (TODO: implement Pulse 2 register write values after valid testing here)
		if (address == 0x4000) {
			pulse1.duty = (value >> 6) & 0x03;  // Bits 6-7 set duty cycle
			pulse1.constant_volume = value & 0x10;
			pulse1.volume = value & 0x0F;
            pulse1.envelope_loop = value & 0x20;
            pulse1.envelope_start = true; 		// resets envelope on new write
		} else if (address == 0x4002) { // Timer low byte
			pulse1.timer = (pulse1.timer & 0xFF00) | value;
		} else if (address == 0x4003) { // Timer high byte & length counter
			pulse1.timer = (pulse1.timer & 0x00FF) | ((value & 0x07) << 8);
			pulse1.length_counter = (value >> 3) & 0x1F;  // Length counter
		} else if (address == 0x4015) {	// channel enable
			pulse1.enabled = value & 0x01;
		}

        printf("APU Write: $%04X <= %02X\n", address, value);
	}
}

uint8_t APU::read_register(uint16_t address) {
	if (address == 0x4015) {
    	uint8_t status = 0;
        if (pulse1.enabled) status |= 0x01;
        return status;
	}

	printf("Invalid APU register read: %04X\n", address);
	return 0;
}

// =======================================
// ============ MAIN CLOCK ===============
// =======================================
void APU::clock() {
	frame_counter++;

	// 4-Step Frame Sequence
	if (frame_counter_mode == 4) {
    	if (frame_counter % 7457 == 0) step_envelope();
        if (frame_counter % 14914 == 0) {
        	step_envelope();
        	step_length_counter();
        }
        if (frame_counter % 22372 == 0) step_envelope();
        if (frame_counter % 29830 == 0) {
          	step_envelope();
          	step_length_counter();
        }
	}
    pulse1.update_timer(); // updates pulse 1 timer

    int16_t sample = get_pulse_output() * 500; // scale volume
    audio_buffer.push_back(sample);
}

// =======================================
// ======== APU HELPER FUNCTIONS =========
// =======================================
void APU::step_envelope() {
	if (pulse1.enabled) pulse1.update_envelope();
}

void APU::step_length_counter() {
	if (pulse1.length_counter > 0) pulse1.length_counter--;
}

// Transfers APU samples to emulator
void APU::mixer(std::vector<int16_t>& output_buffer) {
	output_buffer.insert(output_buffer.end(), audio_buffer.begin(), audio_buffer.end());
    audio_buffer.clear();
}

// APU Retrieves Pulse 1 Sample Output
uint8_t APU::get_pulse_output() {
	return pulse1.generate_sample();
}

void APU::audio_callback(void* userdata, Uint8* stream, int len) {
    APU* apu = static_cast<APU*>(userdata);
    std::vector<int16_t> buffer;

    apu->mixer(buffer);  // Get audio samples

    memset(stream, 0, len);
    int sample_count = buffer.size();
    int bytes_to_copy = std::min(len, sample_count * (int)sizeof(int16_t));

    memcpy(stream, buffer.data(), bytes_to_copy);
}

// =======================================
// ====== PULSE 1 HELPER FUNCTIONS =======
// =======================================
// Generate Pulse 1 Sample Output
uint8_t PulseChannel::generate_sample() {
	// Duty cycle patterns (8-step sequence)
	static const uint8_t duty_waveforms[4][8] = {
		{0, 0, 0, 0, 0, 0, 1, 0}, // 12.5%
		{0, 0, 0, 0, 1, 0, 1, 0}, // 25%
		{0, 0, 0, 1, 1, 0, 1, 0}, // 50%
		{1, 0, 1, 1, 1, 0, 1, 0}, // 75%
	};

	if (!enabled || length_counter == 0 || timer < 8) return 0; // No sound if muted

	uint8_t duty_value = duty_waveforms[duty][duty_position]; // Get waveform sample
	uint8_t volume_level = constant_volume ? volume : envelope_counter;

	return duty_value * volume_level;
}

// Pulse 1 Timer Update (Generates wave output)
void PulseChannel::update_timer() {
	if (!enabled || length_counter == 0) return; // No sound if disabled

	if (timer_counter == 0) {
		timer_counter = timer;
		duty_position = (duty_position + 1) % 8; // Advance waveform
	} else timer_counter--;
}

// Pulse 1 Envelope Update
void PulseChannel::update_envelope() {
	if (envelope_start) {
		envelope_start = false;
		envelope_counter = 15;
		envelope_divider = volume;
	} else if (envelope_divider > 0) {
		envelope_divider--;
	} else {
		if (envelope_counter > 0) {
			envelope_counter--;
		} else if (envelope_loop) {
			envelope_counter = 15;
		}
		envelope_divider = volume;
	}
}

// Pulse 1 Length Counter Update
void PulseChannel::update_length_counter() {
	if (length_counter > 0) length_counter--;
}