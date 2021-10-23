﻿/**
 *  @file       rapl.cpp
 *
 *  @author     Tobias Anker <tobias.anker@kitsunemimi.moe>
 *
 *  @copyright  MIT License
 */

#include <libKitsunemimiCpu/rapl.h>
#include <libKitsunemimiCommon/logger.h>

typedef std::chrono::milliseconds chronoMilliSec;
typedef std::chrono::microseconds chronoMicroSec;
typedef std::chrono::nanoseconds chronoNanoSec;
typedef std::chrono::seconds chronoSec;
typedef std::chrono::high_resolution_clock::time_point chronoTimePoint;
typedef std::chrono::high_resolution_clock chronoClock;

namespace Kitsunemimi
{
namespace Cpu
{

#define MSR_RAPL_POWER_UNIT            0x606

/*
 * Platform specific RAPL Domains.
 * Note that PP1 RAPL Domain is supported on 062A only
 * And DRAM RAPL Domain is supported on 062D only
 */
/* Package RAPL Domain */
#define MSR_PKG_RAPL_POWER_LIMIT       0x610
#define MSR_PKG_ENERGY_STATUS          0x611
#define MSR_PKG_PERF_STATUS            0x13
#define MSR_PKG_POWER_INFO             0x614

/* PP0 RAPL Domain */
#define MSR_PP0_POWER_LIMIT            0x638
#define MSR_PP0_ENERGY_STATUS          0x639
#define MSR_PP0_POLICY                 0x63A
#define MSR_PP0_PERF_STATUS            0x63B

/* PP1 RAPL Domain, may reflect to uncore devices */
#define MSR_PP1_POWER_LIMIT            0x640
#define MSR_PP1_ENERGY_STATUS          0x641
#define MSR_PP1_POLICY                 0x642

/* DRAM RAPL Domain */
#define MSR_DRAM_POWER_LIMIT           0x618
#define MSR_DRAM_ENERGY_STATUS         0x619
#define MSR_DRAM_PERF_STATUS           0x61B
#define MSR_DRAM_POWER_INFO            0x61C

/* RAPL UNIT BITMASK */
#define POWER_UNIT_OFFSET              0
#define POWER_UNIT_MASK                0x0F

#define ENERGY_UNIT_OFFSET             0x08
#define ENERGY_UNIT_MASK               0x1F00

#define TIME_UNIT_OFFSET               0x10
#define TIME_UNIT_MASK                 0xF000

#define SIGNATURE_MASK                 0xFFFF0
#define IVYBRIDGE_E                    0x306F0
#define SANDYBRIDGE_E                  0x206D0

/**
 * @brief Rapl::Rapl
 * @param threadId
 */
Rapl::Rapl(const int32_t threadId)
{
    m_threadId = threadId;
}

/**
 * @brief initalize rapl-class by open and reading msr-file
 *
 * @return false, if already initialized or can not open msr-file, else true
 */
bool
Rapl::initRapl()
{
    // check if already initialized
    if(m_isInit) {
        return false;
    }

    // try to open msr-file
    const bool ret = openMSR();
    if(ret == false) {
        return false;
    }

    // check if cpu supports pp1-value
    m_info.supportPP1 = checkPP1();

    // Read MSR_RAPL_POWER_UNIT Register
    uint64_t raw_value = readMSR(MSR_RAPL_POWER_UNIT);
    m_info.power_units = pow(0.5, (double) (raw_value & 0xf));
    m_info.energy_units = pow(0.5, (double) ((raw_value >> 8) & 0x1f));
    m_info.time_units = pow(0.5,	(double) ((raw_value >> 16) & 0xf));

    // Read MSR_PKG_POWER_INFO Register
    raw_value = readMSR(MSR_PKG_POWER_INFO);
    m_info.thermal_spec_power = m_info.power_units * ((double)(raw_value & 0x7fff));
    m_info.minimum_power = m_info.power_units * ((double)((raw_value >> 16) & 0x7fff));
    m_info.maximum_power = m_info.power_units * ((double)((raw_value >> 32) & 0x7fff));
    m_info.time_window = m_info.time_units * ((double)((raw_value >> 48) & 0x7fff));

    // create inital state
    RaplState initialState;
    initialState.timeStamp = std::chrono::system_clock::now();
    m_lastState = initialState;
    calculateDiff();

    m_isInit = true;

    return true;
}

/**
 * @brief check if the pp1-energy-value is supported by the cpu
 *
 * @return false, if sandy-bridge or ivy-bridge cpu, else true
 */
bool
Rapl::checkPP1()
{
    const uint32_t eax_input = 1;
    uint32_t eax;
    __asm__("cpuid;" : "=a"(eax) : "0"(eax_input) : "%ebx","%ecx","%edx");

    const uint32_t cpuType = eax & SIGNATURE_MASK;
    if(cpuType == SANDYBRIDGE_E
            || cpuType == IVYBRIDGE_E)
    {
        return false;
    }

    return true;
}

/**
 * @brief open msr-file for the specific thread
 *
 * @return false, if file doesn't exist or can not be opened, else true
 */
bool
Rapl::openMSR()
{
    const std::string path = "/dev/cpu/" + std::to_string(m_threadId) + "/msr";
    m_fd = open(path.c_str(), O_RDONLY);
    if(m_fd <= 0)
    {
        if(errno == EIO)
        {
            LOG_ERROR("CPU doesn't support MSRs");
            return false;
        }
        else
        {
            LOG_ERROR("Failed to open path:" + path);
            return false;
        }
    }

    return true;
}

/**
 * @brief read a single value from the msr-file
 *
 * @param offset value-specific offset
 *
 * @return requeste value, if successfull, else 0
 */
uint64_t
Rapl::readMSR(const int32_t offset)
{
    uint64_t data;
    if(pread(m_fd, &data, sizeof(data), offset) != sizeof(data))
    {
        LOG_ERROR("can not read MSR");
        return 0;
    }

    return data;
}

/**
 * @brief get new data from rapl and calculate diff the the last call of this function
 *
 * @return new diff-data
 */
RaplDiff
Rapl::calculateDiff()
{
    RaplState state;

    // read data from msr
    state.pkg = readMSR(MSR_PKG_ENERGY_STATUS);
    state.pp0 = readMSR(MSR_PP0_ENERGY_STATUS);
    state.dram = readMSR(MSR_DRAM_ENERGY_STATUS);
    if(m_info.supportPP1) {
        state.pp1 = readMSR(MSR_PP1_ENERGY_STATUS);
    }
    state.timeStamp = std::chrono::system_clock::now();

    // create diff to last run
    RaplDiff diff;
    diff.pkgDiff = m_info.energy_units * static_cast<double>(state.pkg - m_lastState.pkg);
    diff.pp0Diff = m_info.energy_units * static_cast<double>(state.pp0 - m_lastState.pp0);
    diff.pp1Diff = m_info.energy_units * static_cast<double>(state.pp1 - m_lastState.pp1);
    diff.dramDiff = m_info.energy_units * static_cast<double>(state.dram - m_lastState.dram);

    // calculate time-difference to last run and convert it into seconds
    uint64_t nanoSec = std::chrono::duration_cast<chronoNanoSec>(state.timeStamp -
                                                                 m_lastState.timeStamp).count();
    const double nanoSecPerSec = 1000000000.0;
    diff.time = static_cast<double>(nanoSec) / nanoSecPerSec;

    // calculate average power-consumption per second
    diff.pkgAvg = static_cast<double>(diff.pkgDiff) / diff.time;
    diff.pp0Avg = static_cast<double>(diff.pp0Diff) / diff.time;
    diff.pp1Avg = static_cast<double>(diff.pp1Diff) / diff.time;
    diff.dramAvg = static_cast<double>(diff.dramDiff) / diff.time;

    // update internal state
    m_lastState = state;

    return diff;
}

/**
 * @brief get global info-data
 *
 * @return return rapl-info
 */
RaplInfo
Rapl::getInfo() const
{
    return m_info;
}

}
}
