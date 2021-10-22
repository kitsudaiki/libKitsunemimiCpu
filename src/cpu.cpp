/**
 *  @file       cpu.cpp
 *
 *  @author     Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright  MIT License
 */

#include <libKitsunemimiCpu/cpu.h>

#include <libKitsunemimiCommon/common_methods/string_methods.h>

namespace Kitsunemimi
{
namespace Cpu
{

/**
 * @brief generic function to get file-content of a requested file
 *
 * @param path path to the file
 *
 * @return file-content, if available, else empty string
 */
const std::string
getInfo(const std::string &path)
{
    // open file
    std::ifstream inFile;
    inFile.open(path);
    if(inFile.is_open() == false) {
        return "";
    }

    // get file-content
    std::stringstream strStream;
    strStream << inFile.rdbuf();
    inFile.close();

    // make content clean
    std::string content = strStream.str();
    Kitsunemimi::trim(content);

    return content;
}

/**
 * @brief get total number of cpu-threads of all cpu-sockets of the system
 *
 * @return -1 if target-file not found or broken, else number of cpu-threads
 */
int32_t
getNumberOfCpuThreads()
{
    // get info from requested file
    const std::string info = getInfo("/sys/devices/system/cpu/possible");
    if(info == "") {
        return -1;
    }

    // handle case of only one socket
    if(info == "0") {
        return 1;
    }

    // process content
    std::vector<std::string> numberRange;
    Kitsunemimi::splitStringByDelimiter(numberRange, info, '-');
    if(numberRange.size() < 2) {
        return -1;
    }

    return std::stoi(numberRange.at(1)) + 1;
}

/**
 * @brief get number of cpu-sockets of the system
 *
 * @return -1 if target-file not found or broken, else number of cpu-sockets
 */
int32_t
getNumberOfCpuSockets()
{
    // get info from requested file
    const std::string info = getInfo("/sys/devices/system/node/possible");
    if(info == "") {
        return -1;
    }

    // handle case of only one socket
    if(info == "0") {
        return 1;
    }

    // process content
    std::vector<std::string> numberRange;
    Kitsunemimi::splitStringByDelimiter(numberRange, info, '-');
    if(numberRange.size() < 2) {
        return -1;
    }

    return std::stoi(numberRange.at(1)) + 1;
}

/**
 * @brief check to which cpu-socket a specific thread belongs to
 *
 * @param threadId id of the thread to check
 *
 * @return -1 if target-file not found or broken, else id of cpu-socket
 */
int32_t
getCpuSocketId(const int32_t threadId)
{
    // build request-path
    const std::string filePath = "/sys/devices/system/cpu/cpu"
                                 + std::to_string(threadId)
                                 + "/topology/physical_package_id";

    // get info from requested file
    const std::string info = getInfo(filePath);
    if(info == "") {
        return -1;
    }

    return std::stoi(info);
}

/**
 * @brief get thread-id of a sibling to another thread-id in case of hyper-threading
 *
 * @param threadId id of the thread create to request
 *
 * @return -1 if target-file not found or broken, else id of the thread-sibling
 */
int32_t
getCpuSiblingId(const int32_t threadId)
{
    // build request-path
    const std::string filePath = "/sys/devices/system/cpu/cpu"
                                 + std::to_string(threadId)
                                 + "/topology/thread_siblings_list";

    // get info from requested file
    const std::string info = getInfo(filePath);
    if(info == "") {
        return -1;
    }

    // process content
    std::vector<std::string> siblings;
    Kitsunemimi::splitStringByDelimiter(siblings, info, ',');
    if(siblings.size() < 2) {
        return -1;
    }

    // filter correct result from the output
    const int32_t second = std::stoi(siblings.at(1));
    if(second == threadId) {
        return std::stoi(siblings.at(0));
    }
    return second;
}

/**
 * @brief get speed-value of a file of a thread
 *
 * @param threadId id of the thread create to request
 * @param fileName name of the file in cpufreq-directory to read
 *
 * @return -1, if file not exist, else speed-value in Hz
 */
int64_t
getSpeed(const int32_t threadId,
         const std::string &fileName)
{
    // build request-path
    const std::string filePath = "/sys/devices/system/cpu/cpu"
                                 + std::to_string(threadId)
                                 + "/cpufreq/"
                                 + fileName;

    // get info from requested file
    const std::string info = getInfo(filePath);
    if(info == "") {
        return -1;
    }

    return std::stol(info);
}

/**
 * @brief get current set minimum speed of a cpu-thread
 *
 * @param threadId id of thread to check
 *
 * @return -1, if file not exist, else speed-value in Hz
 */
int64_t
getCurrentMinimumSpeed(const int32_t threadId)
{
    return getSpeed(threadId, "scaling_min_freq");
}

/**
 * @brief get current set maximum speed of a cpu-thread
 *
 * @param threadId id of thread to check
 *
 * @return -1, if file not exist, else speed-value in Hz
 */
int64_t
getCurrentMaximumSpeed(const int32_t threadId)
{
    return getSpeed(threadId, "scaling_max_freq");
}

/**
 * @brief get current speed of a cpu-thread
 *
 * @param threadId id of thread to check
 *
 * @return -1, if file not exist, else speed-value in Hz
 */
int64_t
getCurrentSpeed(const int32_t threadId)
{
    return getSpeed(threadId, "scaling_cur_freq");
}

/**
 * @brief get absolute minimum value of a thread
 *
 * @param threadId id of thread to check
 *
 * @return -1, if file not exist, else speed-value in Hz
 */
int64_t
getMinimumSpeed(const int32_t threadId)
{
    return getSpeed(threadId, "cpuinfo_min_freq");
}

/**
 * @brief get absolute maxiumum value of a thread
 *
 * @param threadId id of thread to check
 *
 * @return -1, if file not exist, else speed-value in Hz
 */
int64_t
getMaximumSpeed(const int32_t threadId)
{
    return getSpeed(threadId, "cpuinfo_max_freq");
}

/**
 * @brief write new value into a cpu-freq-file
 *
 * @param value new value to write into the file
 * @param threadId id of the thread to change
 * @param fileName target file-name to update
 *
 * @return false, if no permission to update files, else true
 */
bool
writeToFile(const int64_t value,
            const int32_t threadId,
            const std::string &fileName)
{
    // build target-path
    const std::string filePath = "/sys/devices/system/cpu/cpu"
                                 + std::to_string(threadId)
                                 + "/cpufreq/"
                                 + fileName;

    // open file
    std::ofstream outputFile;
    outputFile.open(filePath, std::ios_base::in);
    if(outputFile.is_open() == false) {
        return false;
    }

    // update file
    outputFile << std::to_string(value);
    outputFile.flush();
    outputFile.close();

    return true;
}

/**
 * @brief set minimum speed value of cpu-thread
 *
 * @param threadId id of the thread to change
 * @param newSpeed new minimum speed-value to set in Hz
 *
 * @return false, if no permission to update files, else true
 */
bool
setMinimumSpeed(const int32_t threadId, int64_t newSpeed)
{
    // fix lower border
    const int64_t minSpeed = getMinimumSpeed(threadId);
    if(newSpeed < minSpeed) {
        newSpeed = minSpeed;
    }

    // fix upper border
    const int64_t maxSpeed = getMaximumSpeed(threadId);
    if(newSpeed > maxSpeed) {
        newSpeed = maxSpeed;
    }

    return writeToFile(newSpeed, threadId, "scaling_min_freq");
}

/**
 * @brief setMinimumSpeed
 *
 * @param threadId id of the thread to change
 * @param newSpeed new maximum speed-value to set in Hz
 *
 * @return false, if no permission to update files, else true
 */
bool
setMaximumSpeed(const int32_t threadId, int64_t newSpeed)
{
    // fix lower border
    const int64_t minSpeed = getMinimumSpeed(threadId);
    if(newSpeed < minSpeed) {
        newSpeed = minSpeed;
    }

    // fix upper border
    const int64_t maxSpeed = getMaximumSpeed(threadId);
    if(newSpeed > maxSpeed) {
        newSpeed = maxSpeed;
    }

    return writeToFile(newSpeed, threadId, "scaling_max_freq");
}

/**
 * @brief reset speed values to basic values
 *
 * @param threadId id of the thread to reset
 *
 * @return false, if no permission to update files, else true
 */
bool
resetSpeed(const int32_t threadId)
{
    // reset max-speed
    if(setMaximumSpeed(threadId, getMaximumSpeed(threadId)) == false) {
        return false;
    }

    // reset min-speed
    if(setMinimumSpeed(threadId, getMinimumSpeed(threadId)) == false) {
        return false;
    }

    return true;
}

}
}

