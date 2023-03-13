#pragma once

#include "Test.hpp"

class TestThreadsafe: public Test
{
public:
    TestThreadsafe();
    TestThreadsafe( const TestThreadsafe& other ) = delete;
    TestThreadsafe( TestThreadsafe&& source ) = delete;

    virtual ~TestThreadsafe();

    TestThreadsafe& operator=( const TestThreadsafe& other ) = delete;
    TestThreadsafe& operator=( TestThreadsafe&& source ) = delete;

    bool operator==( const TestThreadsafe& other ) const = delete;

    bool run() override;
protected:
private:


};
