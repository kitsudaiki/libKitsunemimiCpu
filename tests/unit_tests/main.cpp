#include <iostream>

#include <libKitsunemimiCpu/cpu.h>

int main()
{
    std::cout<<"cpus: "<<Kitsunemimi::Cpu::getNumberOfCpuThreads()<<std::endl;
    return 0;
}
