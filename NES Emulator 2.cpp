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
    mem->test();
    cpu->test();

    // Test CPU Instructions
    cpu->zeroPC();
    mem->write(1, 0xac);
    mem->write(0xac, 0xff);
    cpu->DEC(1);
    printf("\n  %02x", mem->read(0xac));
}