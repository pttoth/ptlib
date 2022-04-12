#pragma once

#include "Test.hpp"
#include "pt/event.hpp"

#include <string>

#include <iostream>

class TestEvent: public Test
{
private:
    pt::EventTrigger<int, int>          evtrigger;   //this is private (because it's callable)
    pt::EventTrigger<int, std::string>  evtrigger2;
    pt::EventTrigger<>                  evVoidTrigger;
public:
    pt::Event<int, int>             ev;         //this is public
    pt::Event<int, std::string>     ev2;
    pt::Event<>                     evVoid;

    TestEvent():
        ev(evtrigger)   // Event and Trigger pairing happens in ctor
                        // (note, that this has to be done manually for every Event during move/copy-construction)
        ,ev2(evtrigger2)
        ,evVoid(evVoidTrigger)
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
    static void StaticMemberFunction(int a, int b);
    static void StaticMemberFunction2(int a, std::string b);
    static void StaticVoidFunction();
    void VoidFunction();
    void ConstVoidFunction() const;
    void PublicFunction(int a, int b);
    void ConstPublicFunction(int a, int b) const;
    void addPrivateFunctionToEvent(pt::Event<int, int>& ev);
    void addConstPrivateFunctionToEvent(pt::Event<int, int>& ev) const;
};

void testfunc(int a, int b);
void testfuncVoid();
void foo(int a, int b);
void bar(int a, int b);
void fail1(int a);
void fail2(int a, int b, int c);
void fail3(int a, std::string str);
void fail4(long a, long b);
void fail5(short a, short b);
