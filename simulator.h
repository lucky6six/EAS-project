#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include <vector>
#include <cstdint>

#include "cpu.h"
#include "sched.h"

using std::vector;

class Simultor {
private:
    EnergyModels *energyModels;
    Scheduler *scheduler;
    vector <PerfDomain*> perfDomains;
    vector <CPU*> cpus;
    CPUType type;
    static uint64_t startTime; /* us */
    static uint64_t totalPeriod; /* us */

    const uint32_t NUM_CPUS = 8;
    const uint32_t NUM_CORE_TYPES = 3;
    const uint32_t NUM_LITTEL_CORE = 4;
    const uint32_t NUM_MIDDLE_CORE = 3;
    const uint32_t NUM_BIG_CORE = 1;
    const string LittleCoreFreq = "../cpu-model/LittleCoreFreq.csv";
    const string MiddleCoreFreq = "../cpu-model/MiddleCoreFreq.csv";
    const string BigCoreFreq = "../cpu-model/BigCoreFreq.csv";

    static uint64_t getCurrentTimeReal();
public:
    static const uint32_t MAX_CAP;

    Simultor();
    void Run();
    static uint64_t GetCurrentTime();
};

#endif