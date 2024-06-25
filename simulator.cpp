#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>

#include "sched.h"
#include "sched-eas.h"
#include "sched-opt.h"
#include "simulator.h"

uint64_t Simulator::startTime = 0;
uint32_t const Simulator::MAX_CAP = 1024;
bool Simulator::finishFlag = false;

Simulator::Simulator()
{
    uint32_t i;
    EnergyModel *littleCore, *middleCore, *bigCore;

    /* Init Energy Models */
    littleCore = new EnergyModel(CPU_LITTLE, LittleCorePath);
    middleCore = new EnergyModel(CPU_MIDDLE, MiddleCorePath);
    bigCore = new EnergyModel(CPU_BIG, BigCorePath);
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

    /* Create EAS Scheduler */
    scheduler = new EasScheduler(&this->perfDomains);

    this->passSchedulerToCPU(scheduler);
}

/* Return microseconds */
uint64_t Simulator::getCurrentTimeReal()
{
    return std::chrono::duration_cast<std::chrono::microseconds>
        (std::chrono::steady_clock::now().time_since_epoch()).count();
}

/* Return microseconds */
uint64_t Simulator::GetCurrentTime()
{
    auto currentTimeReal = getCurrentTimeReal();
    auto currentTime = currentTimeReal - startTime;

    return currentTime;
}

vector<Task*> Simulator::InputTasks(const string& path)
{
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

void Simulator::Run()
{
    Simulator::startTime = getCurrentTimeReal();
    Simulator::finishFlag = false;
    for (auto c: this->cpus) {
        c->Run();
    }
}

void Simulator::passSchedulerToCPU(Scheduler *sched)
{
    for (auto c: this->cpus) {
        c->SetScheduler(sched);
    }
}

uint64_t Statistics::totalRuntime = 0;
double Statistics::totalPower = 0;
vector<Task*> Statistics::finishTasks = {};
uint32_t Statistics::totalTaskNum = 0;

void Statistics::AddToFinishList(Task *task)
{
    Statistics::finishTasks.push_back(task);
    if (Statistics::finishTasks.size() == Statistics::totalTaskNum) {
        ReportTotalPower();
        ReportTotalRuntime();
        ReportDelayTaskNum();
        ReportTotalWaitTime();
    }
}

void Statistics::AddTotalPower(double power)
{
    Statistics::totalPower += power;
}

void Statistics::ReportTotalPower()
{
    std::cout << "Total Power: " << Statistics::totalPower << " mW" << std::endl;
}

void Statistics::ReportTotalRuntime()
{
    std::cout << "Total Runtime: " << Statistics::totalRuntime << " us" << std::endl;
}

void Statistics::ReportDelayTaskNum()
{
    uint32_t delayTaskNum = 0;
    for (auto task: Statistics::finishTasks) {
        if (task->IsTaskDelay()) {
            delayTaskNum++;
        }
    }
    std::cout << "Delay Task Number: " << delayTaskNum << std::endl;
}

void Statistics::ReportTotalWaitTime()
{
    uint64_t totalWaitTime = 0;
    /* TODO */
    std::cout << "Total Wait Time: " << totalWaitTime << " us" << std::endl;
}