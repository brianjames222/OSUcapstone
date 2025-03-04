#include "tests.cpp"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <iostream>

int main() {

	SDL_Init(SDL_INIT_EVERYTHING);
	std::cout << "SDL Initialized Successfully." << std::endl;
	SDL_Quit();

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
	tests.test_PPU_registers();
	tests.test_pulse1_sound();
    return 0;
}

