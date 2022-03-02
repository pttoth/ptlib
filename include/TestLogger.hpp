#pragma once

#include "Test.hpp"
#include "pt/logging.h"

#include <iostream>

class TestLogger: public Test
{
public:
    TestLogger(){}
    virtual ~TestLogger(){}
    virtual bool run() override;
    void printAsciiTable();
};
