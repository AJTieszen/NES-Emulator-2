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

    mem->clear();

    // Test CPU Instructions
    cpu->zeroPC();
    mem->write(1, 0xce);
    mem->write(2, 0xfa);
    mem->write(0xface, 0x34);
    mem->write(0xface + 1, 0x12);
    cpu->JMP(cpu->indM);
    cpu->print();
    cpu->JSR(cpu->absM);
    cpu->print();
    cpu->RTS();
    cpu->print();


}