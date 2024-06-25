#include <iostream>
#include <chrono>
#include "cpu.h"
#include "simulator.h"
#include <iostream>
#include <fstream>
#include <sstream>

using std::ifstream;
using std::istringstream;

uint32_t CPU::cpusid = 0;
uint64_t CPU::timeSlice = 20000; /* =20ms */

// TODO:初始化 要有curFreq(建议最小的)，perfDomain（belong_to），capacity（0）
CPU::CPU(EnergyModel *e)
{
    this->energyModel = e;
    this->id = cpusid++;
}

void CPU::test()
{
    std::cout << "CPU::test()" << std::endl;
}

uint32_t CPU::GetCurCapacity()
{
    return this->curFreq->capacity;
}

void CPU::execTask(Task *task)
{
    auto currentTime = Simultor::GetCurrentTime();
    /* Calculate cost time & capacity */
    if (task->checkDeadline(currentTime))
    {
        /* TODO */
    }

    task->updateWorkTime(this->GetCurCapacity());
}

void CPU::Run()
{
    auto curCPU = this;
    this->t = thread([curCPU]
                     {
        while(true) {
            if (!curCPU->tasksQueue.empty()) {
                Task *task = curCPU->tasksQueue.front();
                curCPU->execTask(task);
            }
            std::this_thread::sleep_for(std::chrono::microseconds(CPU::timeSlice));
            /* TODO:Ask scheduler for sched() */
        } });
    this->t.join();
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
// 重新计算task队列中的capacity和
void CPU::reBuildCapacity()
{
    uint32_t capacity = 0;
    for (auto task = this->tasksQueue.front(); task != this->tasksQueue.back(); ++task)
    {
        capacity += task->getCapacity();
    }
    this->capacity = capacity;
    return;
}

uint32_t CPU::GetCapacity()
{
    return this->capacity;
}

PerfDomain *CPU::GetPD()
{
    return this->perfDomain;
}

CPUFreq *CPU::GetFreq()
{
    return this->curFreq;
}

void CPU::SetFreq(CPUFreq *freq)
{
    this->curFreq = freq;
}

EnergyModel::EnergyModel(CPUType type)
{
    this->type = type;
}

EnergyModel::EnergyModel(enum CPUType type, const string &path)
{
    this->type = type;
    ifstream file(path);
    string line;
    while (getline(file, line))
    {
        istringstream ss(line);
        string cell;
        vector<string> row;

        while (std::getline(ss, cell, ','))
        {
            row.push_back(cell);
        }
        CPUFreq cpuFreq = {static_cast<uint32_t>(std::stoul(row[0])), std::stod(row[1]), static_cast<uint32_t>(std::stoul(row[2]))};
        this->cpufreqs.push_back(cpuFreq);
    }
    this->num = this->cpufreqs.size();
}

vector<CPUFreq> *EnergyModel::GetFreqs()
{
    return &this->cpufreqs;
}
// TODO::初始化频点！(建议最小的)()
PerfDomain::PerfDomain(uint32_t cpuNum, EnergyModel *energyModel)
{
    this->energyModel = energyModel;
    for (uint32_t i = 0; i < cpuNum; i++)
    {
        CPU *cpu = new CPU(energyModel);
        this->cpus.push_back(cpu);
    }
}

vector<CPU *> PerfDomain::GetCPUS()
{
    return this->cpus;
}

CPUFreq *PerfDomain::GetFreq()
{
    return this->curFreq;
}

vector<CPUFreq> *PerfDomain::GetEM()
{
    return this->energyModel->GetFreqs();
}

void PerfDomain::ReBuildPerfDomain()
{
    uint32_t max_capacity = 0;
    for (auto cpu = this->cpus.begin(); cpu != this->cpus.end(); ++cpu)
    {
        (*cpu)->reBuildCapacity();
        if ((*cpu)->GetCapacity() > max_capacity)
        {
            max_capacity = (*cpu)->GetCapacity();
        }
    }
    this->curFreq = this->getSuitableFreq(max_capacity);
    for (auto cpu = this->cpus.begin(); cpu != this->cpus.end(); ++cpu)
    {
        (*cpu)->SetFreq(this->curFreq);
    }
}

CPUFreq *PerfDomain::getSuitableFreq(uint32_t expectCapacity)
{
    CPUFreq *ret = nullptr;
    vector<CPUFreq> *EM = this->GetEM();
    for (auto it = EM->begin(); it != EM->end(); it++)
    {
        if (it->capacity >= expectCapacity)
        {
            ret = &(*it);
            break;
        }
    }
    return ret;
}