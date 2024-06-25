#ifndef __CPU_H__
#define __CPU_H__

#include <cstdint>
#include <queue>
#include <thread>
#include <vector>

#include "task.h"
#include <string>

using std::queue;
using std::string;
using std::thread;
using std::vector;

struct CPUFreq
{
    uint32_t freq;
    double power;
    uint32_t capacity;
};

class EnergyModel;
struct CPUFreq;
class CPU
{
private:
    static uint32_t cpusid;
    static uint64_t interruptTime; /* ms */
    uint32_t id;
    EnergyModel *energyModel; /** Which EnergyModel it belongs to */
    // TODO:初始化时设置该cpu位于哪个PerfDomain
    PerfDomain *perfDomain; /** Which PerfDomain it belongs to */
    thread t;
    queue<Task *> tasksQueue;
    CPUFreq *curFreq;  // 当前的频点（freq,pwr,cap）
    uint32_t capacity; // 当前的算力负载

    void execTask(Task *);

public:
    CPU() {}
    CPU(EnergyModel *energyModel);
    void test();
    void Run();
    Task *PopTask();
    void AddTask(Task *);
    Task *TopTask(); // 返回队列头部的任务
    void reBuildCapacity();
    uint32_t GetCapacity();
    PerfDomain *GetPD();
    CPUFreq *GetFreq();
    void SetFreq(CPUFreq *);
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
    vector<CPUFreq> cpufreqs;
    string path;
    enum CPUType type;

public:
    EnergyModel() {}

    EnergyModel(enum CPUType type);

    EnergyModel(enum CPUType type, const string &path);

    vector<CPUFreq> *GetFreqs();
};

class PerfDomain
{
private:
    EnergyModel *energyModel; /* One PerfDomain contains only one type of CPU */
    vector<CPU *> cpus;
    CPUFreq *curFreq;

public:
    PerfDomain() {}
    PerfDomain(uint32_t cpuNum, EnergyModel *energyModel);
    vector<CPU *> GetCPUS();
    // 获取当前频点
    CPUFreq *GetFreq();
    // 获取可调整频点列表
    vector<CPUFreq> *GetEM();
    void ReBuildPerfDomain();
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