#include "NES.h"


void NES::load_rom(const char *filename) {
    if (on == false) {
        rom.load(filename);
        rom_loaded = true;
        uint16_t memory_address = 0x0000;
        bus.connectROM(rom);


        // write CHR ROM to ppu memory
        for (int i = 0; i < 1024 * 8; i++) {
            bus.ppu.writePatternTable(memory_address, rom.chrRom[memory_address]);
            memory_address++;
        }
        bus.ppu.decodePatternTable();
        memory_address = 0x0000;

        // Write prg ROM to CPU Memory
        // this section is specifically for NROM, changes will be necessary for future mappers
        if (rom.mirrored) {
            uint16_t memory_address_cpu = 0x8000;
            uint16_t memory_address_cpu_mirror = 0xC000;

			// NROM-128
            for (int i = 0; i < 1024 * 16; i++) {
                uint8_t prgByte = rom.prgRom[memory_address];
                memory_address ++;
                bus.cpu->writeBus(memory_address_cpu, prgByte);
                bus.cpu->writeBus(memory_address_cpu_mirror, prgByte);
                memory_address_cpu ++;
                memory_address_cpu_mirror ++;
            }
        } else {
        	uint16_t memory_address_cpu = 0x8000;

			// NROM-256
            for (int i = 0; i < 1024 * 32; i++) {
                uint8_t prgByte = rom.prgRom[memory_address];
                memory_address ++;
                bus.cpu->writeBus(memory_address_cpu, prgByte);
                memory_address_cpu ++;
            }
        }
        // Debugging reset vector
        uint8_t lo = bus.read(0xFFFC);
        uint8_t hi = bus.read(0xFFFD);
        uint16_t resetVector = (hi << 8) | lo;
        printf("Reset Vector @FFFC = %02X\n", lo);
        printf("Reset Vector @FFFD = %02X\n", hi);
        printf("Reset Vector Address = %04X\n", resetVector);
    }
}

void NES::initNES() {
    if (on == true) {
        return;
    }
    bus.cpu->reset();
    //Uncomment for finite CPU testing with nestest.nes
    //cpu.PC = 0xC000;
    on = true;
}

void NES::run() {
    while (on) {
        //cpu.PC = 0xC000;
        int counter = 0;
        for (int i = 0;i < 10000; i++) {
            uint8_t opcode = bus.cpu->readBus(bus.cpu->PC);
            printf("Opcode: %02X\n", opcode);
            printf("counter %d \n", counter);
            bus.cpu->printRegisters();
            bus.cpu->execute();


            uint8_t test_passed = bus.cpu->readBus(0x002);
            printf("test_passed 0x%02X\n\n", test_passed);
            counter++;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void NES::cycle() {
    if(on == true) {
        // Uncomment to test NES at full speed, might need to add more code if system is running too fast.
          double fps = 1./60.;
          auto start = std::chrono::high_resolution_clock::now();
          while (true) {
              bus.clock();
              auto end = std::chrono::high_resolution_clock::now();
              std::chrono::duration<double> elapsed_time = end - start;
              std::chrono::duration<double> frame_time(fps);
              if ((elapsed_time) > frame_time) {
                  bus.cpuClockCounter = 0;
                  break;
              }
          }
        //bus.clock();
    }
}

void NES::end() {
    on = false;
}


uint32_t* NES::getFramebuffer() {
    // for (int i = 0; i < 256 * 240; i++) {
    //     rgbFramebuffer[i] = nesPalette[i % 64];
    //     uint8_t colorIndex = framebuffer[i];  // Get NES color index
    //     rgbFramebuffer[i] = 0xFF000000 | nesPalette[colorIndex % 64];  // Convert to 32-bit ARGB
    // }
    return bus.ppu.nextFrame;
}

void NES::RandomizeFramebuffer() {
    for (int i = 0; i < 256 * 240; i++) {
        uint8_t r = rand() % 256;
        uint8_t g = rand() % 256;
        uint8_t b = rand() % 256;

        // Set the pixel in framebuffer as a 32-bit ARGB value
        framebuffer[i] = 0xFF000000 | (r << 16) | (g << 8) | b;
    }
}

