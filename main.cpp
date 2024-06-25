#include <iostream>

#include "sched.h"
#include "cpu.h"
#include "simulator.h"

void test() {
    Simulator simultor;
    vector<Task*> TaskList = simultor.InputTasks("../taskSample.csv");
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    test();

    return 0;
}