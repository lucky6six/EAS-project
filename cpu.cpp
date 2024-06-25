#include <iostream>
#include <chrono>
#include "cpu.h"

uint32_t CPU::cpusid = 0;
uint64_t CPU::interruptTime = 20;

CPU::CPU(EnergyModel *e) {
    this->energyModel = e;
    this->id = cpusid++;
}

void CPU::test() {
    std::cout << "CPU::test()" << std::endl;
}

void CPU::execTask(Task *task) {
    auto currenTime = std::chrono::steady_clock::now();
    /* Calculate cost time & capacity */
}

void CPU::Run() {
    auto curCPU = this;
    this->t = thread([curCPU] {
        while(true) {
            if (!curCPU->tasksQueue.empty()) {
                Task *task = curCPU->tasksQueue.front();
                curCPU->execTask(task);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(CPU::interruptTime));
            /* TODO:Ask scheduler for sched() */
        }
    });
    this->t.join();
}

Task* CPU::PopTask() {
    Task *task = this->tasksQueue.front();
    this->tasksQueue.pop();
    return task;
}

void CPU::AddTask(Task *t) {
    this->tasksQueue.push(t);
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

vector<CPU*> PerfDomain::GetCPUS() {
    return this->cpus;
}