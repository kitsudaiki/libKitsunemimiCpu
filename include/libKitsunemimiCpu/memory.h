/**
 *  @file       memory.h
 *
 *  @author     Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright  MIT License
 */

#ifndef KITSUNEMIMI_CPU_MEMORY_H
#define KITSUNEMIMI_CPU_MEMORY_H

#include <unistd.h>
#include <stdint.h>

namespace Kitsunemimi
{

uint64_t getTotalMemory();
uint64_t getFreeMemory();
uint64_t getPageSize();

} // namespace Kitsunemimi

#endif // KITSUNEMIMI_CPU_MEMORY_H
