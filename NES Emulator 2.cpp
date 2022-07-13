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
    cpu->zeroPC();
    mem->write(1, 0xce);
    mem->write(2, 0xfa);
    mem->write(0xface, 0x34);
    mem->write(0xface + 1, 0x12);
    mem->write(0xfffa, 0xa5);
    mem->write(0xfffb, 0x5a);

    cpu->JMP(cpu->indM);
    cpu->print();
    cpu->JSR(cpu->absM);
    cpu->print();
    cpu->RTS();
    cpu->print();
    cout << "\n\n";

    cpu->BRK();
    cpu->print();
    cpu->RTI();
    cpu->print();
}