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

    // Test CPU Instructions
    mem->write(0x00, 0x88);

    cpu->execute();
    cpu->print();
}