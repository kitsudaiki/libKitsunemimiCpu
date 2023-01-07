/**
 *  @file       memory.cpp
 *
 *  @author     Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright  MIT License
 */

#include <libKitsunemimiCpu/memory.h>

namespace Kitsunemimi
{

/**
 * @brief get total amount of main-memory of the system in bytes
 */
uint64_t
getTotalMemory()
{
    const uint64_t pages = sysconf(_SC_PHYS_PAGES);
    const uint64_t page_size = getPageSize();
    return pages * page_size;
}

/**
 * @brief get amound of free main-memory of the system in bytes
 */
uint64_t
getFreeMemory()
{
    const uint64_t pages = sysconf(_SC_AVPHYS_PAGES);
    const uint64_t page_size = getPageSize();
    return pages * page_size;
}

/**
 * @brief get page-size of the main-memory of the system in bytes
 */
uint64_t
getPageSize()
{
    return sysconf(_SC_PAGE_SIZE);
}

} // namespace Kitsunemimi
