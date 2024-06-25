#include <iostream>
#include <chrono>
#include "cpu.h"
#include <iostream>
#include <fstream>
#include <sstream>

using std::ifstream;
using std::istringstream;

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

EnergyModel::EnergyModel(enum CPUType type, const string& path) {
    this->type = type;
    ifstream file(path);
    string line;
    while (getline(file, line)) {
        istringstream ss(line);
        string cell;
        vector<string> row;

        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        CPUFreq cpuFreq = {static_cast<uint32_t>(std::stoul(row[0])), std::stod(row[1]), static_cast<uint32_t>(std::stoul(row[2]))};
        this->cpufreqs.push_back(cpuFreq);
    }
    this->num = this->cpufreqs.size();
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