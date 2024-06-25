#ifndef __SCHED_OPT_H__
#define __SCHED_OPT_H__

#include "sched.h"

class OptScheduler : public Scheduler
{
protected:
    CPU *schedTask(Task *t) override;
    CPU *findNextCpu(Task *t) override;
    Task *findTaskToSched(CPU *cpu) override;
public:
    OptScheduler(vector<PerfDomain *> *pds) : Scheduler(pds) {}
    CPU *SchedNewTask(Task *t) override;
    CPU *SchedCpu(CPU *cpu) override;
};

#endif