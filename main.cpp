#include "CPU.cpp"



int main() {

    // CPU testing
    CPU cpu;
    cpu.writeMemory(0x10, 0xAB);
    cpu.writeMemory(0x0000, 0xAB);
    cpu.printMemory();


    cpu.setFlag(CPU::FLAGS::Z, true);
    cpu.printRegisters();
    printf("Status Flag Z: %d\n", cpu.getFlag(CPU::FLAGS::Z));


    std::cout << "Memory at 0x10: 0x" << std::hex << static_cast<int>(cpu.readMemory(0x10)) << "\n";
    printf("Value at address 0x0000: %02X\n", cpu.readMemory(0x0000));

    return 0;
}
