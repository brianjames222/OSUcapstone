#include "Bus.h"

#include <thread>

#include "CPU.cpp" // <-- need to implement CPU.h


Bus::Bus() {
    cpu = new CPU();
    apu = new APU();
    cpu->connectBus(this);  // Connect CPU to Bus
    APU apu;                // not needed?
}

Bus::~Bus() = default;

void Bus::write(uint16_t address, uint8_t data) {
    //printf("address 0x%04X   data: 0x%02x\n", address, data);
    if (address >= 0x0000 && address <= 0x1FFF) {
        cpuRam[address & 0x07FF] = data;
    } else if (address >= 0x2000 && address <= 0x3FFF) {
        ppu.cpuWrite(address & 0x0007, data);
    } else if ((address >= 0x4000 && address <= 0x4013) || address == 0x4015 || address == 0x4017) {
        apu->write_register(address, data);
    } else if (address == 0x4014) {
        DMATransfer = true;
        // DMA Page + DMA Address make a 16-bit address for the CPU bus
        DMAPage = data;
        DMAAddress = 0x00;
    } else if (address >= 0x4016 && address <= 0x4017) {
        // TODO: write to address and save controller state
    } else if (address >= 0x4020 && address <= 0xFFFF) {
        // TODO: write to cartridge memory
        // Temporary way of getting rom information, current mappers write to old cpu memory
        cpu->writerom(address, data);
    }
}

uint8_t Bus::read(uint16_t address) {
    if (address >= 0x0000 && address <= 0x1FFF) {
        return cpuRam[address & 0x07FF];
    } else if (address >= 0x2000 && address <= 0x3FFF) {
        return ppu.cpuRead(address & 0x0007);
    } else if ((address >= 0x4000 && address <= 0x4013) || address == 0x4015 || address == 0x4017) {
        return apu->read_register(address);
    } else if (address == 0x4014) {
        // TODO: read from address for DMA transfer
    } else if (address >= 0x4016 && address <= 0x4017) {
        if (address == 0x4016) {
            if (controller_read == 8) {
                copyController = controller1;
                controller_read = 0;
            }
            uint8_t data = copyController.reg & 1;
            copyController.reg = copyController.reg >> 1;
            controller_read++;
            return data;
        }
        return 0;
    } else if (address >= 0x4020 && address <= 0xFFFF) {
        // TODO: read from cartridge memory
        // Temporary way of getting rom information, current mappers write to old cpu memory
        return cpu->readrom(address);
    }
    return -1;
}

void Bus::reset() {
    cpu->reset();
    apu->reset();
    ppu.reset();
    // TODO: add resets for other components
}

void Bus::clock() {
    // Cycle ppu every clock cycle
    ppu.clock();

    // CPU is three times slower than ppu
    if (clockCounter % 3 == 0) {

        // Check if a DMA transfer is happening, it suspends the CPU
        if (DMATransfer) {
            if (!DMACanStart) {
                if (clockCounter % 2 == 1) {
                    DMACanStart = true;
                }
            }
            else {
                // Read from the CPU bus on even clock cycles
                if (clockCounter % 2 == 0) {
                    DMAData = read(DMAPage << 8 | DMAAddress);
                }
                // Write to PPU OAM memory on odd clock cycles
                else {
                    ppu.OAMDATA[DMAAddress] = DMAData;
                    DMAAddress++;

                    // After transfering 256 bytes end the transfer
                    if (DMAAddress == 0x00) {
                        DMATransfer = false;
                        DMACanStart = false;
                    }
                }
            }
        }
        // If no DMA transfer, cycle CPU
        else {
            cpu->cycleExecute();
            cpuClockCounter++;
        }

    }

    // if vblank started, inform cpu through nmi interrupt.
    if (ppu.nmi) {
        ppu.nmi = false;
        cpu->nmi_interrupt();
    }

    clockCounter++;
}

void Bus::connectROM(NESROM& ROM) {
    ppu.connectROM(ROM);
    rom = &ROM;
}
