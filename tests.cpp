#include "CPU.cpp"
#include <cassert>

class Tests {
public:
	
	void test_cpu() {
		std::cout << "\nCPU Tests:\n";
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
	}
	
//----------------------------------------------------------------------------------------------------------------------------
	void test_opcodes() {
		std::cout << "---------------------------\nOpcode Tests:\n\n";
		CPU cpu;

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
	}

//----------------------------------------------------------------------------------------------------------------------------
	void test_stack() {
		CPU cpu;

		uint16_t starting_stack_address = 0x0100 + cpu.S;
		cpu.stack_push(0xBC);
		uint16_t current_stack_address = 0x0100 + cpu.S;
		assert(cpu.memory[current_stack_address] == 0xBC);
		uint8_t stack_top = cpu.stack_pop();
		assert(stack_top == 0xBC);
		current_stack_address = 0x0100 + cpu.S;
		assert(current_stack_address == starting_stack_address);
		cpu.stack_push16(0xABCD);
		current_stack_address = 0x0100 + cpu.S;
		assert(cpu.memory[current_stack_address] == 0xAB);
		assert(cpu.memory[current_stack_address + 1] == 0xCD);
		stack_top = cpu.stack_pop();
		assert(stack_top == 0xAB);
		stack_top = cpu.stack_pop();
		assert(stack_top == 0xCD);
		current_stack_address = 0x0100 + cpu.S;
		assert(current_stack_address == starting_stack_address);

		std::cout << "---------------------------\nStack function tests passed!\n";
	}

//----------------------------------------------------------------------------------------------------------------------------
	void test_reset() {
		CPU cpu;

		std::cout << "---------------------------\nReset test:\n\nCurrent values:\n";
		cpu.printRegisters();
		cpu.setFlag(CPU::FLAGS::Z, 1);
		cpu.setFlag(CPU::FLAGS::V, 1);
		cpu.setFlag(CPU::FLAGS::I, 0);
		cpu.PC = 0x0000;
		cpu.S = 0xAA;
		std::cout << "\nUpdated values:\n";
		cpu.printRegisters();

		// Populate fixed memory address
		cpu.writeMemory(0xFFFC, 0xA9);
		cpu.writeMemory(0xFFFD, 0xC2);

		// Reset CPU state
		cpu.reset();
		std::cout << "\nAfter reset:\n";
		cpu.printRegisters();

		//Check if values match reset
		assert(cpu.P == 0x24);
		assert(cpu.S == 0xFD);
		assert(cpu.PC == 0xC2A9);

		std::cout << "---------------------------\nReset function tests passed!\n";
	}

//----------------------------------------------------------------------------------------------------------------------------
	void test_nmi() {
		CPU cpu;

		uint8_t starting_stack_address = 0x0100 + cpu.S;
		cpu.nmi_interrupt();
		uint8_t current_stack_address = 0x0100 + cpu.S;
		assert(current_stack_address == starting_stack_address - 3);
		assert(cpu.PC == 0xFFFA);
		
		std::cout << "---------------------------\nNMI Interrupt function tests passed!\n";
	}
	
//----------------------------------------------------------------------------------------------------------------------------
	void test_irq() {
		CPU cpu;

		// Set flag so interrupt will work
		cpu.setFlag(CPU::FLAGS::I, false);

		// Call interrupt and get stack address
		uint16_t starting_stack_address = 0x0100 + cpu.S;
	    cpu.irq_interrupt();
		uint16_t current_stack_address = 0x0100 + cpu.S;

		assert(current_stack_address == starting_stack_address - 3);

		// Check if PC address is being set correctly
		uint8_t read_address = 0xFFFE;
		cpu.writeMemory(read_address, 0x12);
		cpu.writeMemory(read_address + 1, 0x34);

		uint16_t lo = cpu.readMemory(read_address);
		uint16_t hi = cpu.readMemory(read_address + 1);

		cpu.PC = (hi << 8) | lo;

		assert(cpu.PC == 0x3412);

		std::cout << "---------------------------\nIRQ Interrupt function tests passed!\n";
	}
};
