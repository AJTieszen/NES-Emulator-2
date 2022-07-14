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

    // Test CPU Instructions
    mem->write(0xFFFA, 0x00);
    mem->write(0xFFFB, 0x00);

    cpu->nmi();
    cpu->print();
}