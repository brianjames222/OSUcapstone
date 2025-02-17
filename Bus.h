#ifndef BUS_H
#define BUS_H

#include <array>
#include <cstdint>
class CPU;
class Bus {
public:
    Bus();  // Constructor
    ~Bus(); // Destructor

    // Devices
    CPU* cpu;
    std::array<uint8_t, 2 * 1024> cpuRam{};
    std::array<uint8_t, 8> ppuRegister{};

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
