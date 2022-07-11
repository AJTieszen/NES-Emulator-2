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
    cpu->LDA(4);
    cpu->print();

    cpu->zeroPC();
    cpu->LDX(4);
    cpu->print();

    cpu->zeroPC();
    cpu->LDY(4);
    cpu->print();

    cpu->zeroPC();
    mem->write(1, 0xcd);
    mem->write(2, 0xab);
    cpu->STY(1);
    printf("\n  %02x", mem->read(0xabcd));
}