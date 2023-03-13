#pragma once

#include "Test.hpp"
#include "pt/logging.h"

#include <iostream>

class TestName: public Test
{
public:
    TestName(){}
    virtual ~TestName(){}
    virtual bool run() override;
    void printAsciiTable();
};
