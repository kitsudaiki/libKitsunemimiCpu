#include <iostream>
#include <unistd.h>

#include <libKitsunemimiCpu/cpu.h>
#include <libKitsunemimiCommon/threading/thread.h>

int main()
{
    std::cout<<"threads: "<<Kitsunemimi::Cpu::getNumberOfCpuThreads()<<std::endl;
    std::cout<<"sockets: "<<Kitsunemimi::Cpu::getNumberOfCpuSockets()<<std::endl;
    std::cout<<"socket of thead 1: "<<Kitsunemimi::Cpu::getCpuSocketId(1)<<std::endl;
    std::cout<<"sibling of thread 1: "<<Kitsunemimi::Cpu::getCpuSiblingId(1)<<std::endl;



    std::cout<<"min of thread 1: "<<Kitsunemimi::Cpu::getMinimumSpeed(1)<<std::endl;
    std::cout<<"max of thread 1: "<<Kitsunemimi::Cpu::getMaximumSpeed(1)<<std::endl;
    std::cout<<"cur of thread 1: "<<Kitsunemimi::Cpu::getCurrentSpeed(1)<<std::endl;

    return 0;
}
