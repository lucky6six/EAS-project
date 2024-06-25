#ifndef __SCHED_H__
#define __SCHED_H__
#include "cpu.h"
#include "task.h"
#include <vector>

// TODO 全局energy大表 可以 from simulator
// EnergyModels *getEnergyModels()
// { /* code */
// }

// TODO 获取所有perfDomain的链表  可以from simulator的this->perfDomains
// 反正就是全局的pds
vector<PerfDomain *> *getAllPerfDomains()
{ /* code */
}

class Scheduler
{
    // EnergyModels *energyModels;
    vector<PerfDomain *> *pds;
    // 用eas找最佳cpu
    CPU *findEasCpu(Task *t);
    // 直接拿队首
    Task *findTaskToSched(CPU *cpu);

public:
    Scheduler();
    void init();
    // 调度新来的task
    CPU *schedNewTask(Task *t);
    // 调度该cpu队首task
    CPU *schedCpu(CPU *cpu);
    void test();
};

#endif