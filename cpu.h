#ifndef __CPU_H__
#define __CPU_H__

#include <vector>
#include <cstdint>
#include <thread>
#include <queue>

#include "task.h"

using std::vector;
using std::thread;
using std::queue;

class EnergyModel;
struct CPUFreq;
class CPU {
private:
    static uint32_t cpusid;
    static uint64_t interruptTime; /* ms */
    uint32_t id;
    EnergyModel *energyModel; /** Which EnergyModel it belongs to */
    thread t;
    queue<Task*> tasksQueue;
    CPUFreq *curFreq;

    void execTask(Task *);
public:
    CPU() {}
    CPU(EnergyModel *energyModel);
    void test();
    void Run();
    Task* PopTask();
    void AddTask(Task *);
};

struct CPUFreq {
    uint32_t freq;
    double power;
    uint32_t capacity;
};

enum CPUType {
    CPU_LITTLE,
    CPU_MIDDLE,
    CPU_BIG
};

class EnergyModel {
private:
    uint32_t num;
    vector<CPUFreq> cpufreqs;
    enum CPUType type;
public:
    EnergyModel() {}

    EnergyModel(enum CPUType type);
};

class PerfDomain {
private:
    vector<CPU*> cpus;
    EnergyModel *energyModel; /* One PerfDomain contains only one type of CPU */
    CPUFreq *curFreq;
public:
    PerfDomain() {}
    PerfDomain(uint32_t cpuNum, EnergyModel *energyModel);
    vector<CPU*> GetCPUS();
};

class EnergyModels {
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