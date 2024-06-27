#ifndef __SCHED_OPT_H__
#define __SCHED_OPT_H__

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include "sched.h"

using std::queue;
using std::unordered_map;
using std::unordered_set;

class OptScheduler : public Scheduler
{
private:
    uint32_t historyCapacity=0; 
    uint32_t currentCapacity=0;
    uint32_t waitCapacity=0;
    bool historyEnable=0;
    CPU * mainCPU;
    unordered_map<CPU*,queue<uint32_t>*> historyCapacities;
    unordered_set<uint32_t>waitlist;
    void updateHistory(CPU *cpu);
protected:
    CPU *schedTask(Task *t) override;
    CPU *findNextCpu(Task *t) override;
    Task *findTaskToSched(CPU *cpu) override;
public:
    OptScheduler(vector<PerfDomain *> *pds);
    ~OptScheduler();
    CPU *SchedNewTask(Task *t) override;
    CPU *SchedCpu(CPU *cpu) override;
};

#endif