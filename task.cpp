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

    return static_cast<double>(tmpCap) / CoreCap;
}

void Task::UpdateWorkTime(uint64_t CoreCap)
{
    this->totalWorkTime += CPU::timeSlice * CoreCap / 1024.0;
    //printf("task %d work %d\n", this->id,this->totalWorkTime);
    if (this->totalWorkTime > this->needWorkTime) {
        //printf("task %d finish\n", this->id);
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
}

uint64_t Task::GetTotalWorkTime()
{
    return this->totalWorkTime;
}

void Task::AddTotalWaitTime(uint64_t waitTime)
{
    this->totalWaitTime += waitTime;
}

uint64_t Task::GetTotalWaitTime() const
{
    return this->totalWaitTime;
}

uint32_t Task::GetTaskId()
{
    return this->id;
}