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
 * @param filePath path to the file
 * @param error reference for error-output
 *
 * @return file-content, if available, else empty string
 */
const std::string
getInfo(const std::string &filePath,
        ErrorContainer &error)
{
    // open file
    std::ifstream inFile;
    inFile.open(filePath);
    if(inFile.is_open() == false)
    {
        error.errorMessage = "can not open file to read content: \"" + filePath + "\"";
        error.possibleSolution = "for the file \"" + filePath + "\"\n"
                                 "1. check if you have read-permissions to the file"
                                 "2. check if the file even exist on your system";
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
 * @brief get max-value of a range-info-output
 *
 * @param info string with the info to parse
 *
 * @return -1 if info is invalid, else the maximum number within the range
 */
int32_t
getRangeInfo(const std::string &info)
{
    // handle case of only one core
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
 * @brief write value into file
 *
 * @param filePath absolute file-path
 * @param value value to write
 * @param error reference for error-output
 *
 * @return false, if no permission to update files, else true
 */
bool
writeToFile(const std::string &filePath,
            const std::string &value,
            ErrorContainer &error)
{
    // open file
    std::ofstream outputFile;
    outputFile.open(filePath, std::ios_base::in);
    if(outputFile.is_open() == false)
    {
        error.errorMessage = "can not open file to write new value: \"" + filePath + "\"";
        error.possibleSolution = "for the file \"" + filePath + "\"\n"
                                 "1. check if you have write-permissions to the file"
                                 "2. check if the file even exist on your system";
        return false;
    }

    // update file
    outputFile << value;
    outputFile.flush();
    outputFile.close();

    return true;
}

/**
 * @brief write new value into a cpu-freq-file
 *
 * @param value new value to write into the file
 * @param threadId id of the thread to change
 * @param fileName target file-name to update
 * @param error reference for error-output
 *
 * @return false, if no permission to update files, else true
 */
bool
writeToFile(const int64_t value,
            const int32_t threadId,
            const std::string &fileName,
            ErrorContainer &error)
{
    // build target-path
    const std::string filePath = "/sys/devices/system/cpu/cpu"
                                 + std::to_string(threadId)
                                 + "/cpufreq/"
                                 + fileName;

    return writeToFile(filePath, std::to_string(value), error);
}

/**
 * @brief get number of cpu-sockets of the system
 *
 * @param error reference for error-output
 *
 * @return -1 if target-file not found or broken, else number of cpu-sockets
 */
int32_t
getNumberOfCpuPackages(ErrorContainer &error)
{
    // get info from requested file
    const std::string filePath = "/sys/devices/system/node/possible";
    const std::string info = getInfo(filePath, error);
    if(info == "")
    {
        error.errorMessage = "Failed to get number of cpu-packages, "
                             "because can not read file \"" + filePath + "\"";
        return -1;
    }

    // process file-content
    const int32_t range = getRangeInfo(info);
    if(range == -1)
    {
        error.errorMessage = "Failed to get number of cpu-packages, "
                             "because something seems to be broken in file \"" + filePath + "\"";
    }

    return range;
}

/**
 * @brief get total number of cpu-threads of all cpu-sockets of the system
 *
 * @param error reference for error-output
 *
 * @return -1 if target-file not found or broken, else number of cpu-threads
 */
int32_t
getNumberOfCpuThreads(ErrorContainer &error)
{
    // get info from requested file
    const std::string filePath = "/sys/devices/system/cpu/possible";
    const std::string info = getInfo(filePath, error);
    if(info == "")
    {
        error.errorMessage = "Failed to get number of cpu-threads, "
                             "because can not read file \"" + filePath + "\"";
        return -1;
    }

    // process file-content
    const int32_t range = getRangeInfo(info);
    if(range == -1)
    {
        error.errorMessage = "Failed to get number of cpu-threads, "
                             "because something seems to be broken in file \"" + filePath + "\"";
    }

    return range;
}

/**
 * @brief check if hyperthreading is enabled on the system
 *
 * @param error reference for error-output
 *
 * @return true, if hyperthreading is enabled, else false
 */
bool
isHyperthreadingEnabled(ErrorContainer &error)
{
    const std::string active = getInfo("/sys/devices/system/cpu/smt/active", error);
    if(active == "")
    {
        error.errorMessage = "Failed to check if hyperthreading is active";
        return false;
    }

    return active == "1";
}

/**
 * @brief check if hyperthreading is suppored by the system
 *
 * @param error reference for error-output
 *
 * @return
 */
bool
isHyperthreadingSupported(ErrorContainer &error)
{
    const std::string htState = getInfo("/sys/devices/system/cpu/smt/control", error);
    if(htState == "")
    {
        error.errorMessage = "Failed to check if hyperthreading is supported";
        return false;
    }

    // htState can be "on", "off" or "notsupported"
    return htState != "notsupported";
}

/**
 * @brief changeHyperthreadingState
 *
 * @param newState true to enable and false to disable hyperthreading
 * @param error reference for error-output
 *
 * @return true, if hyperthreading is suppored and changes successfull, else false
 */
bool
changeHyperthreadingState(const bool newState, ErrorContainer &error)
{
    const std::string filePath = "/sys/devices/system/cpu/smt/control";
    const std::string htState = getInfo(filePath, error);
    if(htState == "")
    {
        error.errorMessage = "Failed to check if hyperthreading is supported";
        return false;
    }

    // check if hyperthreading is supported
    if(htState == "notsupported") {
        return false;
    }

    if(newState)
    {
        // check if hyperthreading is already active
        if(htState == "on") {
            return true;
        }

        // set new state
        if(writeToFile(filePath, "on", error) == false)
        {
            error.errorMessage = "Failed to enable hyperthreading";
            return false;
        }

        return true;
    }
    else
    {
        // check if hyperthreading is already disabled
        if(htState == "off") {
            return true;
        }

        // set new state
        if(writeToFile(filePath, "off", error) == false)
        {
            error.errorMessage = "Failed to disable hyperthreading";
            return false;
        }

        return true;
    }

    return true;
}

/**
 * @brief check to which cpu-socket a specific thread belongs to
 *
 * @param threadId id of the thread to check
 * @param error reference for error-output
 *
 * @return -1 if target-file not found or broken, else id of cpu-socket
 */
int32_t
getCpuPackageId(const int32_t threadId, ErrorContainer &error)
{
    // build request-path
    const std::string filePath = "/sys/devices/system/cpu/cpu"
                                 + std::to_string(threadId)
                                 + "/topology/physical_package_id";

    // get info from requested file
    const std::string info = getInfo(filePath, error);
    if(info == "")
    {
        error.errorMessage = "Failed to get package-id of the cpu-thread with id: \""
                             + std::to_string(threadId) + "\"";
        return -1;
    }

    return std::stoi(info);
}

/**
 * @brief get id of the physical core of a cpu-thread
 *
 * @param threadId id of the thread to check
 * @param error reference for error-output
 *
 * @return -1 if target-file not found or broken, else id of cpu-core
 */
int32_t
getCpuCoreId(const int32_t threadId, ErrorContainer &error)
{
    // build request-path
    const std::string filePath = "/sys/devices/system/cpu/cpu"
                                 + std::to_string(threadId)
                                 + "/topology/core_id";

    // get info from requested file
    const std::string info = getInfo(filePath, error);
    if(info == "")
    {
        error.errorMessage = "Failed to get core-id of the cpu-thread with id: \""
                             + std::to_string(threadId) + "\"";
        return -1;
    }

    return std::stoi(info);
}


/**
 * @brief get thread-id of a sibling to another thread-id in case of hyper-threading
 *
 * @param threadId id of the thread create to request
 * @param error reference for error-output
 *
 * @return -1 if target-file not found or broken, else id of the thread-sibling
 */
int32_t
getCpuSiblingId(const int32_t threadId, ErrorContainer &error)
{
    // if hyperthreading is not enabled, there are no siblings possible
    if(isHyperthreadingEnabled(error))
    {
        error.errorMessage = "Failed to get sibling-id of the cpu-thread with id: \""
                             + std::to_string(threadId)
                             + "\", because hyperthreading is not enabled or supported";
        error.possibleSolution = "Endable hyperthrading, if supported by the system";
        return -1;
    }

    // build request-path
    const std::string filePath = "/sys/devices/system/cpu/cpu"
                                 + std::to_string(threadId)
                                 + "/topology/thread_siblings_list";

    // get info from requested file
    const std::string info = getInfo(filePath, error);
    if(info == "")
    {
        error.errorMessage = "Failed to get sibling-id of the cpu-thread with id: \""
                             + std::to_string(threadId) + "\"";
        return -1;
    }

    // process content
    std::vector<std::string> siblings;
    Kitsunemimi::splitStringByDelimiter(siblings, info, ',');
    if(siblings.size() < 2)
    {
        error.errorMessage = "Failed to get sibling-id of the cpu-thread with id: \""
                             + std::to_string(threadId) + "\"";
        error.possibleSolution = "check if file \"" + filePath + "\" has contains a comma-separated"
                                 " list of thread-ids";
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
 * @param error reference for error-output
 *
 * @return -1, if file not exist, else speed-value in Hz
 */
int64_t
getSpeed(const int32_t threadId,
         const std::string &fileName,
         ErrorContainer &error)
{
    // build request-path
    const std::string filePath = "/sys/devices/system/cpu/cpu"
                                 + std::to_string(threadId)
                                 + "/cpufreq/"
                                 + fileName;

    // get info from requested file
    const std::string info = getInfo(filePath, error);
    if(info == "") {
        return -1;
    }

    return std::stol(info);
}

/**
 * @brief get current set minimum speed of a cpu-thread
 *
 * @param threadId id of thread to check
 * @param error reference for error-output
 *
 * @return -1, if file not exist, else speed-value in Hz
 */
int64_t
getCurrentMinimumSpeed(const int32_t threadId, ErrorContainer &error)
{
    const int64_t speed = getSpeed(threadId, "scaling_min_freq", error);
    if(speed == -1)
    {
        error.errorMessage = "Failed to the current minimum speed of thread with id: \""
                             + std::to_string(threadId) + "\"";
    }

    return speed;
}

/**
 * @brief get current set maximum speed of a cpu-thread
 *
 * @param threadId id of thread to check
 * @param error reference for error-output
 *
 * @return -1, if file not exist, else speed-value in Hz
 */
int64_t
getCurrentMaximumSpeed(const int32_t threadId, ErrorContainer &error)
{
    const int64_t speed = getSpeed(threadId, "scaling_max_freq", error);
    if(speed == -1)
    {
        error.errorMessage = "Failed to the current maximum speed of thread with id: \""
                             + std::to_string(threadId) + "\"";
    }

    return speed;
}

/**
 * @brief get current speed of a cpu-thread
 *
 * @param threadId id of thread to check
 * @param error reference for error-output
 *
 * @return -1, if file not exist, else speed-value in Hz
 */
int64_t
getCurrentSpeed(const int32_t threadId, ErrorContainer &error)
{
    const int64_t speed = getSpeed(threadId, "scaling_cur_freq", error);
    if(speed == -1)
    {
        error.errorMessage = "Failed to the current speed of thread with id: \""
                             + std::to_string(threadId) + "\"";
    }

    return speed;
}

/**
 * @brief get absolute minimum value of a thread
 *
 * @param threadId id of thread to check
 * @param error reference for error-output
 *
 * @return -1, if file not exist, else speed-value in Hz
 */
int64_t
getMinimumSpeed(const int32_t threadId, ErrorContainer &error)
{
    const int64_t speed = getSpeed(threadId, "cpuinfo_min_freq", error);
    if(speed == -1)
    {
        error.errorMessage = "Failed to the minimum speed of thread with id: \""
                             + std::to_string(threadId) + "\"";
    }

    return speed;
}

/**
 * @brief get absolute maxiumum value of a thread
 *
 * @param threadId id of thread to check
 * @param error reference for error-output
 *
 * @return -1, if file not exist, else speed-value in Hz
 */
int64_t
getMaximumSpeed(const int32_t threadId, ErrorContainer &error)
{
    const int64_t speed = getSpeed(threadId, "cpuinfo_max_freq", error);
    if(speed == -1)
    {
        error.errorMessage = "Failed to the maximum speed of thread with id: \""
                             + std::to_string(threadId) + "\"";
    }

    return speed;
}

/**
 * @brief set minimum speed value of cpu-thread
 *
 * @param threadId id of the thread to change
 * @param newSpeed new minimum speed-value to set in Hz
 * @param error reference for error-output
 *
 * @return false, if no permission to update files, else true
 */
bool
setMinimumSpeed(const int32_t threadId, int64_t newSpeed, ErrorContainer &error)
{
    // fix lower border
    const int64_t minSpeed = getMinimumSpeed(threadId, error);
    if(minSpeed == -1) {
        return false;
    }
    if(newSpeed < minSpeed) {
        newSpeed = minSpeed;
    }

    // fix upper border
    const int64_t maxSpeed = getMaximumSpeed(threadId, error);
    if(maxSpeed == -1) {
        return false;
    }
    if(newSpeed > maxSpeed) {
        newSpeed = maxSpeed;
    }

    // set new in value
    if(writeToFile(newSpeed, threadId, "scaling_min_freq", error) == false)
    {
        error.errorMessage = "Failed to set new minimum speed for thread with id: \""
                             + std::to_string(threadId) + "\"";
        return false;
    }

    return true;
}

/**
 * @brief setMinimumSpeed
 *
 * @param threadId id of the thread to change
 * @param newSpeed new maximum speed-value to set in Hz
 * @param error reference for error-output
 *
 * @return false, if no permission to update files, else true
 */
bool
setMaximumSpeed(const int32_t threadId, int64_t newSpeed, ErrorContainer &error)
{
    // fix lower border
    const int64_t minSpeed = getMinimumSpeed(threadId, error);
    if(minSpeed == -1) {
        return false;
    }
    if(newSpeed < minSpeed) {
        newSpeed = minSpeed;
    }

    // fix upper border
    const int64_t maxSpeed = getMaximumSpeed(threadId, error);
    if(maxSpeed == -1) {
        return false;
    }
    if(newSpeed > maxSpeed) {
        newSpeed = maxSpeed;
    }

    // set new max value
    if(writeToFile(newSpeed, threadId, "scaling_max_freq", error) == false)
    {
        error.errorMessage = "Failed to set new maximum speed for thread with id: \""
                             + std::to_string(threadId) + "\"";
        return false;
    }

    return true;
}

/**
 * @brief reset speed values to basic values
 *
 * @param threadId id of the thread to reset
 * @param error reference for error-output
 *
 * @return false, if no permission to update files, else true
 */
bool
resetSpeed(const int32_t threadId, ErrorContainer &error)
{
    // reset max-speed
    const int64_t maxSpeed = getMaximumSpeed(threadId, error);
    if(maxSpeed == -1) {
        return false;
    }
    if(setMaximumSpeed(threadId, maxSpeed, error) == false) {
        return false;
    }

    // reset min-speed
    const int64_t minSpeed = getMinimumSpeed(threadId, error);
    if(minSpeed == -1) {
        return false;
    }
    if(setMinimumSpeed(threadId, minSpeed, error) == false) {
        return false;
    }

    return true;
}

} // namespace Cpu
} // namespace Kitsunemimi
