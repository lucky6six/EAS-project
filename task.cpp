#include "task.h"
#include "cpu.h"
#include "simulator.h"

void Task::CheckDeadline(uint64_t currentTime)
{
    if (currentTime > deadlineTime + arrivalTime) {
        isOvertime = true;
    }
}
double Task::CalculateRatio(uint64_t CoreCap)
{
    uint64_t tmpCap;

    if (this->capacity >= CoreCap) {
        tmpCap = CoreCap;
    }
    else {
        tmpCap = this->capacity;
    }

    return static_cast<double>(tmpCap) / Simulator::MAX_CAP;
}

void Task::UpdateWorkTime(uint64_t CoreCap)
{
    this->totalWorkTime += CPU::timeSlice * CalculateRatio(CoreCap);

    if (this->totalWorkTime > this->needWorkTime) {
        this->isFinish = true;
    }
}

uint32_t Task::GetCapacity()
{
    return this->capacity;
}

uint64_t Task::GetArrivalTime()
{
    return this->arrivalTime;
};

uint64_t Task::GetTotalWorkTime()
{
    return this->totalWorkTime;
};