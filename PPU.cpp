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
            control.reg = data;
            break;
        case 0x0001: // MASK
            PPUMASK = data;
            break;
        case 0x0002: // STATUS
            status.reg = data;
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
            // Todo: increment I bit of CRTL register by 1 or 32 depending on vertical or horizontal mode
            break;
    }
}

void PPU::writePPU(uint16_t addr, uint8_t data) {
    //TODO: Write to ppu bus between 0x0000 and 0x3FFF
}

uint8_t PPU::readPPU(uint16_t addr) {
    //TODO: Read from ppu bus between 0x0000 and 0x3FFF
}

void PPU::connectROM(NESROM& ROM) {
    this->ROM = &ROM;
}

// Pattern tables ----------------------------------------------------------------------------------------------------

// modify to allow specification of table, tile, plane?
uint8_t PPU::readPatternTable(uint16_t addr) {
    return patternTables[addr];
}

void PPU::writePatternTable(uint16_t addr, uint8_t data) {
    patternTables[addr] = data;
}

// fetch a tile
void PPU::getTile(uint8_t tileIndex, uint8_t* tileData, bool table1) {
	// get the index by multiplying the tileIndex (0 - 255) by 16 (each tile is 16 bytes)
    uint16_t index = tileIndex * 16;
    if (!table1) index += 256;			// second table
    
    for (int i = 0; i < 8; i++) {
        tileData[i] = (patternTables[index + i] << 1) | (patternTables[index + i + 8] & 0x01); // Combine bit planes
    }
}

void PPU::setPixel(uint8_t x, uint8_t y, uint32_t color) {
    rgbFramebuffer[y * 256 + x] = 0xFF000000 | color;
}


void PPU::clock() {
    // TODO: add the code for one clock cycle of the PPU
    // There should be a lot of logic to implement as the ppu is going through the scanlines.


    if (scanline < 241 && cycle < 256) {
        setPixel(cycle, scanline, nesPalette[cycle % 64]);
    }
    // if rendering of the screen is over, enable nmi vblank
    if (scanline == 241 && cycle == 1) {
        status.vblank = 1;
        // Check control register
        if (control.vblank_nmi_enable) {
            nmi = true;
        }
    }
    cycle++;
    if (cycle >= 341) {
        cycle = 0;
        scanline++;

        if (scanline >= 261) {
            scanline = -1;
            complete_frame = true;
        }
    }
}
