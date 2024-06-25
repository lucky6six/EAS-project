#include <iostream>
#include "cpu.h"

uint32_t CPU::cpusid = 0;

CPU::CPU(EnergyModel *e) {
    this->energyModel = e;
    this->id = cpusid++;
}

void CPU::test()  {
    std::cout << "CPU::test()" << std::endl;
}

EnergyModel::EnergyModel(CPUType type) {
    this->type = type;
    /** TODO: Add CPUFreq (using util) */
}

PerfDomain::PerfDomain(uint32_t cpuNum, EnergyModel *energyModel) {
    this->energyModel = energyModel;
    for (uint32_t i = 0; i < cpuNum; i++) {
        CPU *cpu = new CPU(energyModel);
        this->cpus.push_back(cpu);
    }
}