#include <libKitsunemimiCpu/cpu.h>

#include <libKitsunemimiCommon/files/text_file.h>
#include <libKitsunemimiCommon/common_methods/string_methods.h>
#include <libKitsunemimiCommon/logger.h>

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
    std::string errorMessage = "";
    std::string content = "";

    const bool ret = Kitsunemimi::readFile(content, path, errorMessage);
    if(ret == false)
    {
        LOG_ERROR("Error while getting cpu-info: " + errorMessage);
        return "";
    }

    return content;
}

/**
 * @brief getNumberOfCpuThreads
 * @return
 */
int32_t
getNumberOfCpuThreads()
{
    // get info from requested file
    const std::string info = getInfo("/sys/devices/system/cpu/possible");
    if(info == ""
        || info == "\n")
    {
        return -1;
    }

    // handle case of only one socket
    if(info == "0\n") {
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
 * @brief getNumberOfCpuSockets
 * @return
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
    if(info == "0\n") {
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
 * @brief getCpuSocketId
 * @param threadId
 * @return
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
    if(info == ""
            || info == "\n")
    {
        return -1;
    }

    return std::stoi(info);
}

/**
 * @brief get thread-id of a sibling to another thread-id in case of hyper-threading
 *
 * @param threadId id of the create to request
 *
 * @return id of the sibling, if successfull, else -1
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
    if(info == ""
            || info == "\n")
    {
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
 * @brief getSpeed
 * @param threadId
 * @param fileName
 * @return
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
 * @brief getMinimumSpeed
 * @param threadId
 * @return
 */
int64_t
getMinimumSpeed(const int32_t threadId)
{
    return getSpeed(threadId, "scaling_min_freq");
}

/**
 * @brief getMaximumSpeed
 * @param threadId
 * @return
 */
int64_t
getMaximumSpeed(const int32_t threadId)
{
    return getSpeed(threadId, "scaling_max_freq");
}

/**
 * @brief getCurrentSpeed
 * @param threadId
 * @return
 */
int64_t
getCurrentSpeed(const int32_t threadId)
{
    return getSpeed(threadId, "scaling_cur_freq");
}

}
}

