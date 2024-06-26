#include "sched-opt.h"
#include "cpu.h"
#include "sched.h"
#include "task.h"
#include "simulator.h"

OptScheduler::OptScheduler(vector<PerfDomain *> *pds) : Scheduler(pds) {
  historyEnable = 1;
  historyCapacity = 2500;
  currentCapacity = 0;
}

// 同时考虑频点更新,但不实际更新频点，在外层调度是更新
CPU *OptScheduler::findNextCpu(Task *t)
{
    CPU *targetCPU = nullptr;
    CPUFreq *curFreq;
    CPU *targetCPUinPd = nullptr;
    CPU *minCapCPU = nullptr;
    uint32_t minCap = 10000000;
    // 当前pd的最小cpu cap
    uint32_t minCPUCapacity;
    // 迁移的实现为先pop task，再类似新task进入系统一样部署
    // 这里是将t部署到targetCPU上需要增加的能耗开销。
    double cur_min_power = 10000; // 极大值
    double powerOld = 0;
    double powerNew = 0;
    uint32_t capSumInPd = 0;
    vector<CPUFreq *> *em;
    CPUFreq *expectFreq;
    uint32_t expectCapacity;

    // 遍历每个pd，评估最低能效开销
    for (auto pd : *perfDomains)
    {
        curFreq = pd->GetCurCPUFreq();
        minCPUCapacity = curFreq->capacity;
        capSumInPd = 0;
        em = pd->GetEnergyModel();
        // 遍历pd中所有cpu，找到capacity最小的cpu
        for (auto cpu : pd->GetCPUS())
        {
            capSumInPd += cpu->GetCapacity();
            if (cpu->GetCapacity() < minCap)
            {
                minCap = cpu->GetCapacity();
                minCapCPU = cpu;
            }
            if (cpu->GetCapacity() < minCPUCapacity)
            {
                minCPUCapacity = cpu->GetCapacity();
                targetCPUinPd = cpu;
            }
        }
        if (targetCPUinPd == nullptr)
        {
            /* Current PerfDomain: No Available CPU */
            continue;
        }
        // 计算原能耗开销
        powerOld = static_cast<double>(capSumInPd) / curFreq->capacity * curFreq->power;
        //  计算部署到targetCPUinPd上的能耗开销
        //  部署后cap
        expectCapacity = targetCPUinPd->GetCapacity() + t->GetCapacity();
        expectFreq = curFreq;
        // 是否要提升频点
        if (expectCapacity > curFreq->capacity)
        {
            // 找到预期频点
            expectFreq = pd->getSuitableFreq(expectCapacity);
            // 当expectFreq为nullptr时，说明没有合适的频点，不考虑这个pd
            if (expectFreq == nullptr)
            {
                continue;
            }
        }
        // 计算能耗开销
        powerNew = static_cast<double>(capSumInPd + t->GetCapacity()) /
                   expectFreq->capacity * expectFreq->power;
        double power = powerNew - powerOld;
        if (power < cur_min_power)
        {
            cur_min_power = power;
            targetCPU = targetCPUinPd;
        }
    }
    if (targetCPU == nullptr)
    {
        targetCPU = minCapCPU;
    }
    return targetCPU;
}

Task *OptScheduler::findTaskToSched(CPU *cpu)
{
    return cpu->TopTask();
}

CPU *OptScheduler::schedTask(Task *t)
{
    
    CPU *c = findNextCpu(t);
    if (c != nullptr)
    {
        c->AddTask(t);
        c->GetPerfDomain()->RebuildPerfDomain();
    }

    return c;
}

CPU *OptScheduler::SchedNewTask(Task *t)
{
    std::lock_guard<mutex> guard(this->schedLock);
    if(historyEnable && t->GetCapacity()+currentCapacity>historyCapacity){
      return nullptr;
    }
    CPU *c = findNextCpu(t);
    if (c != nullptr)
    {
        currentCapacity+= t->GetCapacity();
        c->AddTask(t);
        c->GetPerfDomain()->RebuildPerfDomain();
    }

    return c;
}

CPU *OptScheduler::SchedCpu(CPU *cpu)
{
    Task *t;
    CPU *toCPU = nullptr;
    if (cpu->IsEmpty())
    {
        return nullptr;
    }

    std::lock_guard<mutex> guard(this->schedLock);
    t = cpu->PopTask();
    cpu->GetPerfDomain()->RebuildPerfDomain();
    if (t->IsTaskFinish())
    {
        currentCapacity -= t->GetCapacity();
        Statistics::AddToFinishList(t);
        return nullptr;
    }
    toCPU = schedTask(t);

    return toCPU;
}
