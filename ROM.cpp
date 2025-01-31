#include <iostream>
#include <fstream>
#include <cstdint>

// NES ROM header size
const size_t NES_HEADER_SIZE = 16;

// NES ROM header structure
struct NESHeader {
    char header[4];         // Should be "NES\x1A"
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
    uint8_t* prgRom;
    uint8_t* chrRom;

    // function to load ROM
    bool load(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filepath << std::endl;
            return false;
        }

        // Read the header
        NESHeader header;
        file.read(reinterpret_cast<char*>(&header), NES_HEADER_SIZE);
        if (!isValidHeader(header)) {
            std::cerr << "Invalid NES file: " << filepath << std::endl;
            return false;
        }

        // Calculate sizes based on the header
        size_t prgRomSize = header.prgRomSize * 16 * 1024;
        size_t chrRomSize = header.chrRomSize * 8 * 1024;

        // Dynamically allocate memory for PRG ROM
        prgRom = new uint8_t[prgRomSize];
        file.read(reinterpret_cast<char*>(prgRom), prgRomSize);

        // Dynamically allocate memory for CHR ROM (if present)
        if (chrRomSize > 0) {
            chrRom = new uint8_t[chrRomSize];
            file.read(reinterpret_cast<char*>(chrRom), chrRomSize);
        }

        // Close the file
        file.close();
        std::cout << "Successfully loaded NES ROM: " << filepath << std::endl;
        return true;
    }

    uint8_t readMemoryPRG(const uint16_t address) {
        return prgRom[address];
    }

    uint8_t readMemoryCHR(uint16_t address) {
        return chrRom[address];
    }

    bool isValidHeader(const NESHeader& header) {
        // Check for "NES\x1A" header
        return header.header[0] == 'N' &&
               header.header[1] == 'E' &&
               header.header[2] == 'S' &&
               header.header[3] == 0x1A;
    }

    void printHeaderInfo(const NESHeader& header) {
        std::cout << "NES ROM Header Information:" << std::endl;
        std::cout << "  PRG ROM Size: " << static_cast<int>(header.prgRomSize) << " x 16KB" << std::endl;
        std::cout << "  CHR ROM Size: " << static_cast<int>(header.chrRomSize) << " x 8KB" << std::endl;
        std::cout << "  Flags6: " << std::hex << static_cast<int>(header.flags6) << std::dec << std::endl;
        std::cout << "  Flags7: " << std::hex << static_cast<int>(header.flags7) << std::dec << std::endl;
    }
};