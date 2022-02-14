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

bool TestEvent::
run()
{
    //other actors use Event this way
    ev.add(testfunc);
    ev.add(foo);
    ev.add(bar);
    ev.add(testfunc); //cannot add the same function with the same object twice

    //registering object's member functions
    EventTestClass tc;
    const EventTestClass ctc;

    ev.add(&tc, EventTestClass::ConstPublicFunction);  //non-const member, const function (ok)
    ev.add(&tc, EventTestClass::PublicFunction);       //non-const member, non-const function (ok)
    ev.add(&ctc, EventTestClass::ConstPublicFunction); //const member, const function (ok)

    //ev.add(&ctc, EventTestClass::PublicFunction);    //const member, non-const function (fails)
    //ev.add(&tc, EventTestClass::PrivateFunction);    //member func is private - owner has to add it (fails)

    //private functions are callable, if the private owner adds them
    tc.addPrivateFunctionToEvent(ev);            //private owner adds private func
    tc.addConstPrivateFunctionToEvent(ev);       //private owner adds private const func


    //const private owners are the same
    //ctc.addPrivateFunctionToEvent(ev);        //const private owner adds non-const member (fails)
    ctc.addConstPrivateFunctionToEvent(ev);     //const private owner adds const member (ok)


    //registering a non-const member function from a const private function
    //  should fail: this should not be const inside the const function





    //these mustn't compile
    //  ev.add(fail1); //too few parameters
    //  ev.add(fail2); //too many parameters
    //  ev.add(fail3); //different parameters
    //  ev.add(fail4); //different, but autocastable parameters (loss)
    //  ev.add(fail5); //different, but autocastable parameters (lossless)





    //owner calls the private object
    evtrigger(8, 26);


    //removing functions
    ev.remove_object(&tc);
    ev.remove_object(&ctc);
    ev.remove(bar);
    ev.remove(foo);
    ev.remove(testfunc);

    //check ev and trigger sizes
    //...

    return false; //TODO: fix
}


void
testfunc(int a, int b)
{
    std::cout << "testfunc called: a=" << a << ", b=" << b << "\n";
}

void
foo(int a, int b)
{
    std::cout << "foo called: a=" << a << ", b=" << b << "\n";
}

void
bar(int a, int b)
{
    std::cout << "bar called: a=" << a << ", b=" << b << "\n";
}

void
fail1(int a)
{
    std::cout << "fail1 called: a=" << a << "\n";
}

void
fail2(int a, int b, int c)
{
    std::cout << "fail2 called: a=" << a << ", b=" << b << ", c=" << c << "\n";
}

void
fail3(int a, std::string str)
{
    std::cout << "fail3 called: a=" << a << ", str=" << str << "\n";
}

void
fail4(long a, long b)
{
    std::cout << "fail4 called: a=" << a << ", b=" << b << "\n";
}

void
fail5(short a, short b)
{
    std::cout << "fail5 called: a=" << a << ", b=" << b << "\n";
}





void EventTestClass::PrivateFunction(int a, int b)
{
    std::cout << "PrivateFunction called: a=" << a << ", b=" << b << "\n";
}

void EventTestClass::
ConstPrivateFunction(int a, int b) const
{
    std::cout << "ConstPrivateFunction called: a=" << a << ", b=" << b << "\n";
}

void EventTestClass::
PublicFunction(int a, int b)
{
    std::cout << "PublicFunction called: a=" << a << ", b=" << b << "\n";
}

void EventTestClass::
ConstPublicFunction(int a, int b) const
{
    std::cout << "ConstPublicFunction called: a=" << a << ", b=" << b << "\n";
}

void EventTestClass::
addPrivateFunctionToEvent(pt::Event<int, int> &ev)
{
    ev.add(this, EventTestClass::PrivateFunction);
}

void EventTestClass::
addConstPrivateFunctionToEvent(pt::Event<int, int> &ev) const
{
    ev.add(this, EventTestClass::ConstPrivateFunction);
}

