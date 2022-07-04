#pragma once

class CPU {
	// Singleton Class
	static CPU* instance;
	CPU() {}

private:
	// Internal Registers
	uint16_t PC = 0;	// Program Counter	
	uint8_t ACC = 0;	// Accumulator
	uint8_t X = 0;		// X Register
	uint8_t Y = 0;		// Y Register
	uint8_t SF = 0;		// Status Flags
	uint8_t SP = 0;		// Stack Pointer

	// Helper Functions
	void setFlag(int id) {
		SF = SF | 1 << id;
	}
	void clearFlag(int id) {
		uint8_t mask = 0xFF ^ (1 << id);
		SF = SF & mask;
	}
	bool readFlag(int id) {
		return (SF & 1 << id) != 0;
	}

public:
	// Singleton Class
	static CPU* getInstance() {
		if (!instance) instance = new CPU;
		return instance;
	}

	// Emulator Utilities
	void test() {
		std::cout << "\nCPU OK\n";
	}
	void print() {
		printf("Program Counter: %0004x | Accumulator: %02x | X: %02x | Y: %02x | Flags: %02x | Stack Pointer: %02x\n", PC, ACC, X, Y, SF, SP);
	}

};
