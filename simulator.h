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

    const uint32_t NUM_LITTEL_CORE = 0;
    const uint32_t NUM_MIDDLE_CORE = 0;
    const uint32_t NUM_BIG_CORE = 1;
    const string LittleCorePath = "./cpu-models/LittleCoreFreq.csv";
    const string MiddleCorePath = "./cpu-models/MiddleCoreFreq.csv";
    const string BigCorePath = "./cpu-models/BigCoreFreq.csv";
    const string taskTestPath = "./task-tests/taskSample.csv";

    static uint64_t getCurrentTimeReal();
    void passSchedulerToCPU(Scheduler *);
    void inputTasks(const string& path);
    void startAllocTask(vector<Task*> &TaskList);
public:
    static const uint32_t MAX_CAP;
    static bool finishFlag;
    static const uint64_t checkPeriod;

    Simulator();
    void Run();
    static uint64_t GetCurrentTime();
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
};

#endif