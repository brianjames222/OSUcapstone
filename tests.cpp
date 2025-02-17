#include <cassert>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <iostream>
#include <fstream>

#include "NES.cpp"
#include "Bus.h"

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
	void test_ADC() {
		std::cout << "---------------------------\nADC Tests:\n\n";
    CPU cpu;

    // Test Program
    cpu.writeMemory(0x00, 0x69); // Load 5
    cpu.writeMemory(0x01, 0x05);
    cpu.writeMemory(0x02, 0x69); // Load 0
    cpu.writeMemory(0x03, 0x00);
    cpu.writeMemory(0x04, 0x69); // Load 80
    cpu.writeMemory(0x05, 0x50);
    cpu.writeMemory(0x06, 0x69); // Load -10, signed
    cpu.writeMemory(0x07, 0xF6);

    // Test A Register
    cpu.PC = 0x00;
    cpu.A = 0x05;
    cpu.execute();
    assert(cpu.A == 0x0A);
    std::cout << "   A Register good\n";

    // Test Carry Flag
    cpu.PC = 0x00;
    cpu.A = 0x05;
    cpu.setFlag(CPU::FLAGS::C, true);
    cpu.execute();
    assert(cpu.A == 0x0B);
    std::cout << "   Carry flag modifier good\n";

    // Test Carry Flag Value
    cpu.PC = 0x00;
    cpu.A = 0x05;
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::C) == false);
    cpu.PC = 0x00;
    cpu.A = 0xFF;
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::C) == true);
    std::cout << "   Carry flag result good\n";

    // Test Zero Flag Value
    cpu.PC = 0x00;
    cpu.A = 0x05;
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::Z) == false);
    cpu.A = 0x00;
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::Z) == true);
    std::cout << "   Zero flag good\n";

    // Test Overflow Flag Value
    cpu.PC = 0x00;
    cpu.A = 0x05;
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::V) == false);
    cpu.PC = 0x04;
    cpu.A = 0x50;
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::V) == true);
    std::cout << "   Overflow flag good\n";

    // Test Negative Flag Value
    cpu.PC = 0x00;
    cpu.A = 0x05;
    cpu.execute();
      assert(cpu.getFlag(CPU::FLAGS::N) == false);
    cpu.PC = 0x06;
    cpu.A = 0x05;
    cpu.execute();
    std::cout << "   Negative flag good\n";

    std::cout << "\nADC Tests passed!\n\n";
}

//----------------------------------------------------------------------------------------------------------------------------
	void test_stack() {
		CPU cpu;

		uint16_t starting_stack_address = 0x0100 + cpu.S;
		cpu.stack_push(0xBC);
		uint16_t current_stack_address = 0x0100 + cpu.S;
		assert(cpu.memory[current_stack_address + 1] == 0xBC);
		uint8_t stack_top = cpu.stack_pop();
		assert(stack_top == 0xBC);
		current_stack_address = 0x0100 + cpu.S;
		assert(current_stack_address == starting_stack_address);
		cpu.stack_push16(0xABCD);
		current_stack_address = 0x0100 + cpu.S;
		assert(cpu.memory[current_stack_address + 2] == 0xAB);
		assert(cpu.memory[current_stack_address + 1] == 0xCD);
		stack_top = cpu.stack_pop();
		assert(stack_top == 0xCD);
		stack_top = cpu.stack_pop();
		assert(stack_top == 0xAB);
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

		// Check if PC address is being set correctly
      uint16_t read_address = 0xFFFA;
      cpu.writeMemory(read_address, 0x12);
      cpu.writeMemory(read_address + 1, 0x34);

      uint8_t lo = cpu.readMemory(read_address);
      uint8_t hi = cpu.readMemory(read_address + 1);

      cpu.PC = (hi << 8) | lo;

      assert(cpu.PC == 0x3412);

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
		uint16_t read_address = 0xFFFE;
		cpu.writeMemory(read_address, 0x12);
		cpu.writeMemory(read_address + 1, 0x34);

		uint8_t lo = cpu.readMemory(read_address);
		uint8_t hi = cpu.readMemory(read_address + 1);

		cpu.PC = (hi << 8) | lo;

		assert(cpu.PC == 0x3412);

		std::cout << "---------------------------\nIRQ Interrupt function tests passed!\n";
	}

//----------------------------------------------------------------------------------------------------------------------------
	void test_jmp() {
		CPU cpu;
		cpu.reset();
		uint16_t test_memory = 0x0000;

		// Test JMP, JSR, RTS
		cpu.JMP(0xFFFA);
		assert(cpu.PC == 0xFFFA);

		cpu.JSR(0x1234);
		assert(cpu.PC == 0x1234);

		cpu.RTS(test_memory);
		assert(cpu.PC == 0xFFFA);

		// Test BRK, RTI
		cpu.PC = 0x1973;
		cpu.setFlag(CPU::FLAGS::Z, 1);
		cpu.setFlag(CPU::FLAGS::C, 1);
		cpu.setFlag(CPU::FLAGS::V, 1);

		cpu.BRK(test_memory);
		cpu.RTI(test_memory);

		assert(cpu.PC == 0x1975);
		assert(cpu.P == 0x67);

		// Test Indirect Jump

		cpu.PC = 0x0000;
		cpu.writeMemory(cpu.PC, 0x34);
		cpu.writeMemory(cpu.PC + 1, 0x12);

		cpu.writeMemory(0x1234, 0x78);
		cpu.writeMemory(0x1235, 0x56);

		cpu.JMP(cpu.IndirectJMP());

		assert(cpu.PC == 0x5678);

		std::cout << "---------------------------\nJump functions tests passed!\n";
	}

//----------------------------------------------------------------------------------------------------------------------------
	void test_stack_instructions() {
		CPU cpu;
		cpu.reset();
		uint16_t test_memory = 0x0000;
		cpu.A = 0x34;
		// Test PHA and PLA
		cpu.PHA(test_memory);
		cpu.PLA(test_memory);

		assert(cpu.A == 0x34);
		// Test PHP and PLP
		cpu.PHP(test_memory);
		cpu.PLP(test_memory);

		assert(cpu.P == 0x24);
	}

//----------------------------------------------------------------------------------------------------------------------------
	void test_branch() {
		CPU cpu;
		
		uint16_t test_memory = 0x0000;
		cpu.writeMemory(test_memory, 0x79);
		test_memory = cpu.Relative();
		
		// branch if Zero set
		cpu.setFlag(CPU::FLAGS::Z, 1);
		cpu.BEQ(test_memory);
		assert(cpu.PC == 0x7A);

		// branch if Zero clear
		cpu.PC = 0x0001;
		cpu.setFlag(CPU::FLAGS::Z, 0);
		cpu.BNE(test_memory);
		assert(cpu.PC == 0x7A);
		
		// branch if Carry set
		cpu.PC = 0x0001;
		cpu.setFlag(CPU::FLAGS::C, 1);
		cpu.BCS(test_memory);
		assert(cpu.PC == 0x7A);

		// branch if Carry clear
		cpu.PC = 0x0001;
		cpu.setFlag(CPU::FLAGS::C, 0);
		cpu.BCC(test_memory);
		assert(cpu.PC == 0x7A);
		
		// branch if Negative set
		cpu.PC = 0x0001;
		cpu.setFlag(CPU::FLAGS::N, 1);
		cpu.BMI(test_memory);
		assert(cpu.PC == 0x7A);

		// branch if Negative clear
		cpu.PC = 0x0001;
		cpu.setFlag(CPU::FLAGS::N, 0);
		cpu.BPL(test_memory);
		assert(cpu.PC == 0x7A);
		
		// branch if oVerflow set
		cpu.PC = 0x0001;
		cpu.setFlag(CPU::FLAGS::V, 1);
		cpu.BVS(test_memory);
		assert(cpu.PC == 0x7A);

		// branch if oVerflow clear
		cpu.PC = 0x0001;
		cpu.setFlag(CPU::FLAGS::V, 0);
		cpu.BVC(test_memory);
		assert(cpu.PC == 0x7A);	
		
		std::cout << "---------------------------\nBranch functions tests passed!\n";
	}

//----------------------------------------------------------------------------------------------------------------------------
  void test_ASL() {
    CPU cpu;
    cpu.reset();

    // Accumulator loaded with 25, ASL executed, accumulator should now hold 50
    cpu.A = 0x19;
    cpu.writeMemory(0x00, 0x0A); // ASL Accumulator
    cpu.execute();
    assert(cpu.A == 0x32);

    // Accumulator loaded with 144, ASL executed, accumulator should now hold 32 and carry flag should be set
    cpu.A = 0x90;
    cpu.writeMemory(0x01, 0x0A);
    cpu.execute();
    assert(cpu.A == 0x20);
    assert(cpu.getFlag(CPU::FLAGS::C) == 1);

    // ASL Non-Accumulator Testing. Address 0xABCD loaded with 25, ASL executed, address should now hold 50
    cpu.writeMemory(0x02, 0x0E); // ASL Absolute
    cpu.writeMemory(0x03, 0xCD);
    cpu.writeMemory(0x04, 0xAB);
    cpu.writeMemory(0xABCD, 0x19); // Load 0x19 into address 0xABCD
    cpu.execute();
    assert(cpu.readMemory(0xABCD) == 0x32);

    std::cout << "---------------------------\nASL Instruction tests passed!\n";
  }
//----------------------------------------------------------------------------------------------------------------------------
  void test_LSR() {
    CPU cpu;
    cpu.reset();

    // Accumulator loaded with 144, LSR executed, accumulator should now hold 72
    cpu.A = 0x90;
    cpu.writeMemory(0x00, 0x4A); // LSR Accumulator
    cpu.execute();
    assert(cpu.A == 0x48);
    assert(cpu.getFlag(CPU::FLAGS::C) == 0);

    // LSR Non-Accumulator Testing. Address 0xABCD loaded with 144, LSR executed, address should now hold 72
    cpu.writeMemory(0x01, 0x4E); // LSR Absolute
    cpu.writeMemory(0x02, 0xCD);
    cpu.writeMemory(0x03, 0xAB);
    cpu.writeMemory(0xABCD, 0x90); // Load 0x90 into address 0xABCD
    cpu.execute();
    assert(cpu.readMemory(0xABCD) == 0x48);

    std::cout << "---------------------------\nLSR Instruction tests passed!\n";
  }
//----------------------------------------------------------------------------------------------------------------------------
  void test_ROL() {
    CPU cpu;
    cpu.reset();

    // Accumulator loaded with 25, ROL executed, accumulator should now hold 50
    cpu.A = 0x19;
    cpu.writeMemory(0x00, 0x2A); // ROL Accumulator
    cpu.execute();
    assert(cpu.A == 0x32);

    // Accumulator loaded with 128, ROL executed, accumulator should now hold 0
    cpu.setFlag(CPU::FLAGS::C, 0); // Reset Carry Flag
    cpu.A = 0x80;
    cpu.writeMemory(0x01, 0x2A);
    cpu.execute();
    assert(cpu.A == 0x0);
    assert(cpu.getFlag(CPU::FLAGS::C) == 1); // Carry Flag should now hold 1

    // ROL Non-Accumulator Testing. Address 0xABCD loaded with 128, ROL executed, Carry Flag set, address should now hold 1
    cpu.setFlag(CPU::FLAGS::C, 1); // Set Carry Flag
    cpu.writeMemory(0x02, 0x2E); // ROL Absolute
    cpu.writeMemory(0x03, 0xCD);
    cpu.writeMemory(0x04, 0xAB);
    cpu.writeMemory(0xABCD, 0x80);
    cpu.execute();
    assert(cpu.readMemory(0xABCD) == 0x1);
    assert(cpu.getFlag(CPU::FLAGS::C) == 1); // Carry Flag should now hold 1

    std::cout << "---------------------------\nROL Instruction tests passed!\n";
  }
//----------------------------------------------------------------------------------------------------------------------------
  void test_ROR() {
    CPU cpu;
    cpu.reset();

    // Accumulator loaded with 1, ROR executed, accumulator should now hold 0
    cpu.A = 0x1;
    cpu.writeMemory(0x00, 0x6A); // ROR Accumulator
    cpu.execute();
    assert(cpu.A == 0x0);
    assert(cpu.getFlag(CPU::FLAGS::C) == 1);

    // Accumulator loaded with 25, ROR executed, Carry Flag not set, accumulator should now hold 12
    cpu.setFlag(CPU::FLAGS::C, 0); // Reset Carry Flag
    cpu.A = 0x19;
    cpu.writeMemory(0x01, 0x6A);
    cpu.execute();
    assert(cpu.A == 0xC);

    // ROR Non-Accumulator Testing. Address 0xABCD loaded with 1, ROR executed, Carry Flag set, address should now hold 128
    cpu.setFlag(CPU::FLAGS::C, 1); // Set Carry Flag
    cpu.writeMemory(0x02, 0x6E); // ROR Absolute
    cpu.writeMemory(0x03, 0xCD);
    cpu.writeMemory(0x04, 0xAB);
    cpu.writeMemory(0xABCD, 0x1); // Load 0x1 into address 0xABCD
    cpu.execute();
    assert(cpu.readMemory(0xABCD) == 0x80);
    assert(cpu.getFlag(CPU::FLAGS::C) == 1);

    std::cout << "---------------------------\nROR Instruction tests passed!\n";
  }
//----------------------------------------------------------------------------------------------------------------------------
  void test_CMP() {
    CPU cpu;
    cpu.reset();

    // Accumulator loaded with 144, address loaded with 80, CMP executed, Carry flag should be set
    cpu.A = 0x90;
    cpu.writeMemory(0x00, 0xCD); // CMP Absolute
    cpu.writeMemory(0x01, 0xCD);
    cpu.writeMemory(0x02, 0xAB);
    cpu.writeMemory(0xABCD, 0x50);
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::C) == 1);

    // Accumulator loaded with 80, address loaded with 144, CMP executed, Carry flag should not be set
    cpu.A = 0x50;
    cpu.writeMemory(0x03, 0xCD);
    cpu.writeMemory(0x04, 0xCD);
    cpu.writeMemory(0x05, 0xAB);
    cpu.writeMemory(0xABCD, 0x90);
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::C) == 0);

    std::cout << "---------------------------\nCMP Instruction tests passed!\n";
  }
//----------------------------------------------------------------------------------------------------------------------------
  void test_CPX() {
    CPU cpu;
    cpu.reset();

    // X register loaded with 144, address loaded with 80, CPX executed, Carry flag should be set
    cpu.X = 0x90;
    cpu.writeMemory(0x00, 0xEC); // CPX Absolute
    cpu.writeMemory(0x01, 0xCD);
    cpu.writeMemory(0x02, 0xAB);
    cpu.writeMemory(0xABCD, 0x50);
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::C) == 1);

    // X register loaded with 80, address loaded with 144, CPX executed, Carry flag should not be set
    cpu.X = 0x50;
    cpu.writeMemory(0x03, 0xEC);
    cpu.writeMemory(0x04, 0xCD);
    cpu.writeMemory(0x05, 0xAB);
    cpu.writeMemory(0xABCD, 0x90);
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::C) == 0);

    std::cout << "---------------------------\nCPX Instruction tests passed!\n";
  }
//----------------------------------------------------------------------------------------------------------------------------
  void test_CPY() {
    CPU cpu;
    cpu.reset();

    // Y register loaded with 144, address loaded with 80, CPY executed, Carry flag should be set
    cpu.Y = 0x90;
    cpu.writeMemory(0x00, 0xCC); // CPY Absolute
    cpu.writeMemory(0x01, 0xCD);
    cpu.writeMemory(0x02, 0xAB);
    cpu.writeMemory(0xABCD, 0x50);
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::C) == 1);

    // Y register loaded with 80, address loaded with 144, CPY executed, Carry flag should not be set
    cpu.Y = 0x50;
    cpu.writeMemory(0x03, 0xCC);
    cpu.writeMemory(0x04, 0xCD);
    cpu.writeMemory(0x05, 0xAB);
    cpu.writeMemory(0xABCD, 0x90);
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::C) == 0);

    std::cout << "---------------------------\nCPY Instruction tests passed!\n";
  }
//----------------------------------------------------------------------------------------------------------------------------
  void test_CLD_SED_CLV() {
    CPU cpu;
    cpu.reset();

    cpu.writeMemory(0x00, 0xF8); // SED
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::D) == 1);

    cpu.writeMemory(0x01, 0xD8); // CLD
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::D) == 0);

    cpu.setFlag(CPU::FLAGS::V, 1);
    cpu.writeMemory(0x02, 0xB8); // CLV
    cpu.execute();
    assert(cpu.getFlag(CPU::FLAGS::V) == 0);

    std::cout << "---------------------------\nCLD_SED_CLV Instruction tests passed!\n";
  }

	void test_NES() {
		NES nes;
		nes.load_rom("./nestest.nes");
		nes.initNES();

		std::ofstream outfile("output.txt");

		auto start = std::chrono::high_resolution_clock::now();
		for (int i = 0;i < 4954; i++) {
			outfile << std::hex <<std::uppercase << nes.cpu.PC << std::endl;
			printf("count: %d\n", i+1);
			uint8_t opcode = nes.cpu.readMemory(nes.cpu.PC);
			printf("Opcode: %02X\n", opcode);
			nes.cpu.printRegisters();
			nes.cycle();

		}
		outfile.close();
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed_time = end - start;
		std::cout << "Elapsed Time" << elapsed_time.count() << "seconds\n";
	}

	void test_Bus() {

		Bus bus;
		CPU& cpu = *bus.cpu;

		cpu.reset();
		cpu.writeBus(0x0000, 0xFF);
		uint8_t opcode = cpu.readBus(0x0000);
		assert(opcode == 0xFF);
		std::cout << "---------------------------\nBus tests passed!\n";
	}
};
