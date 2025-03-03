#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <array>
#include <iostream>
#include <iomanip>
#include "PPU.h"

void PPU::cpuWrite(uint16_t addr, uint8_t data) {
    switch (addr) {
        case 0x0000: // CRTL
            PPUCTRL = data;
            break;
        case 0x0001: // MASK
            PPUMASK = data;
            break;
        case 0x0002: // STATUS
            PPUSTATUS = data;
            break;
        case 0x0003: // OAM Address
            OAMADDR = data;
            break;
        case 0x0004: // OAM Data
            OAMDATA[OAMADDR] = data;
            break;
        case 0x0005: // SCROLL
            // first write to scroll register
            if (w == 0) {
                x = data & 0x07;
                t.coarse_x = data >> 3;
                w = 1;
            }
            // second write to scroll register
            else if(w == 1) {
                t.fine_y = data & 0x07;
                t.coarse_y = data >> 3;
                w = 0;
            }
            break;
        case 0x0006: // PPU Address
            // write to high byte on first write
            if (w == 0) {
                t.vram_register = static_cast<uint16_t>((data & 0x3F) << 8) | (t.vram_register & 0x00FF);
                w = 1;
            }
            // write to low byte on second write and copy to vram
            else if(w == 1) {
                t.vram_register = (t.vram_register & 0xFF00) | data;
                v = t;
                w = 0;
            }
            break;
        case 0x0007: // PPU Data
            // Todo: create write function for internal ppu memory and write PPU Data through that function to handle mirroring
            // Todo: increment I bit of CRTL register by 1 or 32 depending on vertical or horizontal mode
            break;
    }
}

    void PPU::connectROM(NESROM& ROM) {
        this->ROM = &ROM;
    }
