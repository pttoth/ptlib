#include "TestEvent.hpp"

bool TestEvent::
run()
{
    bool success = false;

    try{
        //a regular class and a const class containing an exposed Event and a private EventTrigger
        EventTestClass tc;
        const EventTestClass ctc;

        //registering functions happen with Event
        //  trying to register in EventTrigger should fail
        //evtrigger.addCallback(testfunc); //has to fail (function is private)

        //outside handlers use Event this way
        ev.addCallback(testfunc);
        ev.addCallback(foo);
        ev.addCallback(bar);
        ev.addCallback(testfunc); //cannot add the same function with the same object twice

        //registering object's member functions
        ev.addCallback(&tc, &EventTestClass::ConstPublicFunction);  //non-const member, const function (ok)
        ev.addCallback(&tc, &EventTestClass::PublicFunction);       //non-const member, non-const function (ok)
        ev.addCallback(&ctc, &EventTestClass::ConstPublicFunction); //const member, const function (ok)

        //ev.add(&ctc, EventTestClass::PublicFunction);    //const member, non-const function (fails)
        //ev.add(&tc, EventTestClass::PrivateFunction);    //member func is private - owner has to add it (fails)

        //private functions are callable, if the private owner adds them
        tc.addPrivateFunctionToEvent(ev);            //private owner adds private func
        tc.addConstPrivateFunctionToEvent(ev);       //private owner adds private const func

        //Const private owners adding functions respect const rules
        //ctc.addPrivateFunctionToEvent(ev);        //const private owner adds non-const member (fails)
        ctc.addConstPrivateFunctionToEvent(ev);     //const private owner adds const member (ok)


        //registering a non-const member function from a const private function
        //  should fail: this should not be const inside the const function

        //these mustn't compile
        //  ev.add(fail1); //too few parameters
        //  ev.add(fail2); //too many parameters
        //  ev.add(fail3); //different parameters
        //  ev.add(fail4); //different, but autocastable parameters (lossful casting)
        //  ev.add(fail5); //different, but autocastable parameters (lossless casting)


        //owner calls the private object
        evtrigger(8, 26);   //ok
        //ev(8, 26);        //fails


        //removing functions
        ev.removeObject(&tc);
        ev.removeObject(&ctc);
        ev.removeCallback(bar);
        ev.removeCallback(foo);
        ev.removeCallback(testfunc);

        //check registrations and removals
        success = this->testAddAndRemovals();
        if(!success){
            return false;
        }
        //check ev and trigger sizes
        //...


        return true;
    }catch(const std::invalid_argument& e){
        std::cout << "Event test: Badly parameterized test case: " << e.what() << "\n";
        return false;
    }catch(const std::exception& e){
        std::cout << "Event test: Unexpected exception occured: " << e.what() << "\n";
        return false;
    }catch(...){
        std::cout << "Event test: Unknown exception occured!\n";
        return false;
    }
    return false;
}

bool TestEvent::
testAddAndRemovals(uint32_t iterations)
{
    return false;
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
    ev.addCallback(this, &EventTestClass::PrivateFunction);
}

void EventTestClass::
addConstPrivateFunctionToEvent(pt::Event<int, int> &ev) const
{
    ev.addCallback(this, &EventTestClass::ConstPrivateFunction);
}

