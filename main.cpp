#include <iostream>
#include <fstream>

#include "sched.h"
#include "cpu.h"
#include "simulator.h"

void startSimulate() {
    Simulator simultor;
    simultor.Run();
    while (!Simulator::finishFlag) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Statistics::ReportAll();
}

int main() {
    startSimulate();

    return 0;
}