/**
 *  @file       cpu.h
 *
 *  @author     Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright  MIT License
 */

#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <string>
#include <fstream>

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

int64_t getCurrentMinimumSpeed(const int32_t threadId);
int64_t getCurrentMaximumSpeed(const int32_t threadId);

bool setMinimumSpeed(const int32_t threadId, int64_t newSpeed);
bool setMaximumSpeed(const int32_t threadId, int64_t newSpeed);
bool resetSpeed(const int32_t threadId);

}
}

#endif // CPU_H
