#include <iostream>
#include "MemMap.h"
#include "CPU.h"

using namespace std;

MemMap* MemMap::instance = 0;
CPU* CPU::instance = 0;

int main()
{
    // Load Modules
    MemMap* mem = mem->getInstance();
    CPU* cpu = cpu->getInstance();

    cout << "Starting NES Emulator\n";

    // Test Modules
    cpu->test();
    mem->test();
    mem->clear();

    // Check legal opcode count
    for (int i = 0; i <= 0xff; i++) {
        cpu->zeroPC();
        mem->write(0, i);
        cpu->execute();
    }
    cout << "\n" << cpu->illegal_opcodes;
}