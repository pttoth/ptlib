#pragma once

#include "Test.hpp"
#include "pt/event.hpp"

#include <iostream>

class TestEvent: public Test{
private:
    pt::EventTrigger<int, int> evtrigger;   //this is private (because it's callable)
public:
    pt::Event<int, int> ev;                 //this is public

    TestEvent():
        ev(evtrigger)   // Event and Trigger pairing happens in ctor
                        // (note, that this has to be done manually for every Event during move/copy-construction)
    {}

    virtual ~TestEvent(){}
    virtual bool run() override;

    bool testAddAndRemovals(uint32_t iterations = 5000);
};

class EventTestClass
{
private:
    void PrivateFunction(int a, int b);
    void ConstPrivateFunction(int a, int b) const;
public:
    void PublicFunction(int a, int b);
    void ConstPublicFunction(int a, int b) const;
    void addPrivateFunctionToEvent(pt::Event<int, int>& ev);
    void addConstPrivateFunctionToEvent(pt::Event<int, int>& ev) const;
};

void testfunc(int a, int b);
void foo(int a, int b);
void bar(int a, int b);
void fail1(int a);
void fail2(int a, int b, int c);
void fail3(int a, std::string str);
void fail4(long a, long b);
void fail5(short a, short b);
