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

    void initInstructionTable(){
        for (int i = 0; i < 256; i++) {
          instructionTable[i] = {nullptr, nullptr};
        };

          // Brian's Instruction Table START ---------------------------- //
        // LDA
        instructionTable[0xA9] = {&CPU::LDA, &CPU::Immediate};
        instructionTable[0xA5] = {&CPU::LDA, &CPU::ZeroPage};
        instructionTable[0xB5] = {&CPU::LDA, &CPU::ZeroPageX};
        instructionTable[0xAD] = {&CPU::LDA, &CPU::Absolute};
        instructionTable[0xBD] = {&CPU::LDA, &CPU::AbsoluteX};
        instructionTable[0xB9] = {&CPU::LDA, &CPU::AbsoluteY};
        instructionTable[0xA1] = {&CPU::LDA, &CPU::IndirectX};
        instructionTable[0xB1] = {&CPU::LDA, &CPU::IndirectY};

        // LDX
        instructionTable[0xA2] = {&CPU::LDX, &CPU::Immediate};
        instructionTable[0xA6] = {&CPU::LDX, &CPU::ZeroPage};
        instructionTable[0xB6] = {&CPU::LDX, &CPU::ZeroPageY};
        instructionTable[0xAE] = {&CPU::LDX, &CPU::Absolute};
        instructionTable[0xBE] = {&CPU::LDX, &CPU::AbsoluteY};

        // LDY
        instructionTable[0xA0] = {&CPU::LDY, &CPU::Immediate};
        instructionTable[0xA4] = {&CPU::LDY, &CPU::ZeroPage};
        instructionTable[0xB4] = {&CPU::LDY, &CPU::ZeroPageX};
        instructionTable[0xAC] = {&CPU::LDY, &CPU::Absolute};
        instructionTable[0xBC] = {&CPU::LDY, &CPU::AbsoluteX};

        // STA
        instructionTable[0x85] = {&CPU::STA, &CPU::ZeroPage};
        instructionTable[0x95] = {&CPU::STA, &CPU::ZeroPageX};
        instructionTable[0x8D] = {&CPU::STA, &CPU::Absolute};
        instructionTable[0x9D] = {&CPU::STA, &CPU::AbsoluteX};
        instructionTable[0x99] = {&CPU::STA, &CPU::AbsoluteY};
        instructionTable[0x81] = {&CPU::STA, &CPU::IndirectX};
        instructionTable[0x91] = {&CPU::STA, &CPU::IndirectY};

        // STX
        instructionTable[0x86] = {&CPU::STX, &CPU::ZeroPage};
        instructionTable[0x96] = {&CPU::STX, &CPU::ZeroPageY};
        instructionTable[0x8E] = {&CPU::STX, &CPU::Absolute};

        // STY
        instructionTable[0x84] = {&CPU::STY, &CPU::ZeroPage};
        instructionTable[0x94] = {&CPU::STY, &CPU::ZeroPageX};
        instructionTable[0x8C] = {&CPU::STY, &CPU::Absolute};

        // TAX, TAY, TSX, TXA, TXS, TYA
        instructionTable[0xAA] = {&CPU::TAX, &CPU::Implied};
        instructionTable[0xA8] = {&CPU::TAY, &CPU::Implied};
        instructionTable[0xBA] = {&CPU::TSX, &CPU::Implied};
        instructionTable[0x8A] = {&CPU::TXA, &CPU::Implied};
        instructionTable[0x9A] = {&CPU::TXS, &CPU::Implied};
        instructionTable[0x98] = {&CPU::TYA, &CPU::Implied};

        // Brian Instruction Table END -------------------------------- //


        // Add instructions
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
        instructionTable[0xF0] = {&CPU::BEQ, &CPU::Relative};
        instructionTable[0xD0] = {&CPU::BNE, &CPU::Relative};
        instructionTable[0x90] = {&CPU::BCC, &CPU::Relative};
        instructionTable[0xB0] = {&CPU::BCS, &CPU::Relative};
        instructionTable[0x30] = {&CPU::BMI, &CPU::Relative};
        instructionTable[0x10] = {&CPU::BPL, &CPU::Relative};
        instructionTable[0x50] = {&CPU::BVC, &CPU::Relative};
        instructionTable[0x70] = {&CPU::BVS, &CPU::Relative};
        instructionTable[0x18] = {&CPU::CLC, &CPU::Implicit};
        instructionTable[0x38] = {&CPU::SEC, &CPU::Implicit};
        instructionTable[0x0A] = {&CPU::ASL, &CPU::Accumulator};
        instructionTable[0x06] = {&CPU::ASL, &CPU::ZeroPage};
        instructionTable[0x16] = {&CPU::ASL, &CPU::ZeroPageX};
        instructionTable[0x0E] = {&CPU::ASL, &CPU::Absolute};
        instructionTable[0x1E] = {&CPU::ASL, &CPU::AbsoluteX};
        instructionTable[0x4A] = {&CPU::LSR, &CPU::Accumulator};
        instructionTable[0x46] = {&CPU::LSR, &CPU::ZeroPage};
        instructionTable[0x56] = {&CPU::LSR, &CPU::ZeroPageX};
        instructionTable[0x4E] = {&CPU::LSR, &CPU::Absolute};
        instructionTable[0x5E] = {&CPU::LSR, &CPU::AbsoluteX};
        instructionTable[0x2A] = {&CPU::ROL, &CPU::Accumulator};
        instructionTable[0x26] = {&CPU::ROL, &CPU::ZeroPage};
        instructionTable[0x36] = {&CPU::ROL, &CPU::ZeroPageX};
        instructionTable[0x2E] = {&CPU::ROL, &CPU::Absolute};
        instructionTable[0x3E] = {&CPU::ROL, &CPU::AbsoluteX};
        instructionTable[0x6A] = {&CPU::ROR, &CPU::Accumulator};
        instructionTable[0x66] = {&CPU::ROR, &CPU::ZeroPage};
        instructionTable[0x76] = {&CPU::ROR, &CPU::ZeroPageX};
        instructionTable[0x6E] = {&CPU::ROR, &CPU::Absolute};
        instructionTable[0x7E] = {&CPU::ROR, &CPU::AbsoluteX};
        instructionTable[0xC9] = {&CPU::CMP, &CPU::Immediate};
        instructionTable[0xC5] = {&CPU::CMP, &CPU::ZeroPage};
        instructionTable[0xD5] = {&CPU::CMP, &CPU::ZeroPageX};
        instructionTable[0xCD] = {&CPU::CMP, &CPU::Absolute};
        instructionTable[0xDD] = {&CPU::CMP, &CPU::AbsoluteX};
        instructionTable[0xD9] = {&CPU::CMP, &CPU::AbsoluteY};
        instructionTable[0xC1] = {&CPU::CMP, &CPU::IndirectX};
        instructionTable[0xD1] = {&CPU::CMP, &CPU::IndirectY};
        instructionTable[0xE0] = {&CPU::CPX, &CPU::Immediate};
        instructionTable[0xE4] = {&CPU::CPX, &CPU::ZeroPage};
        instructionTable[0xEC] = {&CPU::CPX, &CPU::Absolute};
        instructionTable[0xC0] = {&CPU::CPY, &CPU::Immediate};
        instructionTable[0xC4] = {&CPU::CPY, &CPU::ZeroPage};
        instructionTable[0xCC] = {&CPU::CPY, &CPU::Absolute};
        instructionTable[0xEA] = {&CPU::NOP, &CPU::Implicit};
        instructionTable[0xD8] = {&CPU::CLD, &CPU::Implicit};
        instructionTable[0xF8] = {&CPU::SED, &CPU::Implicit};
        instructionTable[0xB8] = {&CPU::CLV, &CPU::Implicit};
        instructionTable[0x69] = {&CPU::ADC, &CPU::Immediate};
        instructionTable[0x65] = {&CPU::ADC, &CPU::ZeroPage};
        instructionTable[0x75] = {&CPU::ADC, &CPU::ZeroPageX};
        instructionTable[0x6D] = {&CPU::ADC, &CPU::Absolute};
        instructionTable[0x7D] = {&CPU::ADC, &CPU::AbsoluteX};
        instructionTable[0x79] = {&CPU::ADC, &CPU::AbsoluteY};
        instructionTable[0x61] = {&CPU::ADC, &CPU::IndirectX};
        instructionTable[0x71] = {&CPU::ADC, &CPU::IndirectY};

        // Unofficial Opcodes
        // SLO
        instructionTable[0x07] = {&CPU::SLO, &CPU::ZeroPage};
        instructionTable[0x17] = {&CPU::SLO, &CPU::ZeroPageX};
        instructionTable[0x03] = {&CPU::SLO, &CPU::IndirectX};
        instructionTable[0x13] = {&CPU::SLO, &CPU::IndirectY};
        instructionTable[0x0F] = {&CPU::SLO, &CPU::Absolute};
        instructionTable[0x1F] = {&CPU::SLO, &CPU::AbsoluteX};
        instructionTable[0x1B] = {&CPU::SLO, &CPU::AbsoluteY};

        // RLA
        instructionTable[0x27] = {&CPU::RLA, &CPU::ZeroPage};
        instructionTable[0x37] = {&CPU::RLA, &CPU::ZeroPageX};
        instructionTable[0x23] = {&CPU::RLA, &CPU::IndirectX};
        instructionTable[0x33] = {&CPU::RLA, &CPU::IndirectY};
        instructionTable[0x2F] = {&CPU::RLA, &CPU::Absolute};
        instructionTable[0x3F] = {&CPU::RLA, &CPU::AbsoluteX};
        instructionTable[0x3B] = {&CPU::RLA, &CPU::AbsoluteY};

        // SRE
        instructionTable[0x47] = {&CPU::SRE, &CPU::ZeroPage};
        instructionTable[0x57] = {&CPU::SRE, &CPU::ZeroPageX};
        instructionTable[0x43] = {&CPU::SRE, &CPU::IndirectX};
        instructionTable[0x53] = {&CPU::SRE, &CPU::IndirectY};
        instructionTable[0x4F] = {&CPU::SRE, &CPU::Absolute};
        instructionTable[0x5F] = {&CPU::SRE, &CPU::AbsoluteX};
        instructionTable[0x5B] = {&CPU::SRE, &CPU::AbsoluteY};

        // RRA
        instructionTable[0x67] = {&CPU::RRA, &CPU::ZeroPage};
        instructionTable[0x77] = {&CPU::RRA, &CPU::ZeroPageX};
        instructionTable[0x63] = {&CPU::RRA, &CPU::IndirectX};
        instructionTable[0x73] = {&CPU::RRA, &CPU::IndirectY};
        instructionTable[0x6F] = {&CPU::RRA, &CPU::Absolute};
        instructionTable[0x7F] = {&CPU::RRA, &CPU::AbsoluteX};
        instructionTable[0x7B] = {&CPU::RRA, &CPU::AbsoluteY};
    }

    // --------------------------------------  Instructions
    // BRIAN INSTRUCTIONS BEGIN ------------------------------------- //
    // Helper function to update Z and N flags
    void updateZeroNegativeFlags(uint8_t value) {
      setFlag(FLAGS::Z, value == 0);
      setFlag(FLAGS::N, value & 0x80);
    }

    // Access Instructions
    // "LDA loads a memory value into the accumulator."
    void LDA(uint16_t address) {
      uint8_t value = readMemory(address);
      A = value;

      updateZeroNegativeFlags(A);
    }

    // "LDX loads a memory value into the X register."
    void LDX(uint16_t address) {
      uint8_t value = readMemory(address);
      X = value;

      updateZeroNegativeFlags(X);
    }

    // "LDY loads a memory value into the Y register."
    void LDY(uint16_t address) {
      uint8_t value = readMemory(address);
      Y = value;

      updateZeroNegativeFlags(Y);
    }

    // "STA stores the accumulator value into memory."
    void STA(uint16_t address) {
      writeMemory(address, A);
    }

    // "STX stores the X register value into memory."
    void STX(uint16_t address) {
      writeMemory(address, X);
    }

    // "STY stores the Y register value into memory. "
    void STY(uint16_t address) {
      writeMemory(address, Y);
    } 

    // Transfer Instructions
    // "TAX copies the accumulator value to the X register."
    void TAX(uint16_t) {
      X = A;

      updateZeroNegativeFlags(X);
    }

    // "TAY copies the accumulator value to the Y register."
    void TAY(uint16_t) {
      Y = A;

      updateZeroNegativeFlags(Y);
    } 

    // "TSX copies the stack pointer value to the X register."
    void TSX(uint16_t) {
      X = S;

      updateZeroNegativeFlags(X);
    }

    // "TXA copies the X register value to the accumulator."
    void TXA(uint16_t) {
      A = X;

      updateZeroNegativeFlags(A);
    }  

    // "TXS copies the X register value to the stack pointer."
    void TXS(uint16_t) {
      S = X;
    }

    // "TYA copies the Y register value to the accumulator."
    void TYA(uint16_t) {
      A = Y;

      updateZeroNegativeFlags(A);
    } 

    // Justyn's Instructions
    // Arithmetic Instructions

    // Add carry flag and value to A
    void ADC(uint16_t address) {
      uint8_t value = readMemory(address);
      uint16_t result = A + value + getFlag(CPU::FLAGS::C);

      // Set C flag if overflow
      setFlag(CPU::FLAGS::C, result > 0xFF);

      // Set Z flag if zero
      setFlag(CPU::FLAGS::Z, result == 0);

      // Set V flag if signed overflow
      uint8_t trunc_result = result & 0xFF;
      if ((trunc_result ^ A) & (trunc_result ^ value) & 0x80) {
        setFlag(CPU::FLAGS::V, true);
      } else {
        setFlag(CPU::FLAGS::V, false);
      }

      // Set N flag if negative
      setFlag(CPU::FLAGS::N, trunc_result & 0x80);

      // Update A
      A = trunc_result;
    }

    // Subtract value from A with carry flag
    void SBC(uint16_t address) {
      uint8_t value = readMemory(address);
      uint16_t result = A - value + ~getFlag(CPU::FLAGS::C);

      // Set C flag if overflow
      setFlag(CPU::FLAGS::C, result & 0x100);

      // Set Z flag if zero
      setFlag(CPU::FLAGS::Z, result == 0);

      // Set V flag if signed overflow
      uint8_t trunc_result = result & 0xFF;
      if ((trunc_result ^ A) & (trunc_result ^ ~value) & 0x80) {
        setFlag(CPU::FLAGS::V, true);
      } else {
        setFlag(CPU::FLAGS::V, false);
      }

      // Set N flag if negative
      setFlag(CPU::FLAGS::N, trunc_result & 0x80);

      // Update A
      A = trunc_result;
    }

    // Temporary ORA
    // DELETE WHEN MERGING ETHAN'S BRANCH
    void ORA(uint16_t address) {
    }

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




    // Carter's instructions--------------------------------------------------------

    // Branch Instructions (8 count)
	// these are signed, hence int8_t instead of uint8_t

	// branch if Zero flag is set
	void BEQ(uint16_t address) {
		if (getFlag(Z)) {
			int8_t value = readMemory(address);
			PC = PC + 2 + value;
		}
	}

	// branch if Zero flag is not set
	void BNE(uint16_t address) {
		if (!getFlag(Z)) {
			int8_t value = readMemory(address);
			PC = PC + 2 + value;
		}
	}

	// branch if Carry flag is set
	void BCS(uint16_t address) {
		if (getFlag(C)) {
			int8_t value = readMemory(address);
			PC = PC + 2 + value;
		}
	}
	
	// branch if Carry flag is not set
	void BCC(uint16_t address) {
		if (!getFlag(C)) {
			int8_t value = readMemory(address);
			PC = PC + 2 + value;
		}
	}

	// branch if Negative flag is set (Minus)
	void BMI(uint16_t address) {
		if (getFlag(N)) {
			int8_t value = readMemory(address);
			PC = PC + 2 + value;
		}
	}

	// branch if Negative flag is not set (Plus)
	void BPL(uint16_t address) {
		if (!getFlag(N)) {
			int8_t value = readMemory(address);
			PC = PC + 2 + value;
		}
	}

	// branch if oVerflow flag is set
	void BVS(uint16_t address) {
		if (getFlag(V)) {
			int8_t value = readMemory(address);
			PC = PC + 2 + value;
		}
	}
	
	// branch if oVerflow flag is not set
	void BVC(uint16_t address) {
		if (!getFlag(V)) {
			int8_t value = readMemory(address);
			PC = PC + 2 + value;
		}
	}

	// Carry Flag Instructions (2 count)
	
	//set the carry flag
	void SEC(uint16_t address) {
		setFlag(C, true);
	}
	
	// clear the carry flag
	void CLC(uint16_t address) {
		setFlag(C, false);
	}

    // Zachary's Instructions

    // Shift Instructions

    // Arithmetic Shift Left
    void ASL(uint16_t address) {
      uint8_t value;
      // Checking for accumulator mode
      if (address == 0xFFFF) {
        value = A;
      } else {
        value = readMemory(address);
      }
      // MSB = Most Significant Bit
      int value_msb = (value >> 7) & 1;
      uint8_t shifted_value = value << 1;
      int shifted_value_msb = (shifted_value >> 7) & 1;
      // C, N, Z flags are affected
      setFlag(C, value_msb);
      setFlag(N, shifted_value_msb);
      setFlag(Z, shifted_value == 0);
      if (address == 0xFFFF) {
        A = shifted_value;
      } else {
        writeMemory(address, value);
        writeMemory(address, shifted_value);
      }
    }

    // Logical Shift Right
    void LSR(uint16_t address) {
      uint8_t value;
      if (address == 0xFFFF) {
        value = A;
      } else {
        value = readMemory(address); 
      }
      // LSB = Least Significant Bit
      int value_lsb = value & 1;
      uint8_t shifted_value = value >> 1;
      int shifted_value_msb = (shifted_value >> 7) & 1;
      setFlag(C, value_lsb);
      setFlag(N, shifted_value_msb);
      setFlag(Z, shifted_value == 0);
      if (address == 0xFFFF) {
        A = shifted_value;
      } else {
        writeMemory(address, value);
        writeMemory(address, shifted_value);
      }
    }

    // Rotate Left
    void ROL(uint16_t address) {
      uint8_t value;
      if (address == 0xFFFF) {
        value = A;
      } else {
        value = readMemory(address);
      }
      int value_msb = (value >> 7) & 1;
      uint8_t shifted_value = value << 1;
      int shifted_value_msb = (shifted_value >> 7) & 1;
      // The value held in the Carry flag is shifted into the LSB of the new value
      if (getFlag(C) == 1) {
        shifted_value |= 1;
      }
      setFlag(C, value_msb);
      setFlag(N, shifted_value_msb);
      setFlag(Z, shifted_value == 0);
      if (address == 0xFFFF) {
        A = shifted_value;
      } else {
        writeMemory(address, value);
        writeMemory(address, shifted_value);
      }
    }

    // Rotate Right
    void ROR(uint16_t address) {
      uint8_t value;
      if (address == 0xFFFF) {
        value = A;
      } else {
        value = readMemory(address);
      }
      int value_lsb = value & 1;
      uint8_t shifted_value = value >> 1;
      int shifted_value_msb = (shifted_value >> 7) & 1;
      // The value held in the Carry flag is shifted into the MSB of the new value
      if (getFlag(C) == 1) {
        shifted_value |= 0x80;
      }
      setFlag(C, value_lsb);
      setFlag(N, shifted_value_msb);
      setFlag(Z, shifted_value == 0);
      if (address == 0xFFFF) {
        A = shifted_value;
      } else {
        writeMemory(address, value);
        writeMemory(address, shifted_value);
      }
    }

    // Compare Instructions

    // Compare to Accumulator
    void CMP(uint16_t address) {
      uint8_t value = readMemory(address);
      uint8_t result = A - value;
      int result_msb = (result >> 7) & 1;
      setFlag(C, A >= value);
      setFlag(N, result_msb);
      setFlag(Z, A == value);
    }

    // Compare to X Register
    void CPX(uint16_t address) {
      uint8_t value = readMemory(address);
      uint8_t result = X - value;
      int result_msb = (result >> 7) & 1;
      setFlag(C, X >= value);
      setFlag(N, result_msb);
      setFlag(Z, X == value);
    }

    // Compare to Y Register
    void CPY(uint16_t address) {
      uint8_t value = readMemory(address);
      uint8_t result = Y - value;
      int result_msb = (result >> 7) & 1;
      setFlag(C, Y >= value);
      setFlag(N, result_msb);
      setFlag(Z, Y == value);
    }

    // No Operation
    void NOP(uint16_t address) {
      return;
    }

    // Flag Instructions

    // Clear Decimal Flag
    void CLD(uint16_t address) {
      setFlag(D, 0);
    }

    // Set Decimal Flag
    void SED(uint16_t address) {
      setFlag(D, 1);
    }

    // Clear Overflow Flag
    void CLV(uint16_t address) {
      setFlag(V, 0);
    }

    // --------------------------------------  Unofficial Opcodes
    // Shift Left and Or
    void SLO(uint16_t address) {
      ASL(address);
      ORA(address);
    }

    // Rotate Left and And
    void RLA(uint16_t address) {
      uint8_t value = readMemory(address);

      // Rotate left
      bool carry = getFlag(C);
      setFlag(C, value & 0x80);
      value = (value << 1) | carry;
      writeMemory(address, value);

      // And
      A &= value;

      // Set N Flag
      setFlag(N, A & 0x80);

      // Set Z Flag
      setFlag(Z, A == 0);
    }

    // Shift Right and Exclusive Or
    void SRE(uint16_t address) {
      uint8_t value = readMemory(address);

      // Shift Right
      setFlag(C, value & 0x01);
      value >>= 1;
      writeMemory(address, value);

      // Eor
      A ^= value;

      // Set N flag
      setFlag(N, A & 0x80);

      // Set Z flag
      setFlag(Z, A == 0);
    }

    // Rotate Right and Add With Carry
    void RRA(uint16_t address) {
      uint8_t value = readMemory(address);

      // Rotate Right
      bool carry = getFlag(C);
      setFlag(C, value & 0x01);
      value = (value >> 1) | (carry << 7);
      writeMemory(address, value);

      // Add with Carry (using new carry value)
      uint16_t result = A + value + getFlag(C);
      uint8_t trunc_result = result & 0xFF;

      // Set N Flag
      setFlag(N, trunc_result & 0x80);

      // Set V Flag
      if ((trunc_result ^ A) & (trunc_result ^ value) & 0x80) {
        setFlag(CPU::FLAGS::V, true);
      } else {
        setFlag(CPU::FLAGS::V, false);
      }

      // Set Z Flag
      setFlag(Z, trunc_result == 0);

      // Set C Flag
      setFlag(C, result > 0xFF);

      // Set A
      A = trunc_result;
    }

    // --------------------------------------  Addressing Modes
    // Address is implied, returning 0xFFFF as indicator
    uint16_t Implicit() {
      return 0xFFFF;
    }

    // Address is directly at the next PC
    uint16_t Immediate() {
      return PC++;
    }

    // Address mode
    // Implied addressing does nothing, no memory access needed (dummy value)
    uint16_t Implied() {
      return 0;
    }


    // Address is the accumulator, returning 0xFFFF as indicator
    // Logic to be handled in instruction
    uint16_t Accumulator() {
      return 0xFFFF;
    }

    // Return next PC += offset, stored in PC
    uint16_t Relative() {
      // Offset is unsigned, at the memory location stored in PC
      int8_t offset = static_cast<int8_t>(readMemory(PC++));
      return PC + offset;
    }

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

    // Return an address using the operand as a pointer
    uint16_t Indirect() {
      // Find 16 bit address from operand
      uint16_t pointer = readMemory(PC) | readMemory(PC + 1) << 8;
      // Find address referenced by pointer
      uint16_t addr = readMemory(pointer) | readMemory((pointer + 1) & 0xFFFF) << 8;
      PC += 2;
      return addr;
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
            setFlag(B, false);
            stack_push(P);
            setFlag(I, true);
            // Get new PC location
            const uint16_t read_address = 0xFFFE;
            uint16_t lo = readMemory(read_address);
            uint16_t hi = readMemory(read_address + 1);
            PC = (hi << 8) | lo;
        }
    }
};
#endif

