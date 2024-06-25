#include "simulator.h"
#include <string>

Simultor::Simultor() {
    uint32_t i;
    EnergyModel *littleCore, *middleCore, *bigCore;

    /* Init Energy Models */
    littleCore = new EnergyModel(CPU_LITTLE, LittleCoreFreq);
    middleCore = new EnergyModel(CPU_MIDDLE, MiddleCoreFreq);
    bigCore = new EnergyModel(CPU_BIG, BigCoreFreq);
    energyModels = new EnergyModels(littleCore, middleCore, bigCore);

    /* Construct Perf Domain and CPU */
    this->perfDomains.push_back(new PerfDomain(NUM_LITTEL_CORE, littleCore));
    this->perfDomains.push_back(new PerfDomain(NUM_MIDDLE_CORE, middleCore));
    this->perfDomains.push_back(new PerfDomain(NUM_BIG_CORE, bigCore));
    for (auto p: this->perfDomains) {
        for (auto c: p->GetCPUS()) {
            this->cpus.push_back(c);
        }
    }

    /* Init Scheduler */
    scheduler = new Scheduler();
}

void Simultor::run() {
    for (auto c: this->cpus) {
        c->Run();
    }
}