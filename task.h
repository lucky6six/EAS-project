#ifndef __TASK_H__
#define __TASK_H__

#include <cstdint>

/* 默认任务为周期性任务，每工作@workTime，休息@sleepTime，@totalWorkTime是工作总时间 */
class Task {
public:
    uint64_t arrivalTime; /* ms */
    uint64_t workTime; /* ms */
    uint64_t totalWorkTime; /* ms */
    uint64_t sleepTime; /* ms */
    uint64_t deadlineTime; /* ms */
    uint32_t capacity; /* [0, 1024] */
    bool isOvertime; /* 是否超时 */
    bool isFinish;
};

#endif