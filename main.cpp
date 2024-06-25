#include <iostream>

#include "sched.h"
#include "cpu.h"

void test() {
    Scheduler s;
    CPU c;
    s.test();
    c.test();
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    test();

    return 0;
}