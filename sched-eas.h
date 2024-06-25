#ifndef __SCHED_EAS_H__
#define __SCHED_EAS_H__

#include "sched.h"

class EasScheduler : public Scheduler
{
protected:
    CPU *schedTask(Task *t) override;
    CPU *findNextCpu(Task *t) override;
    Task *findTaskToSched(CPU *cpu) override;
public:
    EasScheduler(vector<PerfDomain *> *pds) : Scheduler(pds) {}
    // 用eas找最佳cpu
    CPU *SchedNewTask(Task *t) override;
    CPU *SchedCpu(CPU *cpu) override;
};

#endif