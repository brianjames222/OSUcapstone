#include <chrono>
#include <thread>
#include <cstdlib>
#include <ctime>

#include "Bus.h"
#include "ROM.cpp"
#include "CPU.cpp"

class NES {
    public:
    CPU cpu;
    NESROM rom{};
    bool on = false;
    bool rom_loaded = false;
    bool A_changed = false;
    int count = 0;
    bool paused = false;


    void load_rom(const char *filename) {
        if (on == false) {
            rom.load(filename);
            rom_loaded = true;
            uint16_t memory_address = 0x0000;
            uint16_t memory_address_cpu = 0xC000;

            // Write prg ROM to CPU Memory
            for (int i = 0;i < 1024 * 16; i++) {
                uint8_t prgByte = rom.prgRom[memory_address];
                memory_address ++;
                cpu.writeMemory(memory_address_cpu, prgByte);
                memory_address_cpu ++;
            }
        }
    }

    void initNES() {
        if (on == true) {
            return;
        }
        cpu.reset();
        cpu.PC = 0xC000;
        on = true;
    }

    void run() {
        while (on) {
            cpu.PC = 0xC000;
            int counter = 0;
            for (int i = 0;i < 10000; i++) {
                uint8_t opcode = cpu.readMemory(cpu.PC);
                printf("Opcode: %02X\n", opcode);
                printf("counter %d \n", counter);
                cpu.printRegisters();
                cpu.execute();


                uint8_t test_passed = cpu.readMemory(0x002);
                printf("test_passed 0x%02X\n\n", test_passed);
                counter++;
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }

    void cycle() {
        if(on == true) {
            cpu.execute();
        }
    }

    void end() {
        on = false;
    }

    uint8_t framebuffer[256 * 240]{};  // 8-bit color indices
    uint32_t rgbFramebuffer[256 * 240]{}; // 32-bit color for SDL

    uint32_t nesPalette[64] = {
        0x666666, 0x002A88, 0x1412A7, 0x3B00A4, 0x5C007E, 0x6E0040, 0x6C0600, 0x561D00,
        0x333500, 0x0B4800, 0x005200, 0x004F08, 0x00404D, 0x000000, 0x000000, 0x000000,
        0x6B6B6B, 0x0B3B95, 0x241CA7, 0x3B10A4, 0x631F84, 0x6D2061, 0x6F3011, 0x562C1A,
        0x344000, 0x0F5500, 0x006100, 0x006148, 0x005459, 0x002B42, 0x2F2F2F, 0x111111,
        0xA9A9A9, 0x023C9C, 0x2449CC, 0x3E40CF, 0x6B6C99, 0x7F77AA, 0x8B95C2, 0x8C8A7F,
        0xFF00A0, 0xAA0D42, 0x8C1A4E, 0x801D53, 0x922C6F, 0x9E4A6E, 0x92515D, 0x774E53,
        0x0F77BB, 0x0B9DE8, 0x2F67E0, 0x6A7FFF, 0xA2B9F1, 0x9CC6DB, 0x70A5E9, 0x5C82C7,
        0x080F99, 0x13D1F6, 0x35C8FD, 0x7F8F9E, 0xC8E0F5, 0xF3FBFF, 0xC8EBFF, 0x7F9FF7
    };

    uint32_t* getFramebuffer() {
        for (int i = 0; i < 256 * 240; i++) {
            rgbFramebuffer[i] = nesPalette[i % 64];
            uint8_t colorIndex = framebuffer[i];  // Get NES color index
            rgbFramebuffer[i] = 0xFF000000 | nesPalette[colorIndex % 64];  // Convert to 32-bit ARGB
        }
        return rgbFramebuffer;
    }

    void RandomizeFramebuffer() {
        for (int i = 0; i < 256 * 240; i++) {
            uint8_t r = rand() % 256;
            uint8_t g = rand() % 256;
            uint8_t b = rand() % 256;

            // Set the pixel in framebuffer as a 32-bit ARGB value
            framebuffer[i] = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
    }


};
