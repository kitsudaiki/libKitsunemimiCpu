#include <libKitsunemimiCpu/cpu.h>

namespace Kitsunemimi
{
namespace Cpu
{

uint32_t
getNumberOfCpuThreads()
{
    return std::thread::hardware_concurrency();
}

}
}

