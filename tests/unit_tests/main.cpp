#include <iostream>
#include <unistd.h>

#include <libKitsunemimiCpu/cpu.h>
#include <libKitsunemimiCpu/rapl.h>
#include <libKitsunemimiCommon/logger.h>
#include <libKitsunemimiCommon/threading/thread.h>

int main()
{
   /* std::cout<<"threads: "<<Kitsunemimi::Cpu::getNumberOfCpuThreads()<<std::endl;
    std::cout<<"sockets: "<<Kitsunemimi::Cpu::getNumberOfCpuSockets()<<std::endl;
    std::cout<<"socket of thead 1: "<<Kitsunemimi::Cpu::getCpuSocketId(1)<<std::endl;
    std::cout<<"sibling of thread 1: "<<Kitsunemimi::Cpu::getCpuSiblingId(1)<<std::endl;



    std::cout<<"min of thread 1: "<<Kitsunemimi::Cpu::getCurrentMinimumSpeed(1)<<std::endl;
    std::cout<<"max of thread 1: "<<Kitsunemimi::Cpu::getCurrentMaximumSpeed(1)<<std::endl;

    for(int i = 0; i < 5; i++)
    {
        std::cout<<i<<" ------------------"<<std::endl;
        std::cout<<"cur of thread 0: "<<Kitsunemimi::Cpu::getCurrentMinimumSpeed(0)<<std::endl;

        sleep(1);
    }

    std::cout<<"cur of thread 0: "<<Kitsunemimi::Cpu::setMinimumSpeed(0, 1000000000000)<<std::endl;

    for(int i = 0; i < 5; i++)
    {
        std::cout<<i<<" ------------------"<<std::endl;
        std::cout<<"cur of thread 0: "<<Kitsunemimi::Cpu::getCurrentMinimumSpeed(0)<<std::endl;

        sleep(1);
    }

    Kitsunemimi::Cpu::resetSpeed(0);

    for(int i = 0; i < 5; i++)
    {
        std::cout<<i<<" ------------------"<<std::endl;
        std::cout<<"cur of thread 0: "<<Kitsunemimi::Cpu::getCurrentMinimumSpeed(0)<<std::endl;

        sleep(1);
    }*/

    Rapl rapl(0);
    Kitsunemimi::initConsoleLogger(true);
    std::cout<<"init: "<<rapl.initRapl()<<std::endl;

    sleep(1);
    std::cout<<"info: "<<rapl.getInfo().toString()<<std::endl;

    for(int i = 0; i < 50; i++)
    {
        std::cout<<i<<" ------------------"<<std::endl;
        RaplDiff diff = rapl.calculateDiff();
        std::cout<<diff.toString()<<std::endl;
        sleep(1);
    }

    return 0;
}
