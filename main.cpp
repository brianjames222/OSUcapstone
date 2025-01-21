#include "CPU.cpp"
#include <cassert>


int main() {

    // CPU testing
    CPU cpu;
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
    // Load value at 0x0 into register A
    cpu.PC = 0x0000;
    cpu.instruction(0xA5);
    assert(cpu.A == 0xAB);

    // Store A into mem[0x0001]
    cpu.PC = 0x0001;
    cpu.instruction(0x85);
    assert(cpu.memory[0x0001] == 0xAB);

    // Load AB into X
    cpu.PC = 0x0001;
    cpu.instruction(0xA6);
    assert(cpu.X == 0xAB);

    // Store X into mem[0x0010]
    cpu.PC = 0x0010;
    cpu.instruction(0x86);
    assert(cpu.memory[0x0010] == 0xAB);

    // Load AB into Y
    cpu.PC = 0x0010;
    cpu.instruction(0xA4);
    assert(cpu.Y == 0xAB);

    // Store Y into memory[0x0100]
    cpu.PC = 0x0100;
    cpu.instruction(0x84);
    assert(cpu.memory[0x0100] == 0xAB);

    // Transfer A to X, and X to A
    cpu.A = 0x05;
    cpu.instruction(0xAA);
    assert(cpu.A == cpu.X);
    cpu.X = 0x0A;
    cpu.instruction(0x8A);
    assert(cpu.A == cpu.X);

    // Transfer A to Y, and Y to A
    cpu.A = 0x0F;
    cpu.instruction(0xA8);
    assert(cpu.A == cpu.Y);
    cpu.Y = 0x14;
    cpu.instruction(0x98);
    assert(cpu.A == cpu.Y);

    // Increment value at 0x0000 twice
    cpu.PC = 0x0000;
    cpu.instruction(0xE6);
    cpu.instruction(0xE6);
    assert(cpu.memory[0x0000] == 0xAD);

    // Decrement value at 0x0000 once
    cpu.PC = 0x0000;
    cpu.instruction(0xC6);
    assert(cpu.memory[0x0000] == 0xAC);

    // Inc/Dec X and Y
    cpu.X = 0x0;
    cpu.Y = 0x0;
    cpu.instruction(0xE8);
    cpu.instruction(0xE8);
    cpu.instruction(0xE8);
    cpu.instruction(0xCA);
    cpu.instruction(0xC8);
    cpu.instruction(0xC8);
    cpu.instruction(0x88);
    cpu.instruction(0x88);
    assert(cpu.X == 0x2);
    assert(cpu.Y == 0x0);

    cpu.printRegisters();
    std::cout << "Opcode tests passed!\n";


    return 0;
}
