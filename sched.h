#ifndef __SCHED_H__
#define __SCHED_H__

#include <vector>
#include <mutex>

#include "task.h"

using std::vector;
using std::mutex;

class CPU;
class PerfDomain;

class Scheduler
{
protected:
    // EnergyModels *energyModels;
    vector<PerfDomain *> *perfDomains;
    mutex schedLock;
    virtual CPU *findNextCpu(Task *t) = 0;
    // 直接拿队首
    virtual Task *findTaskToSched(CPU *cpu) = 0;
    virtual CPU *schedTask(Task *t) = 0;

public:
    Scheduler() {}
    Scheduler(vector<PerfDomain *> *pds): perfDomains(pds) {}
    // 调度新来的task
    virtual CPU *SchedNewTask(Task *t) = 0;
    // 调度该cpu队首task
    virtual CPU *SchedCpu(CPU *cpu) = 0;
};

#endif