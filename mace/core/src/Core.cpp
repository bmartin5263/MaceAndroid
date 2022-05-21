//
// Created by Brandon on 5/20/22.
//

#include "Core.h"
#include "Bridge.h"
#include <iostream>

void Core::doCore() {
    std::cout << "HELLO CORE\n";
    Bridge b{};
    b.doBridge();
}