#include "CPU.cpp"
#include "tests.cpp"


int main() {

	// TESTS -- uncomment as needed
	Tests tests;
    tests.test_cpu();
    tests.test_opcodes();
    tests.test_stack();
    tests.test_reset();
	tests.test_nmi();
	tests.test_irq();

    return 0;
}

