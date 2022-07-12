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

	// CPU Status
	unsigned int cycle = 0;
	bool extraCycle = false;

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
		PC += 3;
		return ll + (hh << 8);
	}
	uint16_t abs_x() {
		uint8_t ll = mem->read(PC + 1);
		uint8_t hh = mem->read(PC + 2);
		PC += 3;
		return ll + (hh << 8) + X;
	}
	uint16_t abs_y() {
		uint8_t ll = mem->read(PC + 1);
		uint8_t hh = mem->read(PC + 2);
		PC += 3;
		return ll + (hh << 8) + Y;
	}
	uint16_t imm() {
		uint16_t addr = PC + 1;
		PC += 2;
		return addr;
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
		PC += 2;
		return ll + (hh << 8);
	}
	uint16_t ind_y() {
		uint16_t addr = mem->read(PC + 1) + Y;
		uint8_t ll = mem->read(addr);
		uint8_t hh = mem->read(addr + 1);
		PC += 2;
		return ll + (hh << 8);
	}
	uint16_t rel() {
		int8_t offset = mem->read(PC + 1);
		return PC + offset;
	}
	uint8_t zpg() {
		uint8_t addr = mem->read(PC + 1);
		PC += 2;
		return addr;
	}
	uint8_t zpg_x() {
		uint8_t addr = mem->read(PC + 1) + X;
		PC += 2;
		return addr;
	}
	uint8_t zpg_y() {
		uint8_t addr = mem->read(PC + 1) + Y;
		PC += 2;
		return addr;
	}

	uint8_t readMem(uint8_t mode) {
		uint16_t addr;
		switch (mode) {
		case 1: addr = abs(); break;
		case 2: addr = abs_x(); break;
		case 3: addr = abs_y(); break;
		case 4: addr = imm(); break;
		case 5: addr = ind(); break;
		case 6: addr = x_ind(); break;
		case 7: addr = ind_y(); break;
		case 8: addr = zpg(); break;
		case 9: addr = zpg_x(); break;
		case 10: addr = zpg_y(); break;
		}
		return mem->read(addr);
	}
	void writeMem(uint8_t mode, uint8_t value) {
		uint16_t addr;
		switch (mode) {
		case 1: addr = abs(); break;
		case 2: addr = abs_x(); break;
		case 3: addr = abs_y(); break;
		// case 4: addr = imm(); break;
		case 5: addr = ind(); break;
		case 6: addr = x_ind(); break;
		case 7: addr = ind_y(); break;
		case 8: addr = zpg(); break;
		case 9: addr = zpg_x(); break;
		case 10: addr = zpg_y(); break;
		}
		mem->write(addr, value);
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


		PC = 0;
		X = 0xEF;
		Y = 0xFE;
		mem->write(1, 0xCD);
		mem->write(2, 0xAB);
		mem->write(0xABCD, 0xCE);
		mem->write(0xABCE, 0xFA);
		mem->write(0x78, 0xDE);
		mem->write(0x79, 0xFA);
		mem->write(0x1CB, 0xED);
		mem->write(0x1CC, 0xAC);

		std::cout << "\n  Absolute mode: ";{
			value = abs();
			if (value == 0xABCD) std::cout << "OK";
			else {
				printf("Error: Expected abcd, got %0004x", value);
				err_cnt++;
			}
		}
		PC = 0;
		std::cout << "\n  Absolute, X Indexed mode: ";{
			value = abs_x();
			if (value == 0xACBC) std::cout << "OK";
			else {
				printf("Error: Expected acbc, got %0004x", value);
				err_cnt++;
			}
		}
		PC = 0;
		std::cout << "\n  Absolute, Y Indexed mode: ";{
			value = abs_y();
			if (value == 0xACCB) std::cout << "OK";
			else {
				printf("Error: Expected accb, got %0004x", value);
				err_cnt++;
			}
		}
		PC = 0;
		std::cout << "\n  Immediate mode: ";{
			value = imm();
			if (value == 1) std::cout << "OK";
			else {
				printf("Error: Expected 0001, got %0004x", value);
				err_cnt++;
			}
		}
		PC = 0;
		std::cout << "\n  Indirect mode: ";{
			value = ind();
			if (value == 0xFACE) std::cout << "OK";
			else {
				printf("Error: Expected face, got %0004x", value);
				err_cnt++;
			}
		}
		PC = 0;
		std::cout << "\n  X, Indirect mode: ";{
			X = 0xAB;

			value = x_ind();
			if (value == 0xFADE) std::cout << "OK";
			else {
				printf("Error: Expected fade, got %0004x", value);
				err_cnt++;
			}
		}
		PC = 0;
		std::cout << "\n  Indirect, Y mode: ";{
			value = ind_y();
			if (value == 0xACED) std::cout << "OK";
			else {
				printf("Error: Expected aced, got %0004x", value);
				err_cnt++;
			}
		}
		PC = 0;
		std::cout << "\n  Relative mode: ";{
			value = rel();
			if (value == 0xFFCD) std::cout << "OK";
			else {
				printf("Error: Expected ffed, got %0004x", value);
				err_cnt++;
			}
		}
		PC = 0;
		std::cout << "\n  Zero Page mode: ";{
			value = zpg();
			if (value == 0x00CD) std::cout << "OK";
			else {
				printf("Error: Expected 00cd, got %0004x", value);
				err_cnt++;
			}
		}
		PC = 0;
		std::cout << "\n  Zero Page, X mode: ";{
			value = zpg_x();
			if (value == 0x0078) std::cout << "OK";
			else {
				printf("Error: Expected 0078, got %0004x", value);
				err_cnt++;
			}
		}
		PC = 0;
		std::cout << "\n  Zero Page, Y mode: ";{
			value = zpg_y();
			if (value == 0x00CB) std::cout << "OK";
			else {
				printf("Error: Expected 00cb, got %0004x", value);
				err_cnt++;
			}
		}
		PC = 0;

		if (err_cnt == 0) std::cout << "\nCPU OK\n";
		else printf("\nCPU NOT OK: %d errors found\n", err_cnt);
		mem->clear();
	}
	void print() {
		printf("\nProgram Counter: %0004x | Accumulator: %02x | X: %02x | Y: %02x | Flags: %02x | Stack Pointer: %02x", PC, ACC, X, Y, SF, SP);
	}
	void zeroPC() {
		PC = 0;
	}

// CPU Instructions
	// Transfer Instructions
	void LDA(uint8_t mode) {
		uint8_t value = readMem(mode);
		ACC = value;

		// Set affected flags
		if (value == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (value & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void LDX(uint8_t mode) {
		uint8_t value = readMem(mode);
		X = value;

		// Set affected flags
		if (value == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (value & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void LDY(uint8_t mode) {
		uint8_t value = readMem(mode);
		Y = value;

		// Set affected flags
		if (value == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (value & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void STA(uint8_t mode) {
		uint8_t value = ACC;
		writeMem(mode, value);
	}
	void STX(uint8_t mode) {
		uint8_t value = X;
		writeMem(mode, value);
	}
	void STY(uint8_t mode) {
		uint8_t value = Y;
		writeMem(mode, value);
	}
	void TAX() {
		X = ACC;

		// Set affected flags
		if (X == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (X & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void TAY() {
		Y = ACC;

		// Set affected flags
		if (Y == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (Y & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void TSX() {
		X = SP;

		// Set affected flags
		if (X == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (X & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void TXA() {
		ACC = X;

		// Set affected flags
		if (X == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (X & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void TXS() {
		SP = X;
	}
	void TYA() {
		ACC = Y;

		// Set affected flags
		if (X == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (X & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}

	// Stack Instructions
	void PHA() {
		uint16_t addr = SP + 0x100;
		mem->write(addr, ACC);
		SP--;
	}
	void PHP() {
		uint16_t addr = SP + 0x100;
		setFlag(4);
		mem->write(addr, SF);
		SP--;
	}
	void PLA() {
		SP++;
		uint16_t addr = SP + 0x100;
		ACC = mem->read(addr);

		// Set affected flags
		if (X == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (X & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void PLP() {
		SP++;
		uint16_t addr = SP + 0x100;
		SF = mem->read(addr);
	}

	// Increments and Decrements
	void DEC(uint8_t mode) {
		uint16_t oldPC = PC;
		uint8_t value = readMem(mode);
		value--;
		PC = oldPC;
		writeMem(mode, value);

		// Set affected flags
		if (value == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (value & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void DEX() {
		X--;

		// Set affected flags
		if (X == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (X & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void DEY() {
		Y--;

		// Set affected flags
		if (Y == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (Y & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void INC(uint8_t mode) {
		uint16_t oldPC = PC;
		uint8_t value = readMem(mode);
		value++;
		PC = oldPC;
		writeMem(mode, value);

		// Set affected flags
		if (value == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (value & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void INX() {
		X++;

		// Set affected flags
		if (X == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (X & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void INY() {
		Y++;

		// Set affected flags
		if (Y == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (Y & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}

	// Arithmetic Operations
	void ADC(uint8_t mode) {
		int value = ACC + readMem(mode) + readFlag(0);
		ACC = value;

		// Set Affected Flags
		if (value > 0xFF) setFlag(0); // Carry
		else clearFlag(0);
		if (value > 0x7F) setFlag(6); // Overflow
		else clearFlag(6);
		if (value == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (value & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void SBC(uint8_t mode) {
		int value = ACC - readMem(mode) - !readFlag(0);
		ACC = value;

		if (value < 0) clearFlag(0); // Carry
		else setFlag(0);
		if (value > 0x7F) setFlag(6); // Overflow
		else clearFlag(6);
		if (value == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (value & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}

	// Logical Operations
	void AND(uint8_t mode) {
		uint8_t value = ACC & readMem(mode);
		ACC = value;

		// Set affected flags
		if (value == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (value & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void EOR(uint8_t mode) {
		uint8_t value = ACC ^ readMem(mode);
		ACC = value;

		// Set affected flags
		if (value == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (value & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void ORA(uint8_t mode) {
		uint8_t value = ACC | readMem(mode);
		ACC = value;

		// Set affected flags
		if (value == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (value & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}

	// Bit Shifts
	void ASL() {
		if (ACC & 0x80) setFlag(0);
		else clearFlag(0);
		ACC = ACC << 1;

		// Set Flags
		if (ACC == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (ACC & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void LSR() {
		if (ACC & 0x01) setFlag(0);
		else clearFlag(0);
		ACC = ACC >> 1;

		// Set Flags
		if (ACC == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (ACC & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void ROL() {
		if (ACC & 0x80) setFlag(0);
		else clearFlag(0);
		bool shiftIn = readFlag(0);
		ACC = (ACC << 1) + shiftIn;

		// Set Flags
		if (ACC == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (ACC & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void ROR() {
		if (ACC & 0x01) setFlag(0);
		else clearFlag(0);
		bool shiftIn = readFlag(0);
		ACC = (ACC >> 1) + (shiftIn << 7);

		// Set Flags
		if (ACC == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (ACC & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void ASL(uint8_t mode) {
		uint16_t oldPC = PC;
		uint8_t value = readMem(mode);

		if (value & 0x80) setFlag(0);
		else clearFlag(0);
		value = value << 1;

		PC = oldPC;
		writeMem(mode, value);

		// Set Flags
		if (value == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (value & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void LSR(uint8_t mode) {
		uint16_t oldPC = PC;
		uint8_t value = readMem(mode);

		if (value & 0x01) setFlag(0);
		else clearFlag(0);
		value = value >> 1;

		PC = oldPC;
		writeMem(mode, value);

		// Set Flags
		if (value == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (value & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void ROL(uint8_t mode) {
		uint16_t oldPC = PC;
		uint8_t value = readMem(mode);

		if (value & 0x80) setFlag(0);
		else clearFlag(0);
		bool shiftIn = readFlag(0);
		value = (value << 1) + shiftIn;

		PC = oldPC;
		writeMem(mode, value);

		// Set Flags
		if (value == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (value & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}
	void ROR(uint8_t mode) {
		uint16_t oldPC = PC;
		uint8_t value = readMem(mode);

		if (value & 0x01) setFlag(0);
		else clearFlag(0);
		bool shiftIn = readFlag(0);
		value = (value >> 1) + (shiftIn << 7);

		PC = oldPC;
		writeMem(mode, value);

		// Set Flags
		if (value == 0) setFlag(1);   // Zero
		else clearFlag(1);
		if (value & 0x80) setFlag(7); // Negative
		else clearFlag(7);
	}

	// Flag instructions
	void CLC() {
		clearFlag(0);
	}
	void CLD() {
		cout << "Ricoh 2A03 variant omits decimal mode!";
	}
	void CLI() {
		clearFlag(2);
	}
	void CLV() {
		clearFlag(6);
	}
	void SEC() {
		setFlag(0);
	}
	void SED() {
		cout << "Ricoh 2A03 variant omits decimal mode!";
	}
	void SEI() {
		setFlag(2);
	}
};
