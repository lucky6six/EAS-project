#include <chrono>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <iostream>

#include "cpu.h"
#include "sched.h"
#include "simulator.h"

using std::ifstream;
using std::istringstream;

uint32_t CPU::cpusid = 0;
uint64_t CPU::timeSlice = 20000; /* =20ms */

// 初始化 curCPUFreq -> minCPUFreq，perfDomain（belong_to），capacity（0）
CPU::CPU(PerfDomain *pd, EnergyModel *em)
{
    this->perfDomain = pd;
    this->energyModel = em;
    this->id = cpusid++;
    this->capacity = 0;
    this->curCPUFreq = em->GetFreqs()->front();
}

uint32_t CPU::GetCurCapacity()
{
    return this->curCPUFreq->capacity;
}

double CPU::GetCPUPower(CPU *cpu)
{
    return cpu->GetCPUFreq()->power;
}

void CPU::execTask(CPU *curCPU, Task *task)
{
    auto currentTime = Simulator::GetCurrentTime();
    double power;
    /* Calculate cost time & capacity */
    task->CheckDeadline(currentTime);
    task->UpdateWorkTime(curCPU->GetCurCapacity());
    /* Calculate power const */
    power = task->CalculateRatio(curCPU->GetCurCapacity()) * CPU::GetCPUPower(curCPU);
    Statistics::AddTotalPower(power);
}

void CPU::Run()
{
    auto curCPU = this;
    this->cpuThread = thread([curCPU]{
        std::cout << "cpu " << curCPU->GetCPUId() << " Start to Run" << std::endl;
        while(!Simulator::finishFlag) {
            if (!curCPU->tasksQueue.empty()) {
                Task *task = curCPU->tasksQueue.front();
                curCPU->execTask(curCPU, task);
                printf("task %d in cpu %d total_time: %lu\n", task->id, curCPU->GetCPUId(), task->GetTotalWorkTime());
            }
            std::this_thread::sleep_for(std::chrono::microseconds(CPU::timeSlice));
            curCPU->scheduler->SchedCpu(curCPU);
        }
    });
    this->cpuThread.detach();
}

Task *CPU::PopTask()
{
    Task *task = this->tasksQueue.front();
    this->tasksQueue.pop();
    return task;
}

void CPU::AddTask(Task *t)
{
    this->tasksQueue.push(t);
}

Task *CPU::TopTask()
{
    return this->tasksQueue.front();
}

uint32_t CPU::CalcTotalCapacity()
{
    queue<Task*> tmpQueue;
    uint32_t totalCap = 0;
    Task *task;

    /* We need to tranverse the tasksQueue, hence pop it out into tmpQueue */
    while (!this->tasksQueue.empty()) {
        task = this->tasksQueue.front();
        tasksQueue.pop();
        totalCap += task->GetCapacity();
        tmpQueue.push(task);
    }

    /* Reconstruct the tasksQueue */
    while (!tmpQueue.empty()) {
        task = tmpQueue.front();
        tmpQueue.pop();
        this->tasksQueue.push(task);
    }

    return totalCap;
}
// 重新计算task队列中的capacity和
void CPU::RebuildCapacity()
{
    this->capacity = this->CalcTotalCapacity();
    return;
}

uint32_t CPU::GetCapacity()
{
    return this->capacity;
}

PerfDomain *CPU::GetPerfDomain()
{
    return this->perfDomain;
}

CPUFreq *CPU::GetCPUFreq()
{
    return this->curCPUFreq;
}

void CPU::SetFreq(CPUFreq *freq)
{
    this->curCPUFreq = freq;
}

void CPU::SetScheduler(Scheduler *sched) {
    this->scheduler = sched;
}

EnergyModel::EnergyModel(CPUType type)
{
    this->type = type;
}

EnergyModel::EnergyModel(enum CPUType type, string &path)
{
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
        CPUFreq *cpuFreq = new CPUFreq{static_cast<uint32_t>(std::stoul(row[0])), std::stod(row[1]), static_cast<uint32_t>(std::stoul(row[2]))};
        this->cpufreqs.push_back(cpuFreq);
    }
    this->num = this->cpufreqs.size();
}

vector<CPUFreq*> *EnergyModel::GetFreqs()
{
    return &this->cpufreqs;
}

PerfDomain::PerfDomain(uint32_t cpuNum, EnergyModel *energyModel)
{
    this->energyModel = energyModel;
    printf("|-- PD: %p\n", this);
    for (uint32_t i = 0; i < cpuNum; i++) {
        CPU *cpu = new CPU(this, energyModel);
        printf("    |-- cpuid %p id %d\n", cpu, cpu->GetCPUId());
        this->cpus.push_back(cpu);
    }
    auto cpufreqs = *energyModel->GetFreqs();
    if (cpufreqs.empty()) {
        printf("%s: Empty CPU freq\n", __func__);
        return;
    }

    // 初始化频点 -> minCPUFreq
    this->curCPUFreq = cpufreqs[0];
}

vector<CPU *> PerfDomain::GetCPUS()
{
    return this->cpus;
}

CPUFreq *PerfDomain::GetCurCPUFreq()
{
    return this->curCPUFreq;
}

vector<CPUFreq*> *PerfDomain::GetEnergyModel()
{
    return this->energyModel->GetFreqs();
}

void PerfDomain::RebuildPerfDomain()
{
    uint32_t max_capacity = 0;
    for (auto cpu: this->cpus) {
        cpu->RebuildCapacity();
        if (cpu->GetCapacity() > max_capacity) {
            max_capacity = cpu->GetCapacity();
        }
    }
    this->curCPUFreq = this->getSuitableFreq(max_capacity);
    for (auto cpu: this->cpus) {
        cpu->SetFreq(this->curCPUFreq);
    }
}

CPUFreq *PerfDomain::getSuitableFreq(uint32_t expectCapacity)
{
    CPUFreq *ret = nullptr;
    auto em = this->GetEnergyModel();
    for (auto cpufreq:  *em) {
        if (cpufreq->capacity >= expectCapacity) {
            ret = cpufreq;
            break;
        }
    }

    return ret;
}