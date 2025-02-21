#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <array>
#include <iostream>
#include <iomanip>
#include "Bus.h"

class PPU {
public:
    // Internal Registers
    // TODO: Is there a better implementation for holding partial bytes? Ex. 15 bits, 3 bits, 1 bit
    uint16_t v = 0x0000;            // Current VRAM Address --- 15 bits
    uint16_t t = 0x0000;            // Temporary VRAM Address --- 15 bits
    uint8_t x = 0x00;               // Fine X Scroll --- 3 bits
    uint8_t w = 0x00;               // First or Second write toggle --- 1 bit

    // I/O Registers
    uint8_t PPUCTRL = 0x00;         // Controller
    uint8_t PPUMASK = 0x00;         // Mask
    uint8_t PPUSTATUS = 0x00;       // Status
    uint8_t OAMADDR = 0x00;         // Sprite RAM address
    uint8_t OAMDATA = 0x00;         // Sprite RAM data
    uint8_t PPUSCROLL = 0x00;       // X and Y scroll
    uint8_t PPUADDR = 0x00;         // VRAM address
    uint8_t PPUDATA = 0x00;         // VRAM data
    
    uint8_t OAMDMA = 0x00;          // Sprite DMA

    enum PPUCTRL_BITS {
        V = (1 << 0),               // Vblank NMI Enable
        P = (1 << 1),               // PPU Master/Slave Select
        H = (1 << 2),               // Sprite Size
        B = (1 << 3),               // Background pattern table address
        S = (1 << 4),               // Sprite pattern table address
        I = (1 << 5),               // VRAM address increment per CPU read/write of PPUDATA
        N1 = (1 << 6),              // Base nametable address
        N2 = (1 << 7)               // Base nametable address
    };

    enum PPUMASK_BITS {
        B = (1 << 0),               // Emphasize blue
        G = (1 << 1),               // Emphasize green
        R = (1 << 2),               // Emphasize red
        s = (1 << 3),               // Enable sprite rendering
        b = (1 << 4),               // Enable background rendering
        M = (1 << 5),               // Show sprites in leftmost 8 pixels of screen
        m = (1 << 6),               // Show background in leftmost 8 pixels of screen
        G = (1 << 7)                // Greyscale
    };

    enum PPUSTATUS_BITS {
        V = (1 << 0),               // Vblank flag
        S = (1 << 1),               // Sprite 0 hit flag
        O = (1 << 2)                // Sprite overflow flag
    };
};