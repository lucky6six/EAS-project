#include <chrono>
#include <fstream>
#include <sstream>

#include "cpu.h"
#include "sched.h"
#include "simulator.h"

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

}

void CPU::Run()
{
    auto curCPU = this;
    this->t = thread([curCPU]{
        while(!Simulator::finishFlag) {
            if (!curCPU->tasksQueue.empty()) {
                Task *task = curCPU->tasksQueue.front();
                curCPU->execTask(curCPU, task);
            }
            std::this_thread::sleep_for(std::chrono::microseconds(CPU::timeSlice));
            curCPU->scheduler->SchedCpu(curCPU);
        }
    });
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
void CPU::RebuildCapacity()
{
    uint32_t capacity = 0;
    for (auto task = this->tasksQueue.front(); task != this->tasksQueue.back(); ++task) {
        capacity += task->GetCapacity();
    }
    this->capacity = capacity;
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

EnergyModel::EnergyModel(enum CPUType type, const string &path)
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
    for (uint32_t i = 0; i < cpuNum; i++) {
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

void PerfDomain::RebuildPerfDomain()
{
    uint32_t max_capacity = 0;
    for (auto cpu: this->cpus) {
        cpu->RebuildCapacity();
        if (cpu->GetCapacity() > max_capacity) {
            max_capacity = cpu->GetCapacity();
        }
    }
    this->curFreq = this->getSuitableFreq(max_capacity);
    for (auto cpu: this->cpus) {
        cpu->SetFreq(this->curFreq);
    }
}

CPUFreq *PerfDomain::getSuitableFreq(uint32_t expectCapacity)
{
    CPUFreq *ret = nullptr;
    vector<CPUFreq> *em = this->GetEM();
    for (auto cpufreq:  *em) {
        if (cpufreq.capacity >= expectCapacity) {
            ret = &cpufreq;
            break;
        }
    }
    return ret;
}