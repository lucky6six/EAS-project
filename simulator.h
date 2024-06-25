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
    uint64_t startTime;
    uint64_t totalTime;

    const uint32_t NUM_CPUS = 8;
    const uint32_t NUM_CORE_TYPES = 3;
    const uint32_t NUM_LITTEL_CORE = 4;
    const uint32_t NUM_MIDDLE_CORE = 3;
    const uint32_t NUM_BIG_CORE = 1;
public:
    Simultor();
    void run();
};

#endif