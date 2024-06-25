#include "simulator.h"

Simultor::Simultor() {
    uint32_t i;
    EnergyModel *littleCore, *middleCore, *bigCore;

    /* Init Energy Models */
    littleCore = new EnergyModel(CPU_LITTLE);
    middleCore = new EnergyModel(CPU_MIDDLE);
    bigCore = new EnergyModel(CPU_BIG);
    energyModels = new EnergyModels(littleCore, middleCore, bigCore);

    /* Construct Perf Domain and CPU */
    this->perfDomains.push_back(new PerfDomain(NUM_LITTEL_CORE, littleCore));
    this->perfDomains.push_back(new PerfDomain(NUM_MIDDLE_CORE, middleCore));
    this->perfDomains.push_back(new PerfDomain(NUM_BIG_CORE, bigCore));

    /* Init Scheduler */
    scheduler = new Scheduler();
}