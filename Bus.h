#ifndef BUS_H
#define BUS_H

#include <array>
#include <cstdint>
#include <chrono>
#include "PPU.h"
#include "ROM.h"
#include "APU.h"

class CPU;
class APU;

class Bus {
public:
    Bus();  // Constructor
    ~Bus(); // Destructor

    // Devices
    CPU* cpu;
    APU* apu;
    PPU  ppu;
    std::array<uint8_t, 2 * 1024> cpuRam{};
    NESROM* rom;
    
    // 8 bits for reading / writing $4016/$4017, for controller input
    enum class Button : uint8_t {
		A = 0x01,
		B = 0x02,
		SELECT = 0x04,
		START = 0x08,
		UP = 0x10,
		DOWN = 0x20,
		LEFT = 0x40,
		RIGHT = 0x80
	};
    										
    // Bus read and write functions
    void write(uint16_t address, uint8_t data);
    uint8_t read(uint16_t address);
    
    // for input handling
    void updateControllerInput(uint8_t buttonState);

    // Reset function
    void reset();
    // Clock function
    void clock();
    // Connect Game Rom to Bus
    void connectROM(NESROM& ROM);

    uint32_t clockCounter = 0;
    uint32_t cpuClockCounter = 0;
    
    // whether or not the controller register is being written to
    bool controller1Polling = false;
    // the current state of the buttons
    uint8_t controller1State = 0x00;

private:
    // Device status

    bool DMATransfer = false;
    // DMA transfers need to start on an even clock cycle
    bool DMACanStart = false;
    uint8_t DMAPage = 0x00;
    uint8_t DMAAddress = 0x00;
    uint8_t DMAData = 0x00;
    
    
    // latching only needs to be seen by the bus
    uint8_t controller1Latch = 0x00;				// effectively a variable to act as $4016

    // state of controller 2 buttons (if needed)
    /*uint8_t controller2State = 0x00;
    uint8_t controller2Latch = 0x00;
    bool controller2Polling = false;*/

};

#endif // BUS_H
