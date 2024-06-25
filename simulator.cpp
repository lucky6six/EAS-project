#include "simulator.h"
#include <string>
#include <chrono>

uint64_t Simultor::startTime = 0;
uint64_t Simultor::totalPeriod = 0;
uint32_t const Simultor::MAX_CAP = 1024;

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

/* Return microseconds */
uint64_t Simultor::getCurrentTimeReal() {
    return std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::steady_clock::now().time_since_epoch()).count();

}

/* Return microseconds */
uint64_t Simultor::GetCurrentTime() {
    auto currentTimeReal = getCurrentTimeReal();
    auto currentTime = currentTimeReal - startTime;

    return currentTime;
}

void Simultor::Run() {
    Simultor::startTime = getCurrentTimeReal();
    for (auto c: this->cpus) {
        c->Run();
    }
}