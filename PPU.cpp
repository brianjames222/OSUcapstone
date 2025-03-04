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
void PPU::getTile(uint8_t tileIndex, uint16_t* tileData, bool table1) {
	// get the index by multiplying the tileIndex (0 - 255) by 16 (each tile is 16 bytes)
    uint16_t index = tileIndex * 16;
    if (!table1) index += 256;			// second table
    
    /* Pretty sure this implementation is actually wrong, forgot to account for each individual bit
    for (int i = 0; i < 8; i++) {
        tileData[i] = (patternTables[index + i] << 1) | (patternTables[index + i + 8] & 0x01); // Combine bit planes
    } */
    
    // let's try again:
    for (int i = 0; i < 8; ++i) {
		// combine the data from both planes
		uint8_t low = patternTables[index + i];			// plane 0 - lower bit of each pixel
		uint8_t high = patternTables[index + i + 8];	// plane 1 - higher bit of each pixel

		uint16_t combinedPixels = 0;

		// extract bits for each pixel in a row
		for (int j = 0; j < 8; j++) {
			//start at the far left, move down as j increases
		    uint8_t bit0 = (low >> (7 - j)) & 0x01;
		    uint8_t bit1 = (high >> (7 - j)) & 0x01;
		    
		    // Combine the bits together (bit1 << 1) + bit0
		    combinedPixels |= ((bit1 << 1) | bit0) << (j * 2); // Shift left for the next pixel
		}

		// Store the combined result
		((uint16_t*)tileData)[i] = combinedPixels; // Each entry now represents 8 pixels (16 bits)
	}
}


void PPU::clock() {
    // TODO: add the code for one clock cycle of the PPU
    // There should be a lot of logic to implement as the ppu is going through the scanlines.

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
