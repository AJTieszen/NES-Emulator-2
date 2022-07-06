#pragma once

class MemMap {
	// Singleton Class
	static MemMap* instance;
	MemMap() {}

private:
	// Memory Regions
	uint8_t ram[0x0800]; // 2KB Work RAM
	uint8_t ppu[0x0008]; // PPU Registers
	uint8_t apu[0x0020]; // APU and IO registers
	uint8_t crt[0xBFE0]; // Cartridge Address Space

public:
	// Singleton Class
	static MemMap* getInstance() {
		if (!instance) instance = new MemMap;
		return instance;
	}

	// Emulator Utilities
	void test() {
		std::cout << "\nTesting Memory Map:";

		int err_cnt = 0;
		int value;
		for (int i = 0; i <= 0xFFFF; i++) {
			write(i, i);

			value = read(i);
			if (value != i % 0x0100) {
				printf("Memory Error at byte %0004x: Expected %02x, read %02x\n", i, i %0100, value);
				err_cnt++;
			}
		}

		clear();

		if (err_cnt == 0) std::cout << "\nMemory Map OK\n";
		else printf("\nMemory Map NOT OK: %d errors found\n", err_cnt);
	}
	void clear() {
		for (uint16_t i = 0; i <= 0xBFE0; i++) {
			ram[i % 0x0800] = 0;
			ppu[i % 0x0008] = 0;
			ram[i % 0x0020] = 0;
			ram[i] = 0;
		}
	}

	// Memory Functions
	uint8_t read(uint16_t addr) {
		if (addr < 0x2000) return ram[addr % 0x0800];					// RAM + Mirrors
		else if (addr < 0x4000) return ppu[(addr - 0x2000) % 0x0008];	// PPU + Mirrors
		else if (addr < 0x4020) return apu[(addr - 0x4000)];			// APU & IO
		else return crt[(addr - 0x4020)];								// Cartridge
	}
	void write(uint16_t addr, uint8_t value) {
		if (addr < 0x2000) ram[addr % 0x0800] = value;					// RAM + Mirrors
		else if (addr < 0x4000) ppu[(addr - 0x2000) % 0x0008] = value;	// PPU + Mirrors
		else if (addr < 0x4020) apu[(addr - 0x4000)] = value;			// APU & IO
		else crt[(addr - 0x4020)] = value;								// Cartridge
	}
};
