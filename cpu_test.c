#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define MEM_SIZE 2048 // 2KB

// using uint8_t because we specifically want 8 bits, which may not be specified under unsigned char
uint8_t mem[MEM_SIZE];

// creating registers
// according to nesdev, these are usually stored somewhere in the $4020-$FFFF range
// should probably be changed in the future, for now just variables
uint8_t A;   // accumulator
uint8_t X;   // index x
uint8_t Y;   // index y
uint16_t PC; // program counter, 2 bytes
uint8_t S;   // stack pointer
uint8_t P;   // status register, only uses 6 bits

void print_registers() {
	printf("A: [%02X]\nX:[%02X]\nY: [%02X]\nPC: [%04X]\nS: [%02X]\nP: [%02X]\n", A, X, Y, PC, S, P);
}

// store byte value at specified address
void store(uint16_t addr, uint8_t val) {
	if (addr < MEM_SIZE) {
		mem[addr] = val;
	} else {
		fprintf(stderr, "Address out of bounds: %u\n", addr);
	}
}

// retrieve data stored at specified address
uint8_t retrieve(uint16_t addr) {
	if (addr < MEM_SIZE) {
		return mem[addr];
	} else {
		fprintf(stderr, "Address out of bounds: %u\n", addr);
		return -1;
	}
}

// print the memory, for testing
void print_mem() {
	printf("Memory Content:\n");
	for (int i = 0; i < MEM_SIZE; i+= 16) {
		for (int j = 0; j < 16 && (i+j) < MEM_SIZE; j++) {
			// 2-digit hexadecimal output
			printf("%02X ", mem[i+j]);
		}
		printf("\n");
	}
}

// sample few opcodes
void instruction(uint8_t opcode) {
	switch (opcode) {
		// Load A: Zero Page
		case 0xA5:
			A = mem[PC];
			break;
		// Store A: Zero Page
		case 0x85:
			mem[PC] = A;
			break;
		// Load X: Zero Page
		case 0xA6:
			X = mem[PC];
			break;
		// Store X: Zero Page
		case 0x86:
			mem[PC] = X;
			break;
		// Load Y: Zero Page
		case 0xA4:
			Y = mem[PC];
			break;
		// Store Y: Zero Page
		case 0x84:
			mem[PC] = Y;
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
			mem[PC]++;
			break;
		// Decrement: Zero Page
		case 0xC6:
			mem[PC]--;
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
			fprintf(stderr, "Error: Instruction not valid: '%02X'\n", opcode);
	}
}

// Run some tests using the registers/opcodes
void test_opcodes() {
	// Load value at 0x0 into register A
	PC = 0x0000;
	instruction(0xA5);
	assert(A == 0xAB);

	// Store A into mem[0x0001]
	PC = 0x0001;
	instruction(0x85);
	assert(mem[0x0001] == 0xAB);

	// Load AB into X
	PC = 0x0001;
	instruction(0xA6);
	assert(X == 0xAB);

	// Store X into mem[0x0010]
	PC = 0x0010;
	instruction(0x86);
	assert(mem[0x0010] == 0xAB);

	// Load AB into Y
	PC = 0x0010;
	instruction(0xA4);
	assert(Y == 0xAB);

	// Store Y into mem[0x0100]
	PC = 0x0100;
	instruction(0x84);
	assert(mem[0x0100] == 0xAB);

	// Transfer A to X, and X to A
	A = 0x05;
	instruction(0xAA);
	assert(A == X);
	X = 0x0A;
	instruction(0x8A);
	assert(A == X);

	// Transfer A to Y, and Y to A
	A = 0x0F;
	instruction(0xA8);
	assert(A == Y);
	Y = 0x14;
	instruction(0x98);
	assert(A == Y);

	// Increment value at 0x0000 twice
	PC = 0x0000;
	instruction(0xE6);
	instruction(0xE6);
	assert(mem[0x0000] == 0xAD);

	// Decrement value at 0x0000 once
	PC = 0x0000;
	instruction(0xC6);
	assert(mem[0x0000] == 0xAC);

	// Inc/Dec X and Y
	X = 0x0;
	Y = 0x0;
	instruction(0xE8);
	instruction(0xE8);
	instruction(0xE8);
	instruction(0xCA);
	instruction(0xC8);
	instruction(0xC8);
	instruction(0x88);
	instruction(0x88);
	assert(X == 0x2);
	assert(Y == 0x0);

	print_registers();
	printf("Opcode tests passed!\n");

}

// main function for compilation -- could use -c, but for demonstration purposes
// I'd like some output to be produced
int main() {
    // Example usage
    store(0x0000, 0xAB); // Store value 0xAB at address 0x0000
    uint8_t value = retrieve(0x0000); // Retrieve value from address 0x0000
    
    printf("Value at address 0x0000: %02X\n", value); // Should print AB

    // Print entire memory for demonstration
    print_mem();

	// Test opcodes
	test_opcodes();	

    return 0;
}
