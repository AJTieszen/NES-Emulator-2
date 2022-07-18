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
	uint8_t SF = 0x20;	// Status Flags
	uint8_t SP = 0xFF;	// Stack Pointer

	// CPU Status
	unsigned int cycle = 0;
	bool extraCycle = false;

	// Helper Functions
	enum flags {
		Carry, Zero, Interrupt, Decimal, Break, unused, Overflow, Negative
	};
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
		uint16_t addr = ll + (hh << 8) + X;

		// check if page boundary crossed
		if ((addr >> 8) != (PC >> 8)) extraCycle++;
		PC += 3;
		return addr;
	}
	uint16_t abs_y() {
		uint8_t ll = mem->read(PC + 1);
		uint8_t hh = mem->read(PC + 2);
		uint16_t addr = ll + (hh << 8) + Y;

		// check if page boundary crossed
		if ((addr >> 8) != (PC >> 8)) extraCycle++;

		PC += 3;
		return addr;
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
		uint16_t addr = mem->read(PC + 1);
		uint8_t ll = mem->read(addr);
		uint8_t hh = mem->read(addr + 1);
		addr = ll + (hh << 8) + Y;

		// check if page boundary crossed
		if ((addr >> 8) != (PC >> 8)) extraCycle++;

		PC += 2;
		return addr;
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
			if (value == 0xCECB) std::cout << "OK";
			else {
				printf("Error: Expected cecb, got %0004x", value);
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
		printf("\nProgram Counter: %0004x | Accumulator: %02x | X: %02x | Y: %02x | Stack Pointer: %02x", PC, ACC, X, Y, SP);
		printf("\nFlags: NV-BDIZC | Cycle: %d", cycle);
		printf("\n       ");
		for (int i = 7; i >= 0; i--) {
			std::cout << readFlag(i);
		}
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

		PC++;
	}
	void TAY() {
		Y = ACC;

		// Set affected flags
		if (Y == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (Y & 0x80) setFlag(Negative);
		else clearFlag(Negative);

		PC++;
	}
	void TSX() {
		X = SP;

		// Set affected flags
		if (X == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (X & 0x80) setFlag(Negative);
		else clearFlag(Negative);

		PC ++;
	}
	void TXA() {
		ACC = X;

		// Set affected flags
		if (X == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (X & 0x80) setFlag(Negative);
		else clearFlag(Negative);

		PC ++;
	}
	void TXS() {
		SP = X;
		PC++;
	}
	void TYA() {
		ACC = Y;

		// Set affected flags
		if (X == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (X & 0x80) setFlag(Negative);
		else clearFlag(Negative);

		PC++;
	}

	// Stack Instructions
	void PHA() {
		push(ACC);
		PC++;
	}
	void PHP() {
		setFlag(Break);
		push(SF);
		PC ++;
	}
	void PLA() {
		ACC = pull();

		// Set affected flags
		if (X == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (X & 0x80) setFlag(Negative);
		else clearFlag(Negative);

		PC ++;
	}
	void PLP() {
		SF = pull();
		PC ++;
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

		PC ++;
	}
	void DEY() {
		Y--;

		// Set affected flags
		if (Y == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (Y & 0x80) setFlag(Negative);
		else clearFlag(Negative);

		PC ++;
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

		PC ++;
	}
	void INY() {
		Y++;

		// Set affected flags
		if (Y == 0) setFlag(Zero);
		else clearFlag(Zero);
		if (Y & 0x80) setFlag(Negative);
		else clearFlag(Negative);

		PC++;
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

		PC++;
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

		PC ++;
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

		PC ++;
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

		PC ++;
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
		PC++;
	}
	void CLD() {
		std::cout << "Ricoh 2A03 variant omits decimal mode!";
		PC ++;
	}
	void CLI() {
		clearFlag(Interrupt);
		PC ++;
	}
	void CLV() {
		clearFlag(Overflow);
		PC ++;
	}
	void SEC() {
		setFlag(Carry);
		PC ++;
	}
	void SED() {
		std::cout << "Ricoh 2A03 variant omits decimal mode!";
		PC ++;
	}
	void SEI() {
		setFlag(Interrupt);
		PC ++;
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
		uint16_t oldPC = PC;
		int8_t offset = mem->read(PC + 1);
		if (!readFlag(Carry)) {
			if ((PC & 0x00FF) + offset > 0xFF || (PC & 0x00FF) + offset < 0) cycle += 2;
			else cycle++;

			PC += offset;
		}
		PC += 2;
	}
	void BCS() {
		uint16_t oldPC = PC;
		int8_t offset = mem->read(PC + 1);
		if (readFlag(Carry)) {
			if ((PC & 0x00FF) + offset > 0xFF || (PC & 0x00FF) + offset < 0) cycle += 2;
			else cycle++;

			PC += offset;
		}
		PC += 2;
	}
	void BEQ() {
		uint16_t oldPC = PC;
		int8_t offset = mem->read(PC + 1);
		if (readFlag(Zero)) {
			if ((PC & 0x00FF) + offset > 0xFF || (PC & 0x00FF) + offset < 0) cycle += 2;
			else cycle++;

			PC += offset;
		}
		PC += 2;
	}
	void BMI() {
		uint16_t oldPC = PC;
		int8_t offset = mem->read(PC + 1);
		if (readFlag(Negative)) {
			if ((PC & 0x00FF) + offset > 0xFF || (PC & 0x00FF) + offset < 0) cycle += 2;
			else cycle++;

			PC += offset;
		}
		PC += 2;
	}
	void BNE() {
		uint16_t oldPC = PC;
		int8_t offset = mem->read(PC + 1);
		if (!readFlag(Zero)) {
			if ((PC & 0x00FF) + offset > 0xFF || (PC & 0x00FF) + offset < 0) cycle += 2;
			else cycle++;

			PC += offset;
		}
		PC += 2;
	}
	void BPL() {
		uint16_t oldPC = PC;
		int8_t offset = mem->read(PC + 1);
		if (!readFlag(Negative)) {
			if ((PC & 0x00FF) + offset > 0xFF || (PC & 0x00FF) + offset < 0) cycle += 2;
			else cycle++;

			PC += offset;
		}
		PC += 2;
	}
	void BVC() {
		uint16_t oldPC = PC;
		int8_t offset = mem->read(PC + 1);
		if (!readFlag(Overflow)) {
			if ((PC & 0x00FF) + offset > 0xFF || (PC & 0x00FF) + offset < 0) cycle += 2;
			else cycle++;

			PC += offset;
		}
		PC += 2;
	}
	void BVS() {
		uint16_t oldPC = PC;
		int8_t offset = mem->read(PC + 1);
		if (readFlag(Overflow)) {
			if ((PC & 0x00FF) + offset > 0xFF || (PC & 0x00FF) + offset < 0) cycle += 2;
			else cycle++;

			PC += offset;
		}
		PC += 2;

		if ((PC >> 8) != (oldPC >> 8)) extraCycle = true;
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

	// Interrupts (software)
	void BRK() {
		setFlag(Interrupt);
		setFlag(Break);
		PC += 2;
		push(PC >> 8);
		push(PC);
		push(SF);

		PC = mem->read(0xFFFA) + (mem->read(0xFFFB) << 8);
	}
	void RTI() {
		SF = pull() & 0xCF; // ignore break and unused flags
		PC = pull() + (pull() << 8);
	}

	// Miscellaneous
	void BIT(uint8_t mode) {
		uint8_t value = readMem(mode);
		SF = SF | (value & 0b11000000);
		if ((value & ACC) == 0) clearFlag(Zero);
		else setFlag(Zero);
	}
	void NOP() {
		PC++;
	}

	// Hardware Interrupts
	void reset() {
		setFlag(unused);	// Internally hardwired as 1
		setFlag(Interrupt);	// Block IRQ's until properly initialized
		clearFlag(Decimal);
		SP = 0xFD;

		cycle = 7;
		PC = mem->read(0xFFFC) + (mem->read(0xFFFD) << 8);
	}
	void irq() {
		clearFlag(Break);
		cycle += 7;

		if (!readFlag(Interrupt)) {
			PC += 2;
			push(PC >> 8);
			push(PC);
			push(SF);
			PC = mem->read(0xFFFE) + (mem->read(0xFFFF) << 8);
			setFlag(Interrupt);
		}
	}
	void nmi() {
		setFlag(Interrupt);
		PC += 2;
		push(PC >> 8);
		push(PC);
		push(SF);

		PC = mem->read(0xFFFA) + (mem->read(0xFFFB) << 8);

	}

	void execute() {
		uint8_t opcode = mem->read(PC);
		extraCycle = false;

		switch (opcode) {
			// Catch illegal opcodes
		default:
			std::cout << "\nError: illegal opcode: ";
			printf("%02x.", opcode);
		break;

			// ADC
		case 0x69:
			ADC(immM);
			cycle += 2;
			break;
		case 0x65:
			ADC(zpgM);
			cycle += 3;
			break;
		case 0x75:
			ADC(zpg_xM);
			cycle += 4;
			break;
		case 0x6D:
			ADC(absM);
			cycle += 4;
			break;
		case 0x7D:
			ADC(abs_xM);
			cycle += 4 + extraCycle;
			break;
		case 0x79:
			ADC(abs_yM);
			cycle += 4 + extraCycle;
			break;
		case 0x61:
			ADC(x_indM);
			cycle += 6;
			break;
		case 0x71:
			ADC(ind_yM);
			cycle += 5 + extraCycle;
			break;

			// AND
		case 0x29:
			AND(immM);
			cycle += 2;
			break;
		case 0x25:
			AND(zpgM);
			cycle += 3;
			break;
		case 0x35:
			AND(zpg_xM);
			cycle += 4;
			break;
		case 0x2D:
			AND(absM);
			cycle += 4;
			break;
		case 0x3D:
			AND(abs_xM);
			cycle += 4 + extraCycle;
			break;
		case 0x39:
			AND(abs_yM);
			cycle += 4 + extraCycle;
			break;
		case 0x21:
			AND(x_indM);
			cycle += 6;
			break;
		case 0x31:
			AND(ind_yM);
			cycle += 5 + extraCycle;
			break;

			// ASL
		case 0x0A:
			ASL();
			cycle += 2;
			break;
		case 0x06:
			ASL(zpgM);
			cycle += 5;
			break;
		case 0x16:
			ASL(zpg_xM);
			cycle += 6;
			break;
		case 0x0E:
			ASL(absM);
			cycle += 6;
			break;
		case 0x1E:
			ASL(absM);
			cycle += 7;
			break;

			// Conditional Branches
		case 0x90:
			BCC();
			cycle += 2;
			break;
		case 0xB0:
			BCS();
			cycle += 2;
			break;
		case 0xF0:
			BEQ();
			cycle += 2;
			break;
		case 0x30:
			BMI();
			cycle += 2;
			break;
		case 0xD0:
			BNE();
			cycle += 2;
			break;
		case 0x10:
			BPL();
			cycle += 2;
			break;
		case 0x50:
			BVC();
			cycle += 2;
			break;
		case 0x70:
			BVS();
			cycle += 2;
			break;

			// BIT
		case 0x24:
			BIT(zpgM);
			cycle += 3;
			break;
		case 0x2C:
			BIT(absM);
			cycle += 4;
			break;

			// BRK
		case 0x00:
			BRK();
			cycle += 7;
			break;

			// Clear Flags
		case 0x18:
			CLC();
			cycle += 2;
			break;
		case 0xD8:
			CLD();
			cycle += 2;
			break;
		case 0x58:
			CLI();
			cycle += 2;
			break;
		case 0xB8:
			CLV();
			cycle += 2;
			break;

			// Compare with Accumulator
		case 0xC9:
			CMP(immM);
			cycle += 2;
			break;
		case 0xC5:
			CMP(zpgM);
			cycle += 3;
			break;
		case 0xD5:
			CMP(zpg_xM);
			cycle += 4;
			break;
		case 0xCD:
			CMP(absM);
			cycle += 4;
			break;
		case 0xDD:
			CMP(abs_xM);
			cycle += 4 + extraCycle;
			break;
		case 0xD9:
			CMP(abs_yM);
			cycle += 4 + extraCycle;
			break;
		case 0xC1:
			CMP(x_indM);
			cycle += 6;
			break;
		case 0xD1:
			CMP(ind_yM);
			cycle += 5 + extraCycle;
			break;

			// Compare with XY Registers
		case 0xE0:
			CPX(immM);
			cycle += 2;
			break;
		case 0xE4:
			CPX(zpgM);
			cycle += 3;
			break;
		case 0xEC:
			CPX(absM);
			cycle += 4;
			break;
		case 0xC0:
			CPY(immM);
			cycle += 2;
			break;
		case 0xC4:
			CPY(zpgM);
			cycle += 3;
			break;
		case 0xCC:
			CPY(absM);
			cycle += 4;
			break;

		// DEC
		case 0xC6:
			DEC(zpgM);
			cycle += 5;
			break;
		case 0xD6:
			DEC(zpg_xM);
			cycle += 6;
			break;
		case 0xCE:
			DEC(absM);
			cycle += 6;
			break;
		case 0xDE:
			DEC(abs_xM);
			cycle += 7;
			break;
		case 0xCA:
			DEX();
			cycle += 2;
			break;
		case 0x88:
			DEY();
			cycle += 2;
			break;

			// EOR
		case 0x49:
			EOR(immM);
			cycle += 2;
			break;
		case 0x45:
			EOR(zpgM);
			cycle += 3;
			break;
		case 0x55:
			EOR(zpg_xM);
			cycle += 4;
			break;
		case 0x4D:
			EOR(absM);
			cycle += 4;
			break;
		case 0x5D:
			EOR(abs_xM);
			cycle += 4 + extraCycle;
			break;
		case 0x59:
			EOR(abs_yM);
			cycle += 4 + extraCycle;
			break;
		case 0x41:
			EOR(x_indM);
			cycle += 6;
			break;
		case 0x51:
			EOR(ind_yM);
			cycle += 5 + extraCycle;
			break;

			// INC
		case 0xE6:
			INC(zpgM);
			cycle += 5;
			break;
		case 0xF6:
			INC(zpg_xM);
			cycle += 6;
			break;
		case 0xEE:
			INC(absM);
			cycle += 6;
			break;
		case 0xFE:
			INC(abs_xM);
			cycle += 7;
			break;
		case 0xE8:
			INX();
			cycle += 2;
			break;
		case 0xC8:
			INY();
			cycle += 2;
			break;

			// Jumps
		case 0x4C:
			JMP(absM);
			cycle += 3;
			break;
		case 0x6C:
			JMP(indM);
			cycle += 3;
			break;
		case 0x20:
			JSR(absM);
			cycle += 3;
			break;

			// LDA
		case 0xA9:
			ADC(immM);
			cycle += 2;
			break;
		case 0xA5:
			ADC(zpgM);
			cycle += 3;
			break;
		case 0xB5:
			ADC(zpg_xM);
			cycle += 4;
			break;
		case 0xAD:
			ADC(absM);
			cycle += 4;
			break;
		case 0xBD:
			ADC(abs_xM);
			cycle += 4 + extraCycle;
			break;
		case 0xB9:
			ADC(abs_yM);
			cycle += 4 + extraCycle;
			break;
		case 0xA1:
			ADC(x_indM);
			cycle += 6;
			break;
		case 0xB1:
			ADC(ind_yM);
			cycle += 5 + extraCycle;
			break;

			// LDX
		case 0xA2:
			LDX(immM);
			cycle += 2;
			break;
		case 0xA6:
			LDX(zpgM);
			cycle += 3;
			break;
		case 0xB6:
			LDX(zpg_yM);
			cycle += 4;
			break;
		case 0xAE:
			LDX(absM);
			cycle += 4;
			break;
		case 0xBE:
			LDX(abs_yM);
			cycle += 4 + extraCycle;
			break;

			// LDY
		case 0xA0:
			LDY(immM);
			cycle += 2;
			break;
		case 0xA4:
			LDY(zpgM);
			cycle += 3;
			break;
		case 0xB4:
			LDY(zpg_xM);
			cycle += 4;
			break;
		case 0xAC:
			LDY(absM);
			cycle += 4;
			break;
		case 0xBC:
			LDY(abs_xM);
			cycle += 4 + extraCycle;
			break;

			// LSR
		case 0x4A:
			LSR();
			cycle += 2;
			break;
		case 0x46:
			LSR(zpgM);
			cycle += 5;
			break;
		case 0x56:
			LSR(zpg_xM);
			cycle += 6;
			break;
		case 0x4E:
			LSR(absM);
			cycle += 6;
			break;
		case 0x5E:
			LSR(abs_xM);
			cycle += 7;
			break;

			// NOP
		case 0xEA:
			cycle += 2;
			break;

			// ORA
		case 0x09:
			ORA(immM);
			cycle += 2;
			break;
		case 0x05:
			ORA(zpgM);
			cycle += 3;
			break;
		case 0x15:
			ORA(zpg_xM);
			cycle += 4;
			break;
		case 0x0D:
			ORA(absM);
			cycle += 4;
			break;
		case 0x1D:
			ORA(abs_xM);
			cycle += 4 + extraCycle;
			break;
		case 0x19:
			ORA(abs_yM);
			cycle += 4 + extraCycle;
			break;
		case 0x01:
			ORA(x_indM);
			cycle += 6;
			break;
		case 0x11:
			ORA(ind_yM);
			cycle += 5 + extraCycle;
			break;

			// Push Stack
		case 0x48:
			PHA();
			cycle += 3;
			break;
		case 0x08:
			PHP();
			cycle += 3;
			break;

			// Pull Stack
		case 0x68:
			PLA();
			cycle += 4;
			break;
		case 0x28:
			PLP();
			cycle += 4;
			break;

			// ROL
		case 0x2A:
			ROL();
			cycle += 2;
			break;
		case 0x26:
			ROL(zpgM);
			cycle += 5;
			break;
		case 0x36:
			ROL(zpg_xM);
			cycle += 6;
			break;
		case 0x2E:
			ROL(absM);
			cycle += 6;
			break;
		case 0x3E:
			ROL(abs_xM);
			cycle += 7;
			break;

			// ROR
		case 0x6A:
			ROR();
			cycle += 2;
			break;
		case 0x66:
			ROR(zpgM);
			cycle += 5;
			break;
		case 0x76:
			ROR(zpg_xM);
			cycle += 6;
			break;
		case 0x6E:
			ROR(absM);
			cycle += 6;
			break;
		case 0x7E:
			ROR(abs_xM);
			cycle += 7;
			break;

			// Return
		case 0x40:
			RTI();
			cycle += 6;
			break;
		case 0x60:
			RTS();
			cycle += 6;
			break;

			// SBC
		case 0xE9:
			SBC(immM);
			cycle += 2;
			break;
		case 0xE5:
			SBC(zpgM);
			cycle += 3;
			break;
		case 0xF5:
			SBC(zpg_xM);
			cycle += 4;
			break;
		case 0xED:
			SBC(absM);
			cycle += 4;
			break;
		case 0xFD:
			SBC(abs_xM);
			cycle += 4 + extraCycle;
			break;
		case 0xF9:
			SBC(abs_yM);
			cycle += 4 + extraCycle;
			break;
		case 0xE1:
			SBC(x_indM);
			cycle += 6;
			break;
		case 0xF1:
			SBC(ind_yM);
			cycle += 5 + extraCycle;
			break;

			//Flags
		case 0x38:
			SEC();
			cycle += 2;
			break;
		case 0xF8:
			SED();
			cycle += 2;
			break;
		case 0x78:
			SEI();
			cycle += 2;
			break;

			// STA
		case 0x85:
			STA(zpgM);
			cycle += 3;
			break;
		case 0x95:
			STA(zpg_xM);
			cycle += 4;
			break;
		case 0x8D:
			STA(absM);
			cycle += 4;
			break;
		case 0x9D:
			STA(abs_xM);
			cycle += 5;
			break;
		case 0x99:
			STA(abs_yM);
			cycle += 5;
			break;
		case 0x81:
			STA(x_indM);
			cycle += 6;
			break;
		case 0x91:
			STA(ind_yM);
			cycle += 6;
			break;

			// STX
		case 0x86:
			STX(zpgM);
			cycle += 3;
			break;
		case 0x96:
			STX(zpg_xM);
			cycle += 4;
			break;
		case 0x8E:
			STX(absM);
			cycle += 4;
			break;

			// STY
		case 0x84:
			STY(zpgM);
			cycle += 3;
			break;
		case 0x94:
			STY(zpg_xM);
			cycle += 4;
			break;
		case 0x8C:
			STY(absM);
			cycle += 4;
			break;

			// Transfer Registers
		case 0xAA:
			TAX();
			cycle += 2;
			break;
		case 0xA8:
			TAY();
			cycle += 2;
			break;
		case 0xBA:
			TSX();
			cycle += 2;
			break;
		case 0x8A:
			TXA();
			cycle += 2;
			break;
		case 0x98:
			TYA();
			cycle += 2;
			break;
		}
	}
};
