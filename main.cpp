
#include "tests.cpp"

int main() {

	// // TESTS -- uncomment as needed
	Tests tests;
	tests.test_cpu();
	tests.test_opcodes();
	tests.test_stack();
	tests.test_reset();
	tests.test_ADC();
	tests.test_nmi();
	tests.test_irq();
	tests.test_jmp();
	tests.test_stack_instructions();
	tests.test_branch();
	tests.test_ASL();
	tests.test_LSR();
	tests.test_ROL();
	tests.test_ROR();
	tests.test_CMP();
	tests.test_CPX();
	tests.test_CPY();
	tests.test_CLD_SED_CLV();
	tests.test_NES();
	tests.test_Bus();
    return 0;
}

