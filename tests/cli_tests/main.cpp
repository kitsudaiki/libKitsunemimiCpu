#include <iostream>
#include <unistd.h>

#include <libKitsunemimiCpu/cpu.h>
#include <libKitsunemimiCpu/rapl.h>
#include <libKitsunemimiCpu/memory.h>
#include <libKitsunemimiCommon/logger.h>
#include <libKitsunemimiCommon/threading/thread.h>

using namespace Kitsunemimi::Cpu;

int main()
{
    Kitsunemimi::initConsoleLogger(true);

    std::cout<<"hyperthreading active: "<<isHyperthreadingEnabled()<<std::endl;
    std::cout<<"hyperthreading set to false: "<<changeHyperthreadingState(false)<<std::endl;
    std::cout<<"hyperthreading active: "<<isHyperthreadingEnabled()<<std::endl;
    std::cout<<"hyperthreading set to true: "<<changeHyperthreadingState(true)<<std::endl;
    std::cout<<"hyperthreading active: "<<isHyperthreadingEnabled()<<std::endl;

    //==============================================================================================

    std::cout<<"=============================MEMORY============================="<<std::endl;
    std::cout<<"total: "<<getTotalMemory()<<std::endl;
    std::cout<<"free: "<<getFreeMemory()<<std::endl;
    std::cout<<"page-size: "<<getPageSize()<<std::endl;

    //==============================================================================================

    std::cout<<"=============================CPU============================="<<std::endl;

    std::cout<<"threads: "<<getNumberOfCpuThreads()<<std::endl;
    std::cout<<"sockets: "<<getNumberOfCpuSockets()<<std::endl;
    std::cout<<"socket of thead 1: "<<getCpuSocketId(1)<<std::endl;
    std::cout<<"sibling of thread 1: "<<getCpuSiblingId(1)<<std::endl;

    std::cout<<"min of thread 1: "<<getCurrentMinimumSpeed(1)<<std::endl;
    std::cout<<"max of thread 1: "<<getCurrentMaximumSpeed(1)<<std::endl;

    for(int i = 0; i < 5; i++)
    {
        std::cout<<i<<" ------------------"<<std::endl;
        std::cout<<"cur of thread 0: "<<getCurrentSpeed(0)<<std::endl;

        sleep(1);
    }

    std::cout<<"#######################################################################"<<std::endl;
    for(int i = 0; i < 4; i++) {
        std::cout<<"set speed Min-speed to max: "<<setMinimumSpeed(i, 1000000000000)<<std::endl;
    }
    for(int i = 0; i < 15; i++)
    {
        std::cout<<i<<" ------------------"<<std::endl;
        std::cout<<"cur of thread 0: "<<getCurrentSpeed(0)<<std::endl;
        sleep(1);
    }
    std::cout<<"#######################################################################"<<std::endl;

    for(int i = 0; i < 4; i++) {
        resetSpeed(i);
        std::cout<<"set speed to min: "<<setMaximumSpeed(i, 400000)<<std::endl;
    }

    for(int i = 0; i < 15; i++)
    {
        std::cout<<i<<" ------------------"<<std::endl;
        std::cout<<"cur of thread 0: "<<getCurrentSpeed(0)<<std::endl;
        std::cout<<"min of thread 0: "<<getCurrentMinimumSpeed(0)<<std::endl;
        std::cout<<"max of thread 0: "<<getCurrentMaximumSpeed(0)<<std::endl;

        sleep(1);
    }
    std::cout<<"#######################################################################"<<std::endl;

    for(int i = 0; i < 4; i++) {
        resetSpeed(i);
    }

    for(int i = 0; i < 5; i++)
    {
        std::cout<<i<<" ------------------"<<std::endl;
        std::cout<<"cur of thread 0: "<<getCurrentSpeed(0)<<std::endl;

        sleep(1);
    }

    //==============================================================================================

    std::cout<<"=============================RAPL============================="<<std::endl;

    Rapl rapl(0);
    if(rapl.initRapl())
    {
        std::cout<<"info: "<<rapl.getInfo().toString()<<std::endl;

        for(int i = 0; i < 10; i++)
        {
            std::cout<<i<<" ------------------"<<std::endl;
            RaplDiff diff = rapl.calculateDiff();
            std::cout<<diff.toString()<<std::endl;
            sleep(1);
        }
    }
    else
    {
        std::cout<<"RAPL not available"<<std::endl;
    }
    //==============================================================================================

    return 0;
}
