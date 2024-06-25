#ifndef BC9B3967_08FA_427C_9DFD_E6241A88F917
#define BC9B3967_08FA_427C_9DFD_E6241A88F917
// CPU util.
//  1024                 - - - - - - -              Energy Model
//                                           +-----------+-------------+
//                                           |  Little   |     Big     |
//   768                 =============       +-----+-----+------+------+
//                                           | Cap | Pwr | Cap  | Pwr  |
//                                           +-----+-----+------+------+
//   512  ===========    - ##- - - - -       | 170 | 50  | 512  | 400  |
//                         ##     ##         | 341 | 150 | 768  | 800  |
//   341  -PP - - - -      ##     ##         | 512 | 300 | 1024 | 1700 |
//         PP              ##     ##         +-----+-----+------+------+
//   170  -## - - - -      ##     ##
//         ##     ##       ##     ##
//       ------------    -------------
//        CPU0   CPU1     CPU2   CPU3

//  Current OPP: =====       Other OPP: - - -     util_avg (100 each): ##

#include <cstring>
#include <vector>

using namespace std;

enum PerfDomainType { LITTLE, MIDDLE, BIG };

struct Opp {
  int freq;
  double cap;
  int pwr;
};

class EnergyModel {
  vector<Opp> opps;

public:
  EnergyModel() { /*init */
  }
  struct Opp GetOppByCap(int cap, CPUType type){
      /* data */
  };
  void init(string path) {}
};

class EnergyModels {
  vector<EnergyModel> ems;
};

class PerfDomain {
  vector<CPU> cpus;
  PerfDomainType type;

public:
  vector<CPU> getCpus() { return cpus; }
};

class CPU {
  int id;
  int cap;
  int pwr;
  int freq;
  // tasks的队头是正在运行的task，周期性调度
  vector<Task> tasks;
  PerfDomainType type;

public:
  int getCap() { return cap; }
};

class Task {
  int id;
  int cap;
  double time;
};

//获取所有perfDomain的链表
void getAllPerfDomains(vector<PerfDomain> *pds) { /* code */
}

class EasScheduler {
  EnergyModel em;
  vector<PerfDomain> *pds;

  CPU findEasCpu(Task t);
  void findTaskToSched(CPU cpu);

public:
  void init();
  void easSched(Task t);
  // cpu把自己传给调度器，调度器返回一个需要将队首task cpu
  CPU sched(CPU cpu);
};

#endif /* BC9B3967_08FA_427C_9DFD_E6241A88F917 */
