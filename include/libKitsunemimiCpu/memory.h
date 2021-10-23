/**
 *  @file       memory.h
 *
 *  @author     Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright  MIT License
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <unistd.h>
#include <stdint.h>

namespace Kitsunemimi
{
namespace Cpu
{

uint64_t getTotalMemory();
uint64_t getFreeMemory();
uint64_t getPageSize();

}
}

#endif // MEMORY_H
