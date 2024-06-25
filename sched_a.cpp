#include "sched_a.h"

void EasScheduler::init() {
  em.init("path");
  getAllPerfDomains(pds);
}

CPU EasScheduler::findEasCpu(Task t) {
  for (auto pd : *pds) {
    for (auto cpu : pd.getCpus()) {
    }
  }
}

void EasScheduler::easSched(Task t) {
  CPU cpu = findEasCpu(t);
  // do something
}