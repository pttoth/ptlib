#pragma once

#include "test/Test.hpp"

// Verifies boost.stacktrace + libbacktrace: function names, file:line.
//   Main target: real Windows, see 'src/test/TestStacktrace.cpp'.
class TestStacktrace: public Test
{
public:
    TestStacktrace(){}
    virtual ~TestStacktrace(){}
    virtual bool run() override;

};
