#pragma once

#include "Test.hpp"

#include "pt/threadsafe.hpp"

#include <vector>

class TestGuard: public Test
{
public:
    TestGuard();

    virtual bool run() override;



private:

    pt::thread::Guarded<int>                mCounterGuard;
    pt::thread::Guarded<std::vector<int>>   mVectorGuard;




};
