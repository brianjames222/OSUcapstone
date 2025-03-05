#ifndef NESROM_H
#define NESROM_H

#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>

// NES ROM header size
const size_t NES_HEADER_SIZE = 16;

// NES ROM header structure
struct NESHeader {
    char header[4];        // Should be "NES\x1A"
    uint8_t prgRomSize;    // Size of PRG ROM in 16KB units
    uint8_t chrRomSize;    // Size of CHR ROM in 8KB units
    uint8_t flags6;        // Flags 6
    uint8_t flags7;        // Flags 7
    uint8_t prgRamSize;    // Size of PRG RAM in 8KB units (0 means 8KB)
    uint8_t flags9;        // Flags 9
    uint8_t flags10;       // Flags 10 (unofficial)
    uint8_t padding[5];    // Padding, should be zero
};

class NESROM {
public:
    uint8_t* prgRom;          // Pointer to PRG ROM data
    uint8_t* chrRom;          // Pointer to CHR ROM data
    NESHeader ROMheader;
    bool mirrored = false;    // Flag for NROM-128 mirroring

    // Function to detect and initialize the mapper based on header and file data
    void detect_mapper(const NESHeader& header, std::ifstream& file);

    // Function to load the ROM from a file
    bool load(const std::string& filepath);

    // Function to read PRG memory
    uint8_t readMemoryPRG(const uint16_t address);

    // Function to read CHR memory
    uint8_t readMemoryCHR(uint16_t address);

    // Function to validate the ROM header
    bool isValidHeader(const NESHeader& header);

    // Function to print ROM header information
    void printHeaderInfo(const NESHeader& header);


};

#endif // NESROM_H