#ifndef CPU_CPP
#define CPU_CPP

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <array>
#include <iostream>
#include <iomanip>

class CPU {
public:
    // Registers
    uint8_t A = 0x00;        // Accumulator
    uint8_t X = 0x00;        // X Register
    uint8_t Y = 0x00;        // Y Register
    uint8_t S = 0xFD;        // Stack Pointer, start at 0xFD
    uint16_t PC = 0x0000;    // Program Counter, read memory at 0xFFFC and 0xFFFD for start of program;
    uint8_t P = I + U;        // Status Flags Register, start with I and U

    // RAM for CPU
    std::array<uint8_t, 64 * 1024> memory{};

    // Flags
    enum FLAGS {
        C = (1 << 0),    // Carry
        Z = (1 << 1),    // Zero
        I = (1 << 2),    // Disable Interrupts
        D = (1 << 3),    // Decimal mode, not used in NES
        B = (1 << 4),    // Break
        U = (1 << 5),    // Unused
        V = (1 << 6),    // Overflow
        N = (1 << 7)     // Negative
    };

    // Returns value at memory address
    uint8_t readMemory(const uint16_t address) const {
    	if (address < memory.size()) {
    		return memory[address];
    	} else {
    		std::cerr << "Address out of bounds: " << address << '\n';
    		return -1;
    	}
    }

    // Writes value to memory address
    void writeMemory(const uint16_t address, const uint8_t value) {
        if (address < memory.size()) memory[address] = value;
        else std::cerr << "Address out of bounds: " << address << '\n';
    }

    // Sets or clears a bit of the status register
    void setFlag(FLAGS flag, bool set) {
        if (set)
            P |= flag;  // Set the flag
        else
            P &= ~flag; // Clear the flag
    }

    // Gets the flag value of a bit of the status register
    uint8_t getFlag(FLAGS flag) const {
        return ((P & flag) != 0) ? 1 : 0;
    }

    // Print the CPU registers
    void printRegisters() const {
        printf("A: [%02X]\nX: [%02X]\nY: [%02X]\nPC: [%04X]\nS: [%02X]\nP: [%02X]\n",
            A, X, Y, PC, S, P);
    }

    // Print the contents of the memory
    void printMemory() const {
        for (size_t i = 0; i < memory.size(); ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(memory[i]) << " ";

            // Print 16 values per line for better readability
            if ((i + 1) % 16 == 0) {
                std::cout << "\n";
            }
        }
    }

    // Set the CPU registers as specified by a console reset
    void reset() {
        const uint16_t read_address = 0xFFFC;
        uint16_t lo = readMemory(read_address);
        uint16_t hi = readMemory(read_address + 1);
        PC = (hi << 8) | lo;
        S = 0xFD;
        P = 0x00;
        setFlag(I, true);
        setFlag(U, true);
    }

    // Read and execute the next instruction
    void execute() {
      // Read the opcode
      uint8_t opcode = readMemory(PC++);

      // Get the address mode and instruction type from the opcode
      //std::cout << "Opcode: 0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(opcode) << std::endl;
      Instruction opcodeInstr = instructionTable[opcode];
      if (opcodeInstr.operation == nullptr || opcodeInstr.addressingMode == nullptr) {
        std::cout << "Error: Invalid opcode"; 
      }

      // Find the address
      uint16_t address = (this->*opcodeInstr.addressingMode)();

      // Execute the instruction
      (this->*opcodeInstr.operation)(address);

    }

    // Instruction struct for storing addressingMode and operation
    struct Instruction {
      void (CPU::*operation)(uint16_t);
      uint16_t (CPU::*addressingMode)();
    };

    // Intialize instructionTable with null values
    Instruction instructionTable[256];

    void initInstructionTable() {
        for (int i = 0; i < 256; i++) {
          instructionTable[i] = {nullptr, nullptr};
        };


        // Add instructions
        instructionTable[0xA9] = {&CPU::LDA, &CPU::Immediate};
        instructionTable[0xA5] = {&CPU::LDA, &CPU::ZeroPage};
        instructionTable[0xB5] = {&CPU::LDA, &CPU::ZeroPageX};
        instructionTable[0xAD] = {&CPU::LDA, &CPU::Absolute};
        instructionTable[0xBD] = {&CPU::LDA, &CPU::AbsoluteX};
        instructionTable[0xB9] = {&CPU::LDA, &CPU::AbsoluteY};
        instructionTable[0xA1] = {&CPU::LDA, &CPU::IndirectX};
        instructionTable[0xB1] = {&CPU::LDA, &CPU::IndirectY};
        instructionTable[0x4C] = {&CPU::JMP, &CPU::Absolute};
        instructionTable[0x6C] = {&CPU::JMP, &CPU::IndirectJMP};
        instructionTable[0x20] = {&CPU::JSR, &CPU::Absolute};
        instructionTable[0x60] = {&CPU::RTS, &CPU::Implied};
        instructionTable[0x00] = {&CPU::BRK, &CPU::Implied};
        instructionTable[0x40] = {&CPU::RTI, &CPU::Implied};
        instructionTable[0x48] = {&CPU::PHA, &CPU::Implied};
        instructionTable[0x68] = {&CPU::PLA, &CPU::Implied};
        instructionTable[0x08] = {&CPU::PHP, &CPU::Implied};
        instructionTable[0x28] = {&CPU::PLP, &CPU::Implied};
        instructionTable[0x58] = {&CPU::CLI, &CPU::Implied};
        instructionTable[0x78] = {&CPU::SEI, &CPU::Implied};
    }

    // --------------------------------------  Instructions

    // Ethan's instructions

    //Jump instructions

    // Jump to address
    void JMP(uint16_t address) {
        PC = address;
    }

    // Jump to subroutine
    void JSR(uint16_t address) {
        PC = PC + 2;
        stack_push16(PC);
        PC = address;
    }

    // Return from subroutine
    void RTS(uint16_t address) {
        uint8_t hi = stack_pop();
        uint8_t lo = stack_pop();

        PC = (hi << 8) | lo;
        PC ++;
    }

    // Break(software IRQ)
    void BRK(uint16_t address) {
        PC++;
        PC++;
        stack_push16(PC);

        setFlag(B, true);
        stack_push(P);

        setFlag(I, true);
        setFlag(B, false);

        const uint16_t read_address = 0xFFFE;
        uint16_t lo = readMemory(read_address);
        uint16_t hi = readMemory(read_address + 1);
        PC = (hi << 8) | lo;
    }

    // Return from Interrupt
    void RTI(uint16_t address) {
        // Pop stack and set to flags
        uint8_t flags = stack_pop();
        P = flags;
        setFlag(B, false);
        setFlag(U, true);

        // Pop stack twice and set to PC
        uint8_t hi = stack_pop();
        uint8_t lo = stack_pop();
        PC = (hi << 8) | lo;
    }

    // Stack instructions

    // Push A register to stack
    void PHA(uint16_t address) {
        stack_push(A);
    }

    // Pop stack into A register
    void PLA(uint16_t address) {
        A = stack_pop();
        setFlag(Z, A == 0);
        setFlag(N, A & 0x80);
    }

    // Push status flags to stack
    void PHP(uint16_t address) {
        setFlag(B, true);
        setFlag(U, true);
        stack_push(P);
        setFlag(B, false);
    }

    // Pop status flags
    void PLP(uint16_t address) {
        P = stack_pop();
        setFlag(U, true);
        setFlag(B, false);
    }

    // Flag instructions

    // Clear Interrupt Flag
    void CLI(uint16_t address) {
        setFlag(I, false);
    }

    // Set Interrupt Flag
    void SEI(uint16_t address) {
        setFlag(I, true);
    }

    // Load value into A
    // Still needs to update flags!!! Barebones for testing addressing mode only
    void LDA(uint16_t address) {
      uint8_t value = readMemory(address);
      A = value;
    };


    // --------------------------------------  Addressing Modes
    // No address, return the next PC
    uint16_t Immediate() {
      return PC++;
    };

    // Return address from zero page memory
    uint16_t ZeroPage() {
      return readMemory(PC++);
    }

    // Reuturn address + X from zero page memory, wrapped
    uint16_t ZeroPageX() {
      return readMemory(PC++) + X & 0xFF;
    }

    // Reuturn address + X from zero page memory, wrapped
    uint16_t ZeroPageY() {
      return readMemory(PC++) + Y & 0xFF;
    }

    // Return a full 16 bit address from the next two PC
    uint16_t Absolute() {
      uint16_t addr = readMemory(PC) | readMemory(PC + 1) << 8;
      PC += 2;
      return addr;
    }

    // Return a full 16 bit address from the next two PC + X
    uint16_t AbsoluteX() {
      uint16_t addr = readMemory(PC) | readMemory(PC + 1) << 8;
      PC += 2;
      return addr + X;
    }

    // Return a full 16 bit address from the next two PC + Y
    uint16_t AbsoluteY() {
      uint16_t addr = readMemory(PC) | readMemory(PC + 1) << 8;
      PC += 2;
      return addr + Y;
    }

    // Return a full 16 bit address from a pointer in the zero page + X
    uint16_t IndirectX() {
      uint16_t ptrAddr = (readMemory(PC++) + X) & 0xFF;
      uint16_t addr = readMemory(ptrAddr) | (readMemory(ptrAddr + 1) & 0xFF) << 8;
      return addr;
    }

    // Return a full 16 bit address from a pointer in the zero page + Y
    uint16_t IndirectY() {
      uint16_t ptrAddr = readMemory(PC++);
      uint16_t addr = readMemory(ptrAddr) | (readMemory(ptrAddr + 1) & 0xFF) << 8;
      return addr + Y;
    }

    // Special Indirect mode for JMP
    uint16_t IndirectJMP() {
        uint16_t lo = readMemory(PC);
        PC ++;
        uint16_t hi = readMemory(PC);
        PC ++;

        uint16_t addr = (hi << 8) | lo;

        lo = readMemory(addr);
        hi = readMemory(addr + 1);

        addr = (hi << 8) | lo;
        return addr;
    }

    uint16_t Implied() {
        return 0;
    }

    // Constructor
    CPU() {
      initInstructionTable();
    }

	// Push to the stack (8 bits)
    void stack_push(uint8_t value) {
        S -= 1;
        uint16_t stack_address = 0x0100 + S;
        writeMemory(stack_address, value);
    }

    // Push to the stack (16 bits)
    void stack_push16(uint16_t value) {
        uint8_t low_byte = value & 0xFF;
        uint8_t high_byte = value >> 8;
        S -= 1;
        uint16_t stack_address = 0x0100 + S;
        writeMemory(stack_address, low_byte);
        stack_address -= 1;
        S -= 1;
        writeMemory(stack_address, high_byte);
    }

    // Pop from the stack
    uint8_t stack_pop() {
        uint16_t stack_address = 0x0100 + S;
        uint8_t stack_top_value = readMemory(stack_address);
        S += 1;
        return stack_top_value;
    }

    // CPU Handling of an NMI Interrupt
    void nmi_interrupt() {
        stack_push16(PC);
        stack_push(P);
        setFlag(FLAGS::I, 1);
        PC = 0xFFFA;
    }

    // CPU Handling of an IRQ Interrupt
    void irq_interrupt() {
        // Check if interrupt is allowed
        if (getFlag(I) == 0) {
            // Push PC and P to stack
            stack_push16(PC);
            setFlag(I, true);
            setFlag(B, false);
            stack_push(P);
            // Get new PC location
            const uint16_t read_address = 0xFFFE;
            uint16_t lo = readMemory(read_address);
            uint16_t hi = readMemory(read_address + 1);
            PC = (hi << 8) | lo;
        }
    }
};
#endif

