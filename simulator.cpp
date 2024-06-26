#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <map>
#include <algorithm>

#include "sched.h"
#include "sched-eas.h"
#include "sched-opt.h"
#include "simulator.h"

uint64_t Simulator::startTime = 0;
uint32_t const Simulator::MAX_CAP = 1024;
bool Simulator::finishFlag = false;
uint64_t const Simulator::checkPeriod = 100;

using std::map;

/* Remove *space* and *"* on both sides */
std::string trim(const std::string& str) {
    std::string::const_iterator it = str.begin();
    while (it != str.end() && (std::isspace(*it) || *it == '"')) {
        it++;
    }

    std::string::const_reverse_iterator rit = str.rbegin();
    while (rit.base() != it && (std::isspace(*rit) || *rit == '"')) {
        rit++;
    }

    return std::string(it, rit.base());
}

void Simulator::configSimulator()
{
    std::map<std::string, std::string> configMap;
    std::string line;
    std::string key, value;

    std::ifstream configFile(this->configFile);
    std::cout << "Reading config file ..." << std::endl;
    if (!configFile.is_open()) {
        std::cout << "Can not open config file " << this->configFile << ", using default configuartion" << std::endl;
        goto out_report_config;
    }

    while (std::getline(configFile, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            key = trim(line.substr(0, pos));
            value = trim(line.substr(pos + 1));
            configMap[key] = value;
        }
    }

    configFile.close();
    this->litteCoreNum = std::stoi(configMap["LITTLE_CORE_NUM"]);
    this->middleCoreNum = std::stoi(configMap["MIDDLE_CORE_NUM"]);
    this->bigCoreNum = std::stoi(configMap["BIG_CORE_NUM"]);

    this->littleCorePath = configMap["LITTLE_CORE_PATH"];
    this->middleCorePath = configMap["MIDDLE_CORE_PATH"];
    this->bigCorePath = configMap["BIG_CORE_PATH"];

    this->taskTestPath = configMap["TEST_SAMPLE_PATH"];

out_report_config:
    std::cout << "|-- LITTLE_CORE_NUM: " << this->litteCoreNum << std::endl;
    std::cout << "|-- MIDDLE_CORE_NUM: " << this->middleCoreNum << std::endl;
    std::cout << "|-- BIG_CORE_NUM: " << this->bigCoreNum << std::endl;
    std::cout << "|-- LITTLE_CORE_PATH: " << this->littleCorePath << std::endl;
    std::cout << "|-- MIDDLE_CORE_PATH: " << this->middleCorePath << std::endl;
    std::cout << "|-- BIG_CORE_PATH: " << this->bigCorePath << std::endl;
    std::cout << "|-- TEST_SAMPLE_PATH: " << this->taskTestPath << std::endl;
    std::cout << "*************** Config Done! ***************\n" << std::endl;
}


Simulator::Simulator()
{
    uint32_t i;
    EnergyModel *littleCore, *middleCore, *bigCore;

    this->configSimulator();

    /* Init Energy Models */
    littleCore = new EnergyModel(CPU_LITTLE, littleCorePath);
    middleCore = new EnergyModel(CPU_MIDDLE, middleCorePath);
    bigCore = new EnergyModel(CPU_BIG, bigCorePath);
    energyModels = new EnergyModels(littleCore, middleCore, bigCore);

    /* Construct Perf Domain and CPU */
    printf("Build Litte-Core PD\n");
    this->perfDomains.push_back(new PerfDomain(litteCoreNum, littleCore));
    printf("Build Middle-Core PD\n");
    this->perfDomains.push_back(new PerfDomain(middleCoreNum, middleCore));
    printf("Build Big-Core PD\n");
    this->perfDomains.push_back(new PerfDomain(bigCoreNum, bigCore));
    printf("\n");
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

void Simulator::startAssignTask(vector<Task*> &TaskList) {
    Simulator* curSim = this;

    this->allocTaskThread = thread([&TaskList, curSim] {
        std::cout << "Start startAllocTask" << std::endl;
        while(!Simulator::finishFlag) {
            if (!TaskList.empty()) {
                // Assign Task to CPU
                for (auto taskIt = TaskList.begin(); taskIt != TaskList.end(); ++taskIt) {
                    auto curTime = curSim->GetCurrentTime();
                    if (curTime >= (*taskIt)->GetArrivalTime()) {
                        auto targetCPU = curSim->scheduler->SchedNewTask(*taskIt);
                        if (targetCPU != nullptr) {
                            // Task assigned successfully, just erare it
                            printf("New task %p %d\n", *taskIt, (*taskIt)->id);
                            TaskList.erase(taskIt);
                            break;
                        }
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
    this->startAssignTask(this->taskList);
}

void Simulator::passSchedulerToCPU(Scheduler *sched)
{
    for (auto c: this->cpus) {
        c->SetScheduler(sched);
    }
}

Simulator::~Simulator()
{
    if(allocTaskThread.joinable()) {
        allocTaskThread.join();
    }

    delete energyModels;

    delete scheduler;

    for (auto* perfDomain : perfDomains) {
        delete perfDomain;
    }
    perfDomains.clear();

    for (auto* cpu : cpus) {
        delete cpu;
    }
    cpus.clear();

    for (auto* task : taskList) {
        delete task;
    }
    taskList.clear();
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
    std::cout << "Total Power: " << Statistics::totalPower << " uJ" << std::endl;
}

void Statistics::ReportTotalRuntime()
{
    std::cout << "Total Runtime: " << Statistics::totalRuntime / 1000 << " ms" << std::endl;
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
    uint64_t totalTaskNum = Statistics::finishTasks.size();
    for (auto task: Statistics::finishTasks) {
        totalWaitTime += task->GetTotalWaitTime();
    }

    std::cout << "Average Total Wait Time: " <<
        totalWaitTime / 1000 / totalTaskNum << " ms per task " << std::endl;

    std::sort(Statistics::finishTasks.begin(), Statistics::finishTasks.end(), [](Task *a, Task *b) {
        return a->GetTaskId() < b->GetTaskId();
    });

    for (auto task: Statistics::finishTasks) {
        std::cout << "|-- " << "Task " << task->GetTaskId() << " Wait Time: " <<
            task->GetTotalWaitTime() / 1000 << " ms" << std::endl;
    }
}

void Statistics::ReportAll()
{
    std::cout << "\n*************** Report All ***************\n";
    ReportTotalPower();
    ReportTotalRuntime();
    ReportDelayTaskNum();
    ReportTotalWaitTime();
    ClearFinishTasks();
}

void Statistics::ClearFinishTasks() {
    for(auto task : finishTasks) {
        delete task;
    }
    finishTasks.clear();
};
