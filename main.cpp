#include "tests.cpp"
#include "string"

int main(int argc, char* argv[]) {

  std::string testPath;
  bool debug = false;
  int step = -1;

  for (int i = 0; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "debug") {
      // std::cout << "Debug on!\n";
      debug = true;
    } else if (arg.rfind("test=", 0) == 0) {
      std::string testPath = arg.substr(5);
    } else if (arg.rfind("step=", 0) == 0) {
      step = std::stoi(arg.substr(5));
    }
  }

  if (testPath.empty()) {
    testPath = "./nestest.nes";
  }

	// // TESTS -- uncomment as needed
	Tests tests;
	/*tests.test_cpu();
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
	tests.test_CLD_SED_CLV();*/
	tests.test_NES(testPath, debug, step);
	/*tests.test_Bus();
	tests.test_PPU_registers();*/
  return 0;
}

