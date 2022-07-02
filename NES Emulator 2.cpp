#include <iostream>
#include "MemMap.h";
#include "CPU.h";

using namespace std;

int main()
{
    MemMap mem;
    CPU cpu;

    cout << "Starting NES Emulator\n";

    mem.test();
    cpu.test();
}