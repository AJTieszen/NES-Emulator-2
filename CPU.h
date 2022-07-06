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
	uint16_t abs_y() {
		uint8_t ll = mem->read(PC + 1);
		uint8_t hh = mem->read(PC + 2);
		return ll + (hh << 8) + Y;
	}
	uint16_t imm() {
		return PC + 1;
	}
	uint16_t ind() {
		uint16_t addr = abs();
		uint8_t ll = mem->read(addr);
		uint8_t hh = mem->read(addr + 1);
		return ll + (hh << 8);
	}
	uint16_t x_ind() {
		uint8_t addr = mem->read(PC + 1) + X;
		uint8_t ll = mem->read(addr);
		uint8_t hh = mem->read(addr + 1);
		return ll + (hh << 8);
	}
	uint16_t ind_y() {
		uint16_t addr = mem->read(PC + 1) + Y;
		uint8_t ll = mem->read(addr);
		uint8_t hh = mem->read(addr + 1);
		return ll + (hh << 8);
	}
	uint16_t rel() {
		int8_t offset = mem->read(PC + 1);
		return PC + offset;
	}
	uint8_t zpg() {
		return mem->read(PC + 1);
	}
	uint8_t zpg_x() {
		return mem->read(PC + 1) + X;
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

		std::cout << "\n  Absolute mode: ";
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
		std::cout << "\n  Absolute, X Indexed mode: ";
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
		std::cout << "\n  Absolute, Y Indexed mode: ";
		{
			PC = 0;
			Y = 0xFE;
			mem->write(1, 0xCD);
			mem->write(2, 0xAB);
			value = abs_y();
			if (value == 0xACCB) std::cout << "OK";
			else {
				printf("Error: Expected accb, got %0004x", value);
				err_cnt++;
			}
		}
		std::cout << "\n  Immediate mode: ";
		{
			PC = 0;
			value = imm();
			if (value == 1) std::cout << "OK";
			else {
				printf("Error: Expected 0001, got %0004x", value);
				err_cnt++;
			}
		}
		std::cout << "\n  Indirect mode: ";
		{
			PC = 0;
			mem->write(1, 0xCD);
			mem->write(2, 0xAB);

			mem->write(0xABCD, 0xCE);
			mem->write(0xABCE, 0xFA);
			value = ind();
			if (value == 0xFACE) std::cout << "OK";
			else {
				printf("Error: Expected face, got %0004x", value);
				err_cnt++;
			}
		}
		std::cout << "\n  X, Indirect mode: ";
		{
			PC = 0;
			mem->write(1, 0xCD);
			X = 0xAB;

			mem->write(0x78, 0xDE);
			mem->write(0x79, 0xFA);

			value = x_ind();
			if (value == 0xFADE) std::cout << "OK";
			else {
				printf("Error: Expected fade, got %0004x", value);
				err_cnt++;
			}
		}
		std::cout << "\n  Indirect, Y mode: ";
		{
			PC = 0;
			mem->write(1, 0xCD);
			Y = 0xFE;

			mem->write(0x1CB, 0xED);
			mem->write(0x1CC, 0xAC);
			value = ind_y();
			if (value == 0xACED) std::cout << "OK";
			else {
				printf("Error: Expected aced, got %0004x", value);
				err_cnt++;
			}
		}
		std::cout << "\n  Relative mode: ";
		{
			PC = 0;
			mem->write(1, 0xCD);

			value = rel();
			if (value == 0xFFCD) std::cout << "OK";
			else {
				printf("Error: Expected ffed, got %0004x", value);
				err_cnt++;
			}
		}
		std::cout << "\n  Zero Page mode: ";
		{
			PC = 0;
			mem->write(1, 0xCD);

			value = zpg();
			if (value == 0x00CD) std::cout << "OK";
			else {
				printf("Error: Expected 00cd, got %0004x", value);
				err_cnt++;
			}
		}
		std::cout << "\n  Zero Page, X mode: ";
		{
			PC = 0;
			mem->write(1, 0xCD);
			X = 0xAB;

			value = zpg_x();
			if (value == 0x0078) std::cout << "OK";
			else {
				printf("Error: Expected 0078, got %0004x", value);
				err_cnt++;
			}
		}

		if (err_cnt == 0) std::cout << "\nCPU OK\n";
		else printf("\nCPU NOT OK: %d errors found\n", err_cnt);
		mem->clear();
	}
	void print() {
		printf("Program Counter: %0004x | Accumulator: %02x | X: %02x | Y: %02x | Flags: %02x | Stack Pointer: %02x\n", PC, ACC, X, Y, SF, SP);
	}

};
