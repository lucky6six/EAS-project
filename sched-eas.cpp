#include "sched-eas.h"
#include "cpu.h"
#include "sched.h"
#include "task.h"
#include "simulator.h"

// 同时考虑频点更新,但不实际更新频点，在外层调度是更新
CPU *EasScheduler::findNextCpu(Task *t)
{
    CPU *targetCPU = nullptr;
    CPUFreq *curFreq;
    CPU *targetCPUinPd;
    // 当前pd的最小cpu cap
    uint32_t minCPUCapacity;
    // 迁移的实现为先pop task，再类似新task进入系统一样部署
    // 这里是将t部署到targetCPU上需要增加的能耗开销。
    double cur_min_power = 10000; // 极大值
    double powerOld = 0;
    double powerNew = 0;
    uint32_t capSumInPd = 0;
    vector<CPUFreq> *EM;
    CPUFreq *expectFreq;
    uint32_t expectCapacity;

    // 遍历每个pd，评估最低能效开销
    for (auto pd : *perfDomains) {
        curFreq = pd->GetFreq();
        minCPUCapacity = curFreq->capacity;
        capSumInPd = 0;
        EM = pd->GetEM();
        // 遍历pd中所有cpu，找到capacity最小的cpu
        for (auto cpu : pd->GetCPUS()) {
            capSumInPd += cpu->GetCapacity();
            if (cpu->GetCapacity() < minCPUCapacity) {
                minCPUCapacity = cpu->GetCapacity();
                targetCPUinPd = cpu;
            }
        }
        // 计算原能耗开销
        powerOld = static_cast<double>(capSumInPd) / curFreq->capacity * curFreq->power;
        //  计算部署到targetCPUinPd上的能耗开销
        //  部署后cap
        expectCapacity = targetCPUinPd->GetCapacity() + t->GetCapacity();
        // 是否要提升频点
        if (expectCapacity > curFreq->capacity) {
            // 找到预期频点
            expectFreq = pd->getSuitableFreq(expectCapacity);
            // 当expectFreq为nullptr时，说明没有合适的频点，不考虑这个pd
            if (expectFreq == nullptr) {
                continue;
            }
        // 计算能耗开销
        }
        powerNew = static_cast<double>(capSumInPd + t->GetCapacity()) /
            expectFreq->capacity * expectFreq->power;
        double power = powerNew - powerOld;
        if (power < cur_min_power) {
            cur_min_power = power;
            targetCPU = targetCPUinPd;
        }
    }
    return targetCPU;

}

Task *EasScheduler::findTaskToSched(CPU *cpu)
{
    return cpu->TopTask();
}

CPU *EasScheduler::schedTask(Task *t)
{
    CPU *c = findNextCpu(t);
    c->AddTask(t);
    c->GetPerfDomain()->RebuildPerfDomain();
    return c;
}

CPU *EasScheduler::SchedNewTask(Task *t)
{
    std::lock_guard<mutex> guard(this->schedLock);
    CPU *c = findNextCpu(t);
    c->AddTask(t);
    c->GetPerfDomain()->RebuildPerfDomain();
    return c;
}

// 对传入cpu的第一个task（正在允许的task）进行调度，返回调度后的cpu
// task迁移已在函数内完成
CPU *EasScheduler::SchedCpu(CPU *cpu)
{
    std::lock_guard<mutex> guard(this->schedLock);
    Task *t = cpu->PopTask();
    CPU *toCPU;

    cpu->GetPerfDomain()->RebuildPerfDomain();
    if (t->IsTaskFinish()) {
        Statistics::AddToFinishList(t);
        return nullptr;
    }
    toCPU = schedTask(t);
    return toCPU;
}
