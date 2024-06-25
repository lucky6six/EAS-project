#include "sched.h"
#include <iostream>

void Scheduler::test() { std::cout << "Scheduler::test" << std::endl; }

Scheduler::Scheduler()
{
  // energyModels = getEnergyModels();
  pds = getAllPerfDomains();
}

void Scheduler::init()
{
  // energyModels = getEnergyModels();
  pds = getAllPerfDomains();
}

// CPUFreq *getSuitableFreq(PerfDomain *pd, uint32_t expectCapacity)
// {
//   CPUFreq *ret = nullptr;
//   vector<CPUFreq> *EM = pd->GetEM();
//   for (auto it = EM->begin(); it != EM->end(); it++)
//   {
//     if (it->capacity >= expectCapacity)
//     {
//       ret = &(*it);
//       break;
//     }
//   }
//   return ret;
// }

// 同时考虑频点更新,但不实际更新频点，在外层调度是更新
CPU *Scheduler::findEasCpu(Task *t)
{
  CPU *targetCPU = nullptr;
  CPUFreq *curFreq;
  CPU *targetCPUinPd;
  // 当前pd的最小cpu cap
  uint32_t min_cpu_capacity;
  // 迁移的实现为先pop task，再类似新task进入系统一样部署
  // 这里是将t部署到targetCPU上需要增加的能耗开销。
  double cur_min_power = 10000; // 极大值
  double powerOld = 0;
  double powerNew = 0;
  uint32_t capSumInPd = 0;
  vector<CPUFreq> *EM;
  CPUFreq *expectFreq;
  // 遍历每个pd，评估最低能效开销
  for (auto pd : *pds)
  {
    curFreq = pd->GetFreq();
    min_cpu_capacity = curFreq->capacity;
    capSumInPd = 0;
    EM = pd->GetEM();
    // 遍历pd中所有cpu，找到capacity最小的cpu
    for (auto cpu : pd->GetCPUS())
    {
      capSumInPd += cpu->GetCapacity();
      if (cpu->GetCapacity() < min_cpu_capacity)
      {
        min_cpu_capacity = cpu->GetCapacity();
        targetCPUinPd = cpu;
      }
    }
    // 计算原能耗开销
    powerOld = capSumInPd / (curFreq->capacity) * (curFreq->power);
    //  计算部署到targetCPUinPd上的能耗开销
    //  部署后cap
    uint32_t expectCapacity = targetCPUinPd->GetCapacity() + t->getCapacity();
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
      // 计算能耗开销
    }
    powerNew = (capSumInPd + t->getCapacity()) / (expectFreq->capacity) * (expectFreq->power);
    double power = powerNew - powerOld;
    if (power < cur_min_power)
    {
      cur_min_power = power;
      targetCPU = targetCPUinPd;
    }
  }
  return targetCPU;
}

Task *Scheduler::findTaskToSched(CPU *cpu)
{
  return cpu->TopTask();
}

CPU *Scheduler::schedNewTask(Task *t)
{
  CPU *c = findEasCpu(t);
  c->AddTask(t);
  // c->reBuildCapacity();
  c->GetPD()->ReBuildPerfDomain();
  return c;
}

// 对传入cpu的第一个task（正在允许的task）进行调度，返回调度后的cpu
// task迁移已在函数内完成
CPU *Scheduler::schedCpu(CPU *cpu)
{
  Task *t = findTaskToSched(cpu);
  cpu->PopTask();
  // cpu->reBuildCapacity();
  cpu->GetPD()->ReBuildPerfDomain();
  CPU *toCPU = schedNewTask(t);
  return toCPU;
}
