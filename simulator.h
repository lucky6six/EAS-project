#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include <vector>
#include <cstdint>

#include "cpu.h"
#include "sched.h"

using std::vector;

class Simulator
{
private:
    EnergyModels *energyModels;
    Scheduler *scheduler;
    vector <PerfDomain*> perfDomains;
    vector <CPU*> cpus;
    vector <Task*> taskList;
    static uint64_t startTime; /* us */
    thread allocTaskThread;

    uint32_t litteCoreNum = 3;
    uint32_t middleCoreNum = 4;
    uint32_t bigCoreNum = 1;
    string littleCorePath = "./cpu-models/LittleCoreFreq.csv";
    string middleCorePath = "./cpu-models/MiddleCoreFreq.csv";
    string bigCorePath = "./cpu-models/BigCoreFreq.csv";
    string taskTestPath = "./task-tests/taskSample.csv";
    const string configFile = "./simulator.config";

    static uint64_t getCurrentTimeReal();
    void passSchedulerToCPU(Scheduler *);
    void inputTasks(const string& path);
    void startAssignTask(vector<Task*> &TaskList);
    void configSimulator();
public:
    static const uint32_t MAX_CAP;
    static bool finishFlag;
    static const uint64_t checkPeriod;

    Simulator();
    void Run();
    static uint64_t GetCurrentTime();
    ~Simulator();
};

class Statistics
{
public:
    static vector<Task*> finishTasks;
    static uint64_t totalRuntime; /* us */
    static double totalPower; /* mW */
    static uint64_t totalWaitTime; /* us */
    static uint32_t totalTaskNum;

    static void AddToFinishList(Task *);
    static void AddTotalPower(double);
    static void ReportTotalPower();
    static void ReportTotalRuntime();
    static void ReportDelayTaskNum();
    static void ReportTotalWaitTime();
    static void ReportAll();
    static void ClearFinishTasks();    
};

#endif
