#include "Bus.h"
#include "CPU.cpp" // <-- need to implement CPU.h

Bus::Bus() {
    cpu = new CPU();
    apu = new APU();
    cpu->connectBus(this);  // Connect CPU to Bus
}

Bus::~Bus() {
    delete cpu;
    delete apu;
}

void Bus::write(uint16_t address, uint8_t data) {

    if (address >= 0x0000 && address <= 0x1FFF) {
        cpuRam[address & 0x07FF] = data;
    } else if (address >= 0x2000 && address <= 0x3FFF) {
        ppu.cpuWrite(address & 0x0007, data);
    } else if ((address >= 0x4000 && address <= 0x4013) || address == 0x4015 || address == 0x4017) {
        apu->writeRegister(address, data);
    } else if (address == 0x4014) {
        // TODO: write to address for DMA transfer
    } else if (address >= 0x4016 && address <= 0x4017) {
        // TODO: write to address and save controller state
    } else if (address >= 0x4020 && address <= 0xFFFF) {
        // TODO: write to cartridge memory
    }
}

uint8_t Bus::read(uint16_t address) {
    if (address >= 0x0000 && address <= 0x1FFF) {
        return cpuRam[address & 0x07FF];
    } else if (address >= 0x2000 && address <= 0x3FFF) {
        // TODO: read from PPU registers and mirror
        //return ppuRegister[(address - 0x2000) % 0x8];
    } else if ((address >= 0x4000 && address <= 0x4013) || address == 0x4015 || address == 0x4017) {
        // return apu->cpuRead(address);
    } else if (address == 0x4014) {
        // TODO: read from address for DMA transfer
    } else if (address >= 0x4016 && address <= 0x4017) {
        // TODO: read from address and save controller state
    } else if (address >= 0x4020 && address <= 0xFFFF) {
        // TODO: read from cartridge memory
    }
    return -1;
}

void Bus::reset() const {
    cpu->reset();
    apu->reset();
    // TODO: add resets for other components
}

void Bus::clock() {
    clockCounter++;

    apu->clock();

    if (clockCounter % 3 == 0) {
        // cpu->clock()
    }
}
