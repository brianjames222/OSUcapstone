#include "CPU.cpp"
#include <cassert>


int main() {

    // CPU testing
    CPU cpu;

    // Check start up values
    cpu.printRegisters();

    // Check write
    cpu.writeMemory(0x10, 0xAB);
    cpu.writeMemory(0x0000, 0xAB);
    cpu.printMemory();

    // OOB, should return error
    cpu.writeMemory(0x801, 0xAB); // 2049
    cpu.readMemory(0xFFF); // 4095

    cpu.setFlag(CPU::FLAGS::Z, true);
    cpu.printRegisters();
    printf("Status Flag Z: %d\n", cpu.getFlag(CPU::FLAGS::Z));

    std::cout << "Memory at 0x10: 0x" << std::hex << static_cast<int>(cpu.readMemory(0x10)) << "\n";
    printf("Value at address 0x0000: %02X\n", cpu.readMemory(0x0000));

    // OPcode tests:-----------------------------------------------------------------------------------
    std::cout << "\nOpcode Tests:\n";

    // Test Program
    cpu.writeMemory(0x00, 0xA9); // LDA Immediate AA
    cpu.writeMemory(0x01, 0xAA);
    cpu.writeMemory(0x02, 0xA5); // LDA Zero Page
    cpu.writeMemory(0x03, 0x35); 
    cpu.writeMemory(0x35, 0xBB); // Load BB into 0x35
    cpu.writeMemory(0x04, 0xB5); // LDA Zero Page X
    cpu.writeMemory(0x05, 0x35);
    cpu.writeMemory(0x38, 0xCC); // Load CC into 0x38
    cpu.writeMemory(0x06, 0xAD); // LDA Absolute
    cpu.writeMemory(0x07, 0x01);
    cpu.writeMemory(0x08, 0x02);
    cpu.writeMemory(0x0201, 0xDD); // Load DD into 0x0201
    cpu.writeMemory(0x09, 0xBD); // LDA Absolute X
    cpu.writeMemory(0x0A, 0x01);
    cpu.writeMemory(0x0B, 0x02);
    cpu.writeMemory(0x0204, 0xEE); // Load EE into 0x0204
    cpu.writeMemory(0x0C, 0xB9); // LDA Absolute Y
    cpu.writeMemory(0x0D, 0x01);
    cpu.writeMemory(0x0E, 0x02);
    cpu.writeMemory(0x0203, 0xFF); // Load FF into 0x0203
    cpu.writeMemory(0x0F, 0xA1); // LDA Indirect X
    cpu.writeMemory(0x10, 0x20);
    cpu.writeMemory(0x23, 0xAA); // Write 0x01AA to 0x23/24
    cpu.writeMemory(0x24, 0x01);
    cpu.writeMemory(0x01AA, 0xAA); // Load AA into 0x01AA
    cpu.writeMemory(0x11, 0xB1); // LDA Indirect Y
    cpu.writeMemory(0x12, 0x23);
    cpu.writeMemory(0x01AC, 0xBB); // Load BB into 0x01AC

    // Initialize PC
    cpu.PC = 0x0000;
    cpu.X = 3;
    cpu.Y = 2;
    cpu.execute();
    assert(cpu.A == 0xAA);
    cpu.execute();
    assert(cpu.A == 0xBB);
    cpu.execute();
    assert(cpu.A == 0xCC);
    cpu.execute();
    assert(cpu.A == 0xDD);
    cpu.execute();
    assert(cpu.A == 0xEE);
    cpu.execute();
    assert(cpu.A == 0xFF);
    cpu.execute();
    assert(cpu.A == 0xAA);
    cpu.printRegisters();
    cpu.execute();
    assert(cpu.A == 0xBB);
    cpu.printRegisters();

    std::cout << "Opcode tests passed!\n";

    // Check reset function
    std::cout << "\nReset test:\n\nCurrent values:\n";
    cpu.printRegisters();
    cpu.setFlag(CPU::FLAGS::Z, 1);
    cpu.setFlag(CPU::FLAGS::V, 1);
    cpu.setFlag(CPU::FLAGS::I, 0);
    cpu.PC = 0x0000;
    cpu.S = 0xAA;
    std::cout << "\nUpdated values:\n";
    cpu.printRegisters();
    cpu.reset();
    std::cout << "\nAfter reset:\n";
    cpu.printRegisters();

    return 0;
}

