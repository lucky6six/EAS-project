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
uint64_t const Simulator::checkPeriod = 100;

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
    printf("Build Litte-Core PD\n");
    this->perfDomains.push_back(new PerfDomain(NUM_LITTEL_CORE, littleCore));
    printf("Build Middle-Core PD\n");
    this->perfDomains.push_back(new PerfDomain(NUM_MIDDLE_CORE, middleCore));
    printf("Build Big-Core PD\n");
    this->perfDomains.push_back(new PerfDomain(NUM_BIG_CORE, bigCore));
    for (auto p: this->perfDomains) {
        for (auto c: p->GetCPUS()) {
            this->cpus.push_back(c);
        }
    }

    /* Create EAS Scheduler */
    scheduler = new EasScheduler(&this->perfDomains);

    this->passSchedulerToCPU(scheduler);

    this->inputTasks(taskTestPath);
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

void Simulator::inputTasks(const string& path)
{
    std::ifstream file(path);
    string line;
    if (!file.is_open()) {
        std::cerr << "Failed to open file" << std::endl;
        return;
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
        taskList.push_back(task);
    }

    Statistics::totalTaskNum = taskList.size();
}

void Simulator::startAllocTask(vector<Task*> &TaskList) {
    Simulator* curSim = this;

    this->allocTaskThread = thread([&TaskList, curSim] {
        std::cout << "Start startAllocTask" << std::endl;
        while(!Simulator::finishFlag) {
            if (!TaskList.empty()) {
                // 分配给CPU
                for (auto taskIt = TaskList.begin(); taskIt != TaskList.end(); ++taskIt) {
                    auto curTime = curSim->GetCurrentTime();
                    if (curTime >= (*taskIt)->GetArrivalTime()) {
                        printf("task %p %d\n", *taskIt, (*taskIt)->id);
                        curSim->scheduler->SchedNewTask(*taskIt);
                        TaskList.erase(taskIt);
                        break;
                    }
                }
            }
        std::this_thread::sleep_for(std::chrono::microseconds(Simulator::checkPeriod));
    }});
    this->allocTaskThread.detach();
}

void Simulator::Run()
{
    Simulator::startTime = getCurrentTimeReal();
    Simulator::finishFlag = false;
    for (auto c: this->cpus) {
        c->Run();
    }
    this->startAllocTask(this->taskList);
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
        Simulator::finishFlag = true;
        Statistics::totalRuntime = Simulator::GetCurrentTime();
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
    std::cout << "TODO: Total Wait Time: " << totalWaitTime << " us" << std::endl;
}

void Statistics::ReportAll()
{
    ReportTotalPower();
    ReportTotalRuntime();
    ReportDelayTaskNum();
    ReportTotalWaitTime();
}