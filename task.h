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
    uint32_t id;

    Task()
    {
        isOvertime = false;
        isFinish = false;
    }

    Task(uint64_t arrivalTime, uint64_t workPeriod, uint64_t sleepPeriod,
         uint64_t totalWorkTime, uint64_t needWorkTime, uint64_t deadlineTime)
    {
        static uint32_t newid = 0;
        this->arrivalTime = arrivalTime;
        this->workPeriod = workPeriod;
        this->sleepPeriod = sleepPeriod;
        this->totalWorkTime = totalWorkTime;
        this->needWorkTime = needWorkTime;
        this->deadlineTime = deadlineTime;
        this->capacity = 1024 * workPeriod / (workPeriod + sleepPeriod);
        this->isOvertime = false;
        this->isFinish = false;
        this->id = newid++;
    };

    void CheckDeadline(uint64_t currentTime);

    void UpdateWorkTime(uint64_t curCoreCapacity);

    uint32_t GetCapacity();

    bool IsTaskFinish()
    {
        return isFinish;
    }

    bool IsTaskDelay()
    {
        return isOvertime;
    }

    double CalculateRatio(uint64_t);

    uint64_t GetArrivalTime();

    uint64_t GetTotalWorkTime();

    ~Task() = default;
};

#endif