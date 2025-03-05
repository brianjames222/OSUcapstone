#ifndef PPU_H
#define PPU_H

#include <cstdint>  // For uint8_t and uint16_t
#include "ROM.h"
class PPU {
public:
    // Internal Registers
    union vram {
        struct {
            uint16_t coarse_x: 5;
            uint16_t coarse_y: 5;
            uint16_t nametable_x: 1;
            uint16_t nametable_y: 1;
            uint16_t fine_y: 3;
            uint16_t unused_bit: 1;
        };
        uint16_t vram_register = 0x0000;
    };

    // Current VRAM Address --- 15 bits
    vram v;
    // Temporary VRAM Address --- 15 bits
    vram t;
    uint8_t x = 0x00;               // Fine X Scroll --- 3 bits
    uint8_t w = 0x00;               // First or Second write toggle --- 1 bit

    // I/O Registers
    union PPUSTATUS {
        struct {
            uint8_t unused: 5;
            uint8_t sprite_overflow: 1;
            uint8_t sprite_zerohit: 1;
            uint8_t vblank: 1;
        };
        uint8_t reg;
    } status;

    union PPUCTRL {
        struct {
            uint8_t nametable_x: 1;
            uint8_t nametable_y: 1;
            uint8_t increment_type: 1;
            uint8_t sprite_pattern: 1;
            uint8_t background_pattern: 1;
            uint8_t sprite_size: 1;
            uint8_t ppu_master: 1;
            uint8_t vblank_nmi_enable: 1;
        }; uint8_t reg;
    } control;

    //uint8_t PPUCTRL = 0x00;         // Controller
    uint8_t PPUMASK = 0x00;         // Mask
    //uint8_t PPUSTATUS = 0x00;       // Status
    uint8_t OAMADDR = 0x00;         // Sprite RAM address
    uint8_t PPUSCROLL = 0x00;       // X and Y scroll
    uint8_t PPUADDR = 0x00;         // VRAM address
    uint8_t PPUDATA = 0x00;         // VRAM data

    // Object Attribute Memory (OAM) - 64 sprites
    struct ObjectAttributeMemory {
        uint8_t y;          // Y position of a sprite
        uint8_t id;         // ID of a tile in pattern memory
        uint8_t attribute;  // Attribute flags
        uint8_t x;          // X position of a sprite
    } OAM[64]{};

    //TODO: Create memory for name table and palettes

    // Cant have the values in an enum equal the same letter as it causes compiler errors
    // It's the same as two variables having the same name.

    // enum PPUCTRL_BITS {
    //     V = (1 << 0),               // Vblank NMI Enable
    //     P = (1 << 1),               // PPU Master/Slave Select
    //     H = (1 << 2),               // Sprite Size
    //     B = (1 << 3),               // Background pattern table address
    //     S = (1 << 4),               // Sprite pattern table address
    //     I = (1 << 5),               // VRAM address increment per CPU read/write of PPUDATA
    //     N1 = (1 << 6),              // Base nametable address
    //     N2 = (1 << 7)               // Base nametable address
    // };
    //
    // enum PPUMASK_BITS {
    //     B = (1 << 0),               // Emphasize blue
    //     G = (1 << 1),               // Emphasize green
    //     R = (1 << 2),               // Emphasize red
    //     s = (1 << 3),               // Enable sprite rendering
    //     b = (1 << 4),               // Enable background rendering
    //     M = (1 << 5),               // Show sprites in leftmost 8 pixels of screen
    //     m = (1 << 6),               // Show background in leftmost 8 pixels of screen
    //     G = (1 << 7)                // Greyscale
    // };
    //
    // enum PPUSTATUS_BITS {
    //     V = (1 << 0),               // Vblank flag
    //     S = (1 << 1),               // Sprite 0 hit flag
    //     O = (1 << 2)                // Sprite overflow flag
    // };

    uint8_t* OAMDATA = reinterpret_cast<uint8_t *>(OAM);
    uint8_t OAMDMA = 0x00;          // Sprite DMA

    // Method for writing data to PPU registers
    void cpuWrite(uint16_t addr, uint8_t data);

    void writePPU(uint16_t addr, uint8_t data);

    uint8_t readPPU(uint16_t addr);

    // Method to connect ROM to PPU
    void connectROM(NESROM& ROM);
    // Init ROM
    NESROM* ROM{};
    
    // Pattern tables------------------------------------------------------------------------------------
    std::array<uint8_t, 4096 * 2> patternTables; // two pattern tables of 256 tiles each (4096 / 16)

	// read and write may be modified / unused (e.x. caller specifies table + tile, function figures out what to return)
	// useful for testing?
    uint8_t readPatternTable(uint16_t addr);

    void writePatternTable(uint16_t addr, uint8_t data);
    
    // method to get a tile, returned as an 8-byte array of pixel info (0-3)
    void getTile(uint8_t tileIndex, uint8_t* tileData, bool table1);

    void setPixel(uint8_t x, uint8_t y, uint32_t color);

    void clock();

    uint16_t cycle = 0;
    uint16_t scanline = 0;
    bool complete_frame = false;
    bool nmi = false;

    uint8_t framebuffer[256 * 240]{};  // 8-bit color indices
    uint32_t rgbFramebuffer[256 * 240]{}; // 32-bit color for SDL

    uint32_t nesPalette[64] = {
        0x545454, 0x001E74, 0x0810A0, 0x300088, 0x44004C, 0x5C0020, 0x540400, 0x3C1800,
        0x202A00, 0x083A00, 0x004000, 0x003C0A, 0x003238, 0x000000, 0x000000, 0x000000,
        0x989696, 0x074C64, 0x3032EC, 0x5C1EEC, 0x8814B0, 0xA01464, 0x982220, 0x783C0A,
        0x223C00, 0x0A6600, 0x006400, 0x00583A, 0x00393B, 0x001B2A, 0x1F1F1F, 0x111111,
        0xA9A9A9, 0x023C9C, 0x2449CC, 0x3E40CF, 0x6B6C99, 0x7F77AA, 0x8B95C2, 0x8C8A7F,
        0xFF00A0, 0xAA0D42, 0x8C1A4E, 0x801D53, 0x922C6F, 0x9E4A6E, 0x92515D, 0x774E53,
        0x0F77BB, 0x0B9DE8, 0x2F67E0, 0x6A7FFF, 0xA2B9F1, 0x9CC6DB, 0x70A5E9, 0x5C82C7,
        0x080F99, 0x13D1F6, 0x35C8FD, 0x7F8F9E, 0xC8E0F5, 0xF3FBFF, 0xC8EBFF, 0x7F9FF7
    };
};

#endif // PPU_H
