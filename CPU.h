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
	uint8_t SP = 0xff;		// Stack Pointer

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
	enum flags {
		Carry, Zero, Interrupt, Decimal, Break, unused, Overflow, Negative
	};
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
	void push(uint8_t value) {
		uint16_t addr = SP + 0x100;
		mem->write(addr, value);
		SP--;
	}
	uint8_t pull() {
		SP++;
		uint16_t addr = SP + 0x100;
		return mem->read(addr);
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
	enum mode {
		absM = 1, abs_xM, abs_yM, immM, indM, x_indM, ind_yM, zpgM, zpg_xM, zpg_yM
	};

	// Transfer Instructions
	void LDA(uint8_t mode) {
		uint8_t value = readMem(mode);
		ACC = value;

		// Set affected flags
		if (value == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (value & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void LDX(uint8_t mode) {
		uint8_t value = readMem(mode);
		X = value;

		// Set affected flags
		if (value == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (value & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void LDY(uint8_t mode) {
		uint8_t value = readMem(mode);
		Y = value;

		// Set affected flags
		if (value == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (value & 0x80) setFlag(Negative);
		else clearFlag(Negative);
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
		if (X == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (X & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void TAY() {
		Y = ACC;

		// Set affected flags
		if (Y == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (Y & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void TSX() {
		X = SP;

		// Set affected flags
		if (X == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (X & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void TXA() {
		ACC = X;

		// Set affected flags
		if (X == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (X & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void TXS() {
		SP = X;
	}
	void TYA() {
		ACC = Y;

		// Set affected flags
		if (X == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (X & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}

	// Stack Instructions
	void PHA() {
		push(ACC);
	}
	void PHP() {
		setFlag(4);
		push(SF);
	}
	void PLA() {
		ACC = pull();

		// Set affected flags
		if (X == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (X & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void PLP() {
		SF = pull();
	}

	// Increments and Decrements
	void DEC(uint8_t mode) {
		uint16_t oldPC = PC;
		uint8_t value = readMem(mode);
		value--;
		PC = oldPC;
		writeMem(mode, value);

		// Set affected flags
		if (value == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (value & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void DEX() {
		X--;

		// Set affected flags
		if (X == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (X & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void DEY() {
		Y--;

		// Set affected flags
		if (Y == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (Y & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void INC(uint8_t mode) {
		uint16_t oldPC = PC;
		uint8_t value = readMem(mode);
		value++;
		PC = oldPC;
		writeMem(mode, value);

		// Set affected flags
		if (value == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (value & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void INX() {
		X++;

		// Set affected flags
		if (X == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (X & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void INY() {
		Y++;

		// Set affected flags
		if (Y == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (Y & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}

	// Arithmetic Operations
	void ADC(uint8_t mode) {
		int value = ACC + readMem(mode) + readFlag(Carry);
		ACC = value;

		// Set Affected Flags
		if (value > 0xFF) setFlag(Carry);
		else clearFlag(Carry);
		if (value > 0x7F) setFlag(Overflow);
		else clearFlag(6);
		if (value == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (value & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void SBC(uint8_t mode) {
		int value = ACC - readMem(mode) - !readFlag(Carry);
		ACC = value;

		if (value < 0) clearFlag(Carry);
		else setFlag(Carry);
		if (value > 0x7F) setFlag(Overflow);
		else clearFlag(6);
		if (value == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (value & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}

	// Logical Operations
	void AND(uint8_t mode) {
		uint8_t value = ACC & readMem(mode);
		ACC = value;

		// Set affected flags
		if (value == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (value & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void EOR(uint8_t mode) {
		uint8_t value = ACC ^ readMem(mode);
		ACC = value;

		// Set affected flags
		if (value == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (value & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void ORA(uint8_t mode) {
		uint8_t value = ACC | readMem(mode);
		ACC = value;

		// Set affected flags
		if (value == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (value & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}

	// Bit Shifts
	void ASL() {
		if (ACC & 0x80) setFlag(Carry);
		else clearFlag(Carry);
		ACC = ACC << 1;

		// Set Flags
		if (ACC == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (ACC & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void LSR() {
		if (ACC & 0x01) setFlag(Carry);
		else clearFlag(Carry);
		ACC = ACC >> 1;

		// Set Flags
		if (ACC == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (ACC & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void ROL() {
		if (ACC & 0x80) setFlag(Carry);
		else clearFlag(Carry);
		bool shiftIn = readFlag(Carry);
		ACC = (ACC << 1) + shiftIn;

		// Set Flags
		if (ACC == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (ACC & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void ROR() {
		if (ACC & 0x01) setFlag(Carry);
		else clearFlag(Carry);
		bool shiftIn = readFlag(Carry);
		ACC = (ACC >> 1) + (shiftIn << 7);

		// Set Flags
		if (ACC == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (ACC & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void ASL(uint8_t mode) {
		uint16_t oldPC = PC;
		uint8_t value = readMem(mode);

		if (value & 0x80) setFlag(Carry);
		else clearFlag(Carry);
		value = value << 1;

		PC = oldPC;
		writeMem(mode, value);

		// Set Flags
		if (value == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (value & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void LSR(uint8_t mode) {
		uint16_t oldPC = PC;
		uint8_t value = readMem(mode);

		if (value & 0x01) setFlag(Carry);
		else clearFlag(Carry);
		value = value >> 1;

		PC = oldPC;
		writeMem(mode, value);

		// Set Flags
		if (value == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (value & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void ROL(uint8_t mode) {
		uint16_t oldPC = PC;
		uint8_t value = readMem(mode);

		if (value & 0x80) setFlag(Carry);
		else clearFlag(Carry);
		bool shiftIn = readFlag(Carry);
		value = (value << 1) + shiftIn;

		PC = oldPC;
		writeMem(mode, value);

		// Set Flags
		if (value == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (value & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}
	void ROR(uint8_t mode) {
		uint16_t oldPC = PC;
		uint8_t value = readMem(mode);

		if (value & 0x01) setFlag(Carry);
		else clearFlag(Carry);
		bool shiftIn = readFlag(Carry);
		value = (value >> 1) + (shiftIn << 7);

		PC = oldPC;
		writeMem(mode, value);

		// Set Flags
		if (value == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (value & 0x80) setFlag(Negative);
		else clearFlag(Negative);
	}

	// Flag instructions
	void CLC() {
		clearFlag(Carry);
	}
	void CLD() {
		std::cout << "Ricoh 2A03 variant omits decimal mode!";
	}
	void CLI() {
		clearFlag(Interrupt);
	}
	void CLV() {
		clearFlag(Overflow);
	}
	void SEC() {
		setFlag(Carry);
	}
	void SED() {
		std::cout << "Ricoh 2A03 variant omits decimal mode!";
	}
	void SEI() {
		setFlag(Interrupt);
	}

	// Comparisons
	void CMP(uint8_t mode) {
		uint8_t value = readMem(mode);

		if (ACC == value) {
			setFlag(Zero);
			setFlag(Carry);
			clearFlag(Negative);
		}
		if (ACC < value) {
			clearFlag(Zero);
			clearFlag(Carry);
			setFlag(Negative);
		}
		if (ACC > value) {
			clearFlag(Zero);
			setFlag(Carry);
			clearFlag(Negative);
		}
	}
	void CPX(uint8_t mode) {
		uint8_t value = readMem(mode);

		if (Y == value) {
			setFlag(Zero);
			setFlag(Carry);
			clearFlag(Negative);
		}
		if (Y < value) {
			clearFlag(Zero);
			clearFlag(Carry);
			setFlag(Negative);
		}
		if (Y > value) {
			clearFlag(Zero);
			setFlag(Carry);
			clearFlag(Negative);
		}
	}
	void CPY(uint8_t mode) {
		uint8_t value = readMem(mode);

		if (Y == value) {
			setFlag(Zero);
			setFlag(Carry);
			clearFlag(Negative);
		}
		if (Y < value) {
			clearFlag(Zero);
			clearFlag(Carry);
			setFlag(Negative);
		}
		if (Y > value) {
			clearFlag(Zero);
			setFlag(Carry);
			clearFlag(Negative);
		}
	}

	// Conditional Branches
	void BCC() {
		int8_t offset = mem->read(PC + 1);
		if (!readFlag(Carry)) PC += offset;
		PC += 2;
	}
	void BCS() {
		int8_t offset = mem->read(PC + 1);
		if (readFlag(Carry)) PC += offset;
		PC += 2;
	}
	void BEQ() {
		int8_t offset = mem->read(PC + 1);
		if (readFlag(Zero)) PC += offset;
		PC += 2;
	}
	void BMI() {
		int8_t offset = mem->read(PC + 1);
		if (readFlag(Negative)) PC += offset;
		PC += 2;
	}
	void BNE() {
		int8_t offset = mem->read(PC + 1);
		if (!readFlag(Zero)) PC += offset;
		PC += 2;
	}
	void BPL() {
		int8_t offset = mem->read(PC + 1);
		if (!readFlag(Negative)) PC += offset;
		PC += 2;
	}
	void BVC() {
		int8_t offset = mem->read(PC + 1);
		if (!readFlag(Overflow)) PC += offset;
		PC += 2;
	}
	void BVS() {
		int8_t offset = mem->read(PC + 1);
		if (readFlag(Overflow)) PC += offset;
		PC += 2;
	}

	// Jumps
	void JMP(uint8_t mode) {
		uint16_t addr;
		if (mode == absM) addr = abs();
		if (mode == indM) addr = ind();
		PC = addr;
	}
	void JSR(uint8_t mode) {
		push(PC >> 8);
		push(PC);
		JMP(mode);
	}
	void RTS() {
		// pull return address from stack
		PC = pull() + (pull() << 8);
	}
};
