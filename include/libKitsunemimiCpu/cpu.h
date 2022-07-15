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
#include <stdlib.h>

#include <libKitsunemimiCommon/logger.h>

namespace Kitsunemimi
{
namespace Cpu
{

// topological
bool getNumberOfCpuPackages(uint64_t &result, ErrorContainer &error);
bool getNumberOfCpuThreads(uint64_t &result, ErrorContainer &error);
bool getCpuPackageId(uint64_t &result, const uint64_t threadId, ErrorContainer &error);
bool getCpuCoreId(uint64_t &result, const uint64_t threadId, ErrorContainer &error);
bool getCpuSiblingId(uint64_t &result, const uint64_t threadId, ErrorContainer &error);

// hyperthreading
bool isHyperthreadingEnabled(ErrorContainer &error);
bool isHyperthreadingSupported(ErrorContainer &error);
bool changeHyperthreadingState(const bool newState, ErrorContainer &error);

// speed
bool getMinimumSpeed(uint64_t &result, const uint64_t threadId, ErrorContainer &error);
bool getMaximumSpeed(uint64_t &result, const uint64_t threadId, ErrorContainer &error);
bool getCurrentSpeed(uint64_t &result, const uint64_t threadId, ErrorContainer &error);

bool getCurrentMinimumSpeed(uint64_t &result, const uint64_t threadId, ErrorContainer &error);
bool getCurrentMaximumSpeed(uint64_t &result, const uint64_t threadId, ErrorContainer &error);

bool setMinimumSpeed(const uint64_t threadId, uint64_t newSpeed, ErrorContainer &error);
bool setMaximumSpeed(const uint64_t threadId, uint64_t newSpeed, ErrorContainer &error);
bool resetSpeed(const uint64_t threadId, ErrorContainer &error);

// temperature
bool getPkgTemperatureIds(std::vector<uint64_t> &ids, ErrorContainer &error);
double getPkgTemperature(const uint64_t pkgFileId, ErrorContainer &error);

} // namespace Cpu
} // namespace Kitsunemimi

#endif // KITSUNEMIMI_CPU_CPU_H
