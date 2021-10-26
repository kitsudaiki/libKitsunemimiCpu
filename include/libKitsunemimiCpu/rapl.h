/**
 *  @file       rapl.h
 *
 *  @author     Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright  MIT License
 */

#include <cstdint>
#include <string>
#include <unistd.h>
#include <cmath>
#include <chrono>
#include <libKitsunemimiCommon/logger.h>

#ifndef KITSUNEMIMI_CPU_RAPL_H
#define KITSUNEMIMI_CPU_RAPL_H

namespace Kitsunemimi
{
namespace Cpu
{

struct RaplDiff
{
    double pkgDiff = 0;
    double pp0Diff = 0;
    double pp1Diff = 0;
    double dramDiff = 0;

    double pkgAvg = 0.0;
    double pp0Avg = 0.0;
    double pp1Avg = 0.0;
    double dramAvg = 0.0;

    double time = 0.0;

    const std::string toString()
    {
        std::string content = "";
        content += "pkgDiff: " + std::to_string(pkgDiff) + " J\n";
        content += "pp0Diff: " + std::to_string(pp0Diff) + " J\n";
        content += "pp1Diff: " + std::to_string(pp1Diff) + " J\n";
        content += "dramDiff: " + std::to_string(dramDiff) + " J\n";
        content +=  "---\n";
        content += "pkgAvg: " + std::to_string(pkgAvg) + " J/s\n";
        content += "pp0Avg: " + std::to_string(pp0Avg) + " J/s\n";
        content += "pp1Avg: " + std::to_string(pp1Avg) + " J/s\n";
        content += "dramAvg: " + std::to_string(dramAvg) + " J/s\n";
        return content;
    }
};

struct RaplInfo
{
    double power_units = 0.0;
    double energy_units = 0.0;
    double time_units = 0.0;
    double thermal_spec_power = 0.0;
    double minimum_power = 0.0;
    double maximum_power = 0.0;
    double time_window = 0.0;
    bool supportPP1 = false;

    const std::string toString()
    {
        std::string content = "";
        content += "Power units: " + std::to_string(power_units) + " W\n";
        content += "Energy units: " + std::to_string(energy_units) + " J\n";
        content += "Time units: " + std::to_string(time_units) + " s\n";
        content +=  "---\n";
        content += "Package thermal spec: " + std::to_string(thermal_spec_power) + " W\n";
        content += "Package minimum power: " + std::to_string(minimum_power) + " W\n";
        content += "Package maximum power: " + std::to_string(maximum_power) + " W\n";
        content += "Package maximum time window: " + std::to_string(time_window) + " s\n";
        return content;
    }
};

class Rapl
{
public:
    Rapl(const int32_t threadId);
    bool initRapl();

    RaplDiff calculateDiff();
    RaplInfo getInfo() const;

private:
    struct RaplState
    {
        uint64_t pkg = 0;
        uint64_t pp0 = 0;
        uint64_t pp1 = 0;
        uint64_t dram = 0;
        std::chrono::high_resolution_clock::time_point timeStamp;
    };

    int32_t m_threadId = 0;
    int m_fd;
    bool m_isInit = false;

    RaplState m_lastState;
    RaplInfo m_info;

    bool checkPP1();
    bool openMSR();
    uint64_t readMSR(const int32_t offset);
};

} // namespace Cpu
} // namespace Kitsunemimi

#endif // KITSUNEMIMI_CPU_RAPL_H
