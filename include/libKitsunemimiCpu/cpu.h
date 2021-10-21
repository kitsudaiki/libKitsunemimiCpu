#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <string>

namespace Kitsunemimi
{
namespace Cpu
{

// topological
int32_t getNumberOfCpuThreads();
int32_t getNumberOfCpuSockets();
int32_t getCpuSocketId(const int32_t threadId);
int32_t getCpuSiblingId(const int32_t threadId);

// speed
int64_t getMinimumSpeed(const int32_t threadId);
int64_t getMaximumSpeed(const int32_t threadId);
int64_t getCurrentSpeed(const int32_t threadId);

}
}

#endif // CPU_H
