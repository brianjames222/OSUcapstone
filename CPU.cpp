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
    uint16_t PC = 0xFFFC;    // Program Counter, start at 0xFFFC;
    uint8_t P = FLAGS::I;        // Status Flags Register, start with I

    // RAM for CPU
    std::array<uint8_t, 2 * 1024> memory{};

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
      PC = 0xFFFC;
      S -= 3;
      setFlag(FLAGS::I, 1);
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
		stack_push16(PC);
		stack_push(P);
		setFlag(FLAGS::I, 1);
		PC = 0xFFFE;
	}

    // sample few opcodes
    void instruction(uint8_t opcode) {
	switch (opcode) {
		// Load A: Zero Page
		case 0xA5:
			A = memory[PC];
			break;
		// Store A: Zero Page
		case 0x85:
			memory[PC] = A;
			break;
		// Load X: Zero Page
		case 0xA6:
			X = memory[PC];
			break;
		// Store X: Zero Page
		case 0x86:
			memory[PC] = X;
			break;
		// Load Y: Zero Page
		case 0xA4:
			Y = memory[PC];
			break;
		// Store Y: Zero Page
		case 0x84:
			memory[PC] = Y;
			break;
		// Transfer A to X
		case 0xAA:
			X = A;
			break;
		// Transfer X to A
		case 0x8A:
			A = X;
			break;
		// Transfer A to Y
		case 0xA8:
			Y = A;
			break;
		// Transfer Y to A
		case 0x98:
			A = Y;
			break;
		// Increment: Zero Page
		case 0xE6:
			memory[PC]++;
			break;
		// Decrement: Zero Page
		case 0xC6:
			memory[PC]--;
			break;
		// Increment X
		case 0xE8:
			X++;
			break;
		// Decrement X
		case 0xCA:
			X--;
			break;
		// Increment Y
		case 0xC8:
			Y++;
			break;
		// Decrement Y
		case 0x88:
			Y--;
			break;
		default:
			std::cerr << "Error: Instruction not valid: " << opcode << '\n';
	}
    }
};

