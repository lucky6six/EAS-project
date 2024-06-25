#include <iostream>
#include <fstream>

#include "sched.h"
#include "cpu.h"
#include "simulator.h"

void test() {
    Simulator simultor;
    simultor.Run();
    while (!Simulator::finishFlag) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    Statistics::ReportAll();
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    test();

    return 0;
}