#pragma once

#include "MemMap.h"

class CPU {
	// Singleton Class
	static CPU* instance;
	CPU() {}

	// Memory Access
	MemMap* mem = mem->getInstance();

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

	// Address Modes
	uint16_t abs() {
		uint8_t ll = mem->read(PC + 1);
		uint8_t hh = mem->read(PC + 2);
		return ll + (hh << 8);
	}
	uint16_t abs_x() {
		uint8_t ll = mem->read(PC + 1);
		uint8_t hh = mem->read(PC + 2);
		return ll + (hh << 8) + X;
	}

public:
	// Singleton Class
	static CPU* getInstance() {
		if (!instance) instance = new CPU;
		return instance;
	}

	// Emulator Utilities
	void test() {
		std::cout << "Testing CPU";

		int err_cnt = 0;
		int value;

		std::cout << "\n  Absolute: ";
		{
			PC = 0;
			mem->write(1, 0xCD);
			mem->write(2, 0xAB);
			value = abs();
			if (value == 0xABCD) std::cout << "OK";
			else {
				printf("Error: Expected abcd, got %0004x", value);
				err_cnt++;
			}
		}

		std::cout << "\n  Absolute, X Indexed: ";
		{
			PC = 0;
			X = 0xEF;
			mem->write(1, 0xCD);
			mem->write(2, 0xAB);
			value = abs_x();
			if (value == 0xACBC) std::cout << "OK";
			else {
				printf("Error: Expected acbc, got %0004x", value);
				err_cnt++;
			}
		}

		if (err_cnt == 0) std::cout << "\nCPU OK\n";
		else printf("\nCPU NOT OK: %d errors found\n", err_cnt);
	}
	void print() {
		printf("Program Counter: %0004x | Accumulator: %02x | X: %02x | Y: %02x | Flags: %02x | Stack Pointer: %02x\n", PC, ACC, X, Y, SF, SP);
	}

};
