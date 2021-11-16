/**
 *  @file       cpu.h
 *
 *  @author     Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright  MIT License
 */

#ifndef KITSUNEMIMI_CPU_CPU_H
#define KITSUNEMIMI_CPU_CPU_H

#include <stdint.h>
#include <string>
#include <fstream>

#include <libKitsunemimiCommon/logger.h>

namespace Kitsunemimi
{
namespace Cpu
{

// topological
int32_t getNumberOfCpuPackages(ErrorContainer &error);
int32_t getNumberOfCpuThreads(ErrorContainer &error);
int32_t getCpuPackageId(const int32_t threadId, ErrorContainer &error);
int32_t getCpuCoreId(const int32_t threadId, ErrorContainer &error);
int32_t getCpuSiblingId(const int32_t threadId, ErrorContainer &error);

// hyperthreading
bool isHyperthreadingEnabled(ErrorContainer &error);
bool isHyperthreadingSupported(ErrorContainer &error);
bool changeHyperthreadingState(const bool newState, ErrorContainer &error);

// speed
int64_t getMinimumSpeed(const int32_t threadId, ErrorContainer &error);
int64_t getMaximumSpeed(const int32_t threadId, ErrorContainer &error);
int64_t getCurrentSpeed(const int32_t threadId, ErrorContainer &error);

int64_t getCurrentMinimumSpeed(const int32_t threadId, ErrorContainer &error);
int64_t getCurrentMaximumSpeed(const int32_t threadId, ErrorContainer &error);

bool setMinimumSpeed(const int32_t threadId, int64_t newSpeed, ErrorContainer &error);
bool setMaximumSpeed(const int32_t threadId, int64_t newSpeed, ErrorContainer &error);
bool resetSpeed(const int32_t threadId, ErrorContainer &error);

} // namespace Cpu
} // namespace Kitsunemimi

#endif // KITSUNEMIMI_CPU_CPU_H
