#ifndef __TASK_H__
#define __TASK_H__

#include <cstdint>

/*
 * 默认任务为周期性任务，每工作@workTime，休息@sleepTime，@totalWorkTime是工作总时间
 * 所有时间点都是相对于模拟器startTime的相对时间
 */
class Task
{
private:
    /* us为单位，0-500ms */
    uint64_t arrivalTime;
    /* us为单位，1-10ms */
    uint64_t workPeriod; /* us */
    /* us为单位，0-10ms */
    uint64_t sleepPeriod;   /* us */
    uint64_t totalWorkTime; /* us */
    /* us为单位，50-500ms */
    uint64_t needWorkTime; /* us */
    /* us为单位，大于 needWorkTime / workPeriod * (workPeriod + sleepPeriod) */
    uint64_t deadlineTime; /* us */
    uint32_t capacity;     /* [0, 1024] */
    bool isOvertime;       /* 是否超时 */
    bool isFinish;

public:
    Task()
    {
        isOvertime = false;
        isFinish = false;
    }

    bool checkDeadline(uint64_t currentTime);

    void updateWorkTime(uint64_t curCoreCapacity);

    uint32_t getCapacity();
};

#endif