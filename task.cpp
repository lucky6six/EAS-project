#include "task.h"
#include "simulator.h"

bool Task::checkDeadline(uint64_t currentTime) {
    if (currentTime > deadlineTime + arrivalTime) {
        isOvertime = true;
        return true;
    }
    return false;
}

void Task::updateWorkTime(uint64_t CoreCap) {
    if (this->capacity >= CoreCap) {
        this->totalWorkTime += CPU::timeSlice * CoreCap / Simultor::MAX_CAP;
    } else {
        this->totalWorkTime += CPU::timeSlice * this->capacity / Simultor::MAX_CAP;
    }

    if (this->totalWorkTime > this->needWorkTime) {
        this->isFinish = true;
    }
}