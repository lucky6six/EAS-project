#ifndef __CPU_H__
#define __CPU_H__

#include <vector>
#include <cstdint>

using std::vector;

class EnergyModel;
class CPU {
private:
    static uint32_t cpusid;
    uint32_t id;
    EnergyModel *energyModel; /** Which EnergyModel it belongs to */
public:
    CPU() {}
    CPU(EnergyModel *energyModel);
    void test();
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