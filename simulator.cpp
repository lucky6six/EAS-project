#include "simulator.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
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

vector<Task*> Simultor::InputTasks(const string& path) {
    vector<Task*> TaskList;
    std::ifstream file(path);
    string line;
    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return TaskList;
    }

    // 跳过第一行
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        string cell;
        vector<string> row;

        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }
        Task* task = new Task(std::stoull(row[0]), std::stoull(row[1]), std::stoull(row[2]),
                              std::stoull(row[3]), std::stoull(row[4]), std::stoull(row[5]));
        TaskList.push_back(task);
    }
    return TaskList;
}

void Simultor::Run() {
    Simultor::startTime = getCurrentTimeReal();
    for (auto c: this->cpus) {
        c->Run();
    }
}