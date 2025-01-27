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
	tests.test_jmp();
	tests.test_stack_instructions();
	tests.test_branch();
    return 0;
}

