#pragma once

#include "Test.hpp"

class TestUtility: public Test
{
public:
    TestUtility(){}
    virtual ~TestUtility(){}
    virtual bool run() override;

};
