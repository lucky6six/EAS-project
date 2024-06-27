import time
import os
import argparse
import numpy as np
import random

SAMPLE_PATH = './task-tests/taskSample3.csv'
TITLE = 'arrivalTime,workPeriod,sleepPeriod,totalWorkTime,needWorkTime,deadlineTime\n'
MAX_CAPACITY = 1024
#Big: 1024 * 1 = 1024
#Middle: 805 * 4 = 3220
#Little: 193 * 3 = 579
CAPACITY_TOT = 1024 + 3220 + 579
MAX_PERIOD = 20
PRE_PERIOD_NUMBER = 5
PER_TASK_NUMBER = 1
CPU_PERIOD = 2
PER_NUMBER = 100

PER_NUMBER_TIME = PER_NUMBER * CPU_PERIOD
PER_TASK_TIME = PER_NUMBER_TIME * PER_TASK_NUMBER
PER_PERIOD_TIME = PER_NUMBER_TIME * PRE_PERIOD_NUMBER

#                   PERIOD 1
#       TASK 1           |           TASK 2
#       NUM 1            |
#PER_NUMBER*CPU_PERIOD

MYLIST = ['arrivalTime','workPeriod','sleepPeriod','totalWorkTime','needWorkTime','deadlineTime']

def getTask(periodNum, offset, capacity, workNum, averageCapacity, deadlineNum):
  return {"arrivalTime":(periodNum*PER_PERIOD_TIME+offset*PER_NUMBER_TIME+random.randint(0,PER_NUMBER_TIME))*1000,"workPeriod":capacity, \
          "sleepPeriod":MAX_CAPACITY-capacity,'totalWorkTime':0,"needWorkTime":1000*workNum*PER_NUMBER_TIME*capacity//MAX_CAPACITY, \
          "deadlineTime":1000*deadlineNum*PER_NUMBER_TIME}

def simulate_requests_negative_binomial(duration, average_rps, dispersion):
    # 计算负二项分布参数
    p = average_rps / (average_rps + dispersion)
    n = average_rps * p / (1 - p)
    
    # 使用负二项分布模拟每秒请求数
    requests_per_second = np.random.negative_binomial(n, p, size=duration)
    
    return requests_per_second

def getTasks(average, limitup, deadlineNum=5*PER_TASK_NUMBER):
  taskSet = []
  for i in range(MAX_PERIOD):
    cap = [min(x,limitup) for x in simulate_requests_negative_binomial(PRE_PERIOD_NUMBER//PER_TASK_NUMBER, 
           average, average)]
    cap[0]+=2500
    cap[2]-=1000
    cap[3]-=1500
    print(cap)
    for ii,x in enumerate(cap):
      leftCap = x
      while leftCap > 0:
        chooseCap = leftCap
        if leftCap > 10:
          chooseCap = random.randint(1, min(leftCap,50))
        taskSet.append(getTask(i,ii*PER_TASK_NUMBER,chooseCap,PER_TASK_NUMBER,average,deadlineNum))
        leftCap -= chooseCap
  return taskSet

def main():
  taskset = getTasks(2000, 3000)
  f = open(SAMPLE_PATH,'w')
  f.write(TITLE)
  taskset = sorted(taskset, key=lambda x: x["arrivalTime"])
  for task in taskset:
    for x in MYLIST:
      f.write("%s"%task[x])
      if x != MYLIST[-1]:
        f.write(',')
      else:
        f.write('\n')
  f.close()

if __name__ == "__main__":
  main()
