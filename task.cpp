#include "task.h"

bool Task::checkDeadline(uint64_t currentTime) {
    if (currentTime > deadlineTime + arrivalTime) {
        isOvertime = true;
        return true;
    }
    return false;
}

void Task::updateWorkTime(uint64_t Cap, uint64_t timeSlice) {
    uint64_t coreEnableTime = Cap;
}