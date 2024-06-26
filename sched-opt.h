#ifndef __SCHED_OPT_H__
#define __SCHED_OPT_H__

#include <queue>
#include "sched.h"

class OptScheduler : public Scheduler
{
private:
    uint32_t historyCapacity; 
    uint32_t currentCapacity;
    bool historyEnable;
    std::queue<Task*> waitlist;
protected:
    CPU *schedTask(Task *t) override;
    CPU *findNextCpu(Task *t) override;
    Task *findTaskToSched(CPU *cpu) override;
public:
    OptScheduler(vector<PerfDomain *> *pds);
    CPU *SchedNewTask(Task *t) override;
    CPU *SchedCpu(CPU *cpu) override;
};

#endif