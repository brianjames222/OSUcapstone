#include "Bus.h"
#include "CPU.h"
#include <thread>
#include <iostream>

Bus::Bus() {
    cpu = new CPU();
    apu = new APU();
    std::cout << "Bus constructor: CPU PC = " << std::hex << cpu->PC << "\n";
    cpu->connectBus(this);  // Connect CPU to Bus
}

Bus::~Bus() {
    delete cpu;
    delete apu;
}

void Bus::write(uint16_t address, uint8_t data) {
    // Handles CPU RAM --> 0x0000-0x1FFF (mirrored every 0x0800)
    if (address >= 0x0000 && address <= 0x1FFF) {
        cpuRam[address & 0x07FF] = data;


    // Handles PPU registers --> 0x20000-0x3FFF (mirrored every 8 bytes)
    } else if (address >= 0x2000 && address <= 0x3FFF) {
        ppu.cpuWrite(address & 0x0007, data);


    // Handles APU registers --> 0x4000-0x4013, 0x4015, 0x4017
    } else if ((address >= 0x4000 && address <= 0x4013) || address == 0x4015 || address == 0x4017) {
        apu->write_register(address, data);


    // Handles OAM DMA --> 0x4014
    } else if (address == 0x4014) {
        DMATransfer = true;
        // DMA Page + DMA Address make a 16-bit address for the CPU bus
        DMAPage = data;
        DMAAddress = 0x00;


    // Handles controller ports --> 0x4016-0x4017
    } else if (address >= 0x4016 && address <= 0x4017) {
        // TODO: write to address and save controller state


    // Handles cartridge space --> 0x4020-0xFFFF
    } else if (address >= 0x4020 && address <= 0xFFFF) {
        if (rom) {
            bool handled = false;

            // For NROM-128 (1 x 16KB) or NROM-256 (2 x 16KB)
            if (rom->ROMheader.flags6 == 0x00 || rom->ROMheader.flags6 == 0x01) {
                uint16_t offset;

                if (rom->mirrored && address >= 0xC000) {
                    offset = address - 0xC000;
                    if (offset < 0x4000) {
                        rom->prgRom[offset] = data;
                        handled = true;
                    }
                } else if (!rom->mirrored && address >= 0x8000) {
                    offset = address - 0x8000;
                    if (offset < rom->ROMheader.prgRomSize * 16 * 1024) {  // 🔒 BOUNDS CHECK
                        rom->prgRom[offset] = data;
                        handled = true;
                    }
                }
            }

            if (!handled) {
                static int prgWriteWarnCount = 0;
                if (prgWriteWarnCount++ < 10) {
                    std::cerr << "Warning --> Attempted write to PRG-ROM space at 0x"
                              << std::hex << address << " — ignored.\n";
                } else if (prgWriteWarnCount == 10) {
                    std::cerr << "(Further PRG-ROM write warnings suppressed...)\n";
                }
            }
        } else {
            // Allow test fallback writes to fake RAM
            cpuRam[address & 0x07FF] = data;
        }
    }
}

uint8_t Bus::read(uint16_t address) {
    // Handles CPU RAM --> 0x0000-0x1FFF (mirrored every 0x0800)
    if (address >= 0x0000 && address <= 0x1FFF) {
        return cpuRam[address & 0x07FF];


    // Handles PPU registers --> 0x20000-0x3FFF (mirrored every 8 bytes)
    } else if (address >= 0x2000 && address <= 0x3FFF) {
        return ppu.cpuRead(address & 0x0007);


    // Handles APU registers --> 0x4000-0x4013, 0x4015, 0x4017
    } else if ((address >= 0x4000 && address <= 0x4013) || address == 0x4015 || address == 0x4017) {
        return apu->read_register(address);


    // Handles OAM DMA --> 0x4014
    } else if (address == 0x4014) {
        return 0; // OAM DMA is usually write-only


    } else if (address >= 0x4016 && address <= 0x4017) {
        // TODO: read from address and save controller state
        return 0; // placeholder

    // Handles cartridge space --> 0x4020-0xFFFF
    } else if (address >= 0x4020 && address <= 0xFFFF) {
        if (rom) {
            return rom->readMemoryPRG(address); // ← eventually implement this
        } else {
            return cpuRam[address & 0x07FF]; // Allow fallback read in tests
        }
    }
    return -1;  // should this be 0?
}

void Bus::reset() {
    cpu->reset();
    apu->reset();
    ppu.reset();
    clockCounter = 0;
    cpuClockCounter = 0;
    DMATransfer = false;
    DMACanStart = false;
    DMAPage = 0x00;
    DMAAddress = 0x00;
    DMAData = 0x00;
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
