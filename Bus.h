#ifndef BUS_H
#define BUS_H

#include <array>
#include <cstdint>
#include "PPU.cpp" // Need to make header file for PPU

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
    										
    // Bus read and write functions
    void write(uint16_t address, uint8_t data);
    uint8_t read(uint16_t address);

    // Reset function
    void reset() const;
    // Clock function
    void clock() const;


private:
    uint32_t clockCounter = 0;
};

#endif // BUS_H
