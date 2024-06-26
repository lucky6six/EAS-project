import time
import os
import argparse
import numpy as np
import random

SAMPLE_PATH = './task-tests/taskSample3.csv'
TITLE = 'arrivalTime,workPeriod,sleepPeriod,totalWorkTime,needWorkTime,deadlineTime\n'
MAX_CAPACITY = 1024
MAX_PERIOD = 1
PERIOD_TIME = 30
SCALE_FACTOR = 3
MYLIST = ['arrivalTime','workPeriod','sleepPeriod','totalWorkTime','needWorkTime','deadlineTime']

#Big: 1024 * 1 = 1024
#Middle: 805 * 4 = 3220
#Little: 193 * 3 = 579



def getTask(periodNum, offset, capacity, workNum, deadlineNum):
  return {"arrivalTime":(periodNum*PERIOD_TIME+offset)*MAX_CAPACITY,"workPeriod":capacity, "sleepPeriod":MAX_CAPACITY-capacity,'totalWorkTime':0,"needWorkTime":workNum*capacity,"deadlineTime":MAX_CAPACITY*workNum*deadlineNum}

def simulate_requests_negative_binomial(duration, average_rps, dispersion):
    """
    模拟一个固定平均请求率（rps）和指定波动程度（dispersion）的应用在给定持续时间内的每秒请求数。
    
    :param duration: 持续时间（秒）
    :param average_rps: 平均每秒请求数（rps）
    :param dispersion: 波动程度（方差与均值的比值）
    :return: 每秒请求数的列表
    """
    # 计算负二项分布参数
    p = average_rps / (average_rps + dispersion)
    n = average_rps * p / (1 - p)
    
    # 使用负二项分布模拟每秒请求数
    requests_per_second = np.random.negative_binomial(n, p, size=duration)
    
    return requests_per_second

def getTasks(average, limitup, deadlineNum=5):
  taskSet = []
  for i in range(MAX_PERIOD):
    cap = [min(x,limitup) for x in simulate_requests_negative_binomial(PERIOD_TIME//SCALE_FACTOR, average, round(average ** 1.5))]
    print(cap)
    for ii,x in enumerate(cap):
      leftCap = x
      while leftCap > 0:
        chooseCap = leftCap
        if leftCap > 10:
          chooseCap = random.randint(1, min(leftCap,30))
        taskSet.append(getTask(i,ii*SCALE_FACTOR,chooseCap,SCALE_FACTOR,deadlineNum))
        leftCap -= chooseCap
  return taskSet



def main():
  taskset = getTasks(2000,3000)
  f = open(SAMPLE_PATH,'w')
  f.write(TITLE)
  for task in taskset:
    for x in MYLIST:
      f.write("%s"%task[x])
      if x != MYLIST[-1]:
        f.write(',')
      else:
        f.write('\n')
  f.close()


  #parser = argparse.ArgumentParser(description="Set trace generator")
  ## 添加 -m 或 --message 选项
  #parser.add_argument('-n', '--num', type=int, help='Number of period', default=10)
  #parser.add_argument('-a', '--all', action='store_true', help='all model')
  #parser.add_argument('-g', '--gc', action='store_true', help='calc over and gc and then calc')
  #parser.add_argument('-x', '--xall', action='store_true', help='run other before this')
  #parser.add_argument('-b', '--before', type=str, help='run before test',default='')
  #parser.add_argument('-v', '--vmtouch', action='store_true', help='use vmtouch to keep')
  #parser.add_argument('-k', '--keep', action='store_true', help='keep alive')
  #parser.add_argument('-q', '--quite', action='store_true', help='keep quite')
  ## 解析命令行参数
  #args = vars(parser.parse_args())


if __name__ == "__main__":
  main()
