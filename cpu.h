#ifndef __CPU_H__
#define __CPU_H__

#include <cstdint>
#include <queue>
#include <thread>
#include <vector>
#include <string>

#include "task.h"

using std::queue;
using std::string;
using std::thread;
using std::vector;

class EnergyModel;
class PerfDomain;
class Scheduler;
struct CPUFreq;
struct CPUFreq
{
    uint32_t freq;
    double power;
    uint32_t capacity;
};

class CPU
{
private:
    static uint32_t cpusid;
    uint32_t id;
    EnergyModel *energyModel; /** Which EnergyModel it belongs to */
    // TODO:初始化时设置该cpu位于哪个PerfDomain
    PerfDomain *perfDomain; /** Which PerfDomain it belongs to */
    thread cpuThread;
    queue<Task *> tasksQueue;
    CPUFreq *curCPUFreq;  // 当前的频点（freq,pwr,cap）
    uint32_t capacity; // 当前的算力负载
    Scheduler *scheduler;

    static void execTask(CPU *, Task *);
    uint32_t CalcTotalCapacity();
public:
    static uint64_t timeSlice; /* us */

    CPU() {}
    CPU(PerfDomain *perfDomain, EnergyModel *energyModel);
    void Run();
    Task *PopTask();
    void AddTask(Task *);
    uint32_t GetCurCapacity();
    Task *TopTask(); // 返回队列头部的任务
    void RebuildCapacity();
    uint32_t GetCapacity();
    PerfDomain *GetPerfDomain();
    CPUFreq *GetCPUFreq();
    void SetFreq(CPUFreq *);
    void SetScheduler(Scheduler *);
    static double GetCPUPower(CPU *);
    uint32_t GetCPUId() { return id; }
    queue<Task*>& GetTaskQueue() { return tasksQueue; }
    bool IsEmpty() { return tasksQueue.empty(); }

    ~CPU();
};

enum CPUType
{
    CPU_LITTLE,
    CPU_MIDDLE,
    CPU_BIG
};

class EnergyModel
{
private:
    uint32_t num;
    vector<CPUFreq*> cpufreqs;
    string path;
    enum CPUType type;

public:
    EnergyModel() {}

    EnergyModel(enum CPUType type);

    EnergyModel(enum CPUType type, string &path);

    vector<CPUFreq*> *GetFreqs();
};

class PerfDomain
{
private:
    EnergyModel *energyModel; /* One PerfDomain contains only one type of CPU */
    vector<CPU *> cpus;
    CPUFreq *curCPUFreq;

public:
    PerfDomain() {}
    PerfDomain(uint32_t cpuNum, EnergyModel *energyModel);
    vector<CPU *> GetCPUS();
    // 获取当前频点
    CPUFreq *GetCurCPUFreq();
    // 获取可调整频点列表
    vector<CPUFreq*> *GetEnergyModel();
    void RebuildPerfDomain();
    CPUFreq *getSuitableFreq(uint32_t expectCapacity);
};

class EnergyModels
{
private:
    EnergyModel *little;
    EnergyModel *middle;
    EnergyModel *big;

public:
    EnergyModels() {}
    EnergyModels(EnergyModel *l, EnergyModel *m, EnergyModel *b)
        : little(l), middle(m), big(b) {}
};

#endif