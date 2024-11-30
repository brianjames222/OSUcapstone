#include <stdio.h>
#include <stdint.h>

#define MEM_SIZE 2048 // 2KB

// using uint8_t because we specifically want 8 bits, which may not be specified under unsigned char
uint8_t mem[MEM_SIZE];

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

// main function for compilation -- could use -c, but for demonstration purposes
// I'd like some output to be produced
int main() {
    // Example usage
    store(0x0000, 0xAB); // Store value 0xAB at address 0x0000
    uint8_t value = retrieve(0x0000); // Retrieve value from address 0x0000
    
    printf("Value at address 0x0000: %02X\n", value); // Should print AB

    // Print entire memory for demonstration
    print_mem();

    return 0;
}
