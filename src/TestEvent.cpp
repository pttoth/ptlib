#include "TestEvent.hpp"

bool TestEvent::
run()
{
    bool success = false;

    std::cout << "--------------------------------------------------\n";
    std::cout << "  Testing Event                                   \n";
    std::cout << "--------------------------------------------------\n";

    try{
        {
            //test autoremoval
            EventTestClass tc;
            ev.addCallback(testfunc, pt::EventExecRule::TriggerOnce);
            evtrigger(9, 11);
            evtrigger(23, 64);      //this mustn't get called here (TriggerOnce has to remove the registration at the first call)
            ev.addCallback(testfunc, pt::EventExecRule::TriggerOnce);
            evtrigger(10, 36);
            ev.addCallback(testfunc, pt::EventExecRule::TriggerOnce);
            ev.addCallback(testfunc, pt::EventExecRule::TriggerOnce);
            evtrigger(26767, 675);
            ev.shrink_to_fit();
            ev.addCallback(testfunc, pt::EventExecRule::TriggerOnce);
            evtrigger(23, 64);      //this mustn't get called here (TriggerOnce has to remove the registration at the first call)
            std::cout << "-----\n";

            ev.addCallback(testfunc, pt::EventExecRule::TriggerOnce);
            ev.addCallback(foo, pt::EventExecRule::TriggerOnce);
            ev.addCallback(bar, pt::EventExecRule::TriggerOnce);
            ev.clear();
        }

        {
            //test lambdas
            evVoid.addCallback( []() -> void
            {
                std::cout << "lambda(void) is working\n";
                std::cout << "---\n";
            }, pt::EventExecRule::TriggerOnce );

            ev.addCallback( [=](int a, int b) -> void
            {
                std::cout << "lambda(int, int) is working\n";
                std::cout << "  a=" << a << " , b=" << b << "\n";
                std::cout << "---\n";
            }, pt::EventExecRule::TriggerOnce );

            struct TestFunctor{
                void operator()(int pa, int pb){
                    std::cout << "TestClass::operator() called!\n";
                    std::cout << "  a=" << pa << " , b=" << pb << "\n";
                    std::cout << "---\n";
                }
            };

            TestFunctor tf;
            ev.addCallback( tf, pt::EventExecRule::TriggerOnce );

            struct TestNonFunctor{
                int a;
                int b;
            };

            TestNonFunctor tnf;
            //ev.addCallback( tnf, pt::ExecRule::TriggerOnce ); //has to fail, 'tnf' is not a function object

            evVoidTrigger();
            evtrigger(456, 6789);
        }

        {
            //test deliberate misuses
            ev.clear();
            ev.addCallback(nullptr);    //will crash on invocation

            //ev.addCallback(0);        //shouldn't compile, if possible
            //ev.addCallback(1);

            //evtrigger(5, 15);         //should crash with invalid registrations
            ev.clear();
        }

        //a regular class and a const class containing an exposed Event and a private EventTrigger
        EventTestClass tc;
        const EventTestClass ctc;

        //registering functions happen with Event
        //  trying to register in EventTrigger should fail
        //evtrigger.addCallback(testfunc); //has to fail ('addCallback()' function is private)

        //outside handlers use Event this way
        ev.addCallback(testfunc);
        ev.addCallback(foo);
        ev.addCallback(bar);
        ev.addCallback(testfunc); //can add the same function with the same object twice

        evVoid.addCallback(&testfuncVoid);

        //registering object's member functions
        ev.addCallback(&tc, &EventTestClass::ConstPublicFunction);  //non-const member, const function (ok)
        ev.addCallback(&tc, &EventTestClass::PublicFunction);       //non-const member, non-const function (ok)
        ev.addCallback(&ctc, &EventTestClass::ConstPublicFunction); //const member, const function (ok)

        //ev.add(&ctc, EventTestClass::PublicFunction);    //const member, non-const function (fails)
        //ev.add(&tc, EventTestClass::PrivateFunction);    //member func is private - owner has to add it (fails)

        ev.addCallback(EventTestClass::StaticMemberFunction);
        ev.addCallback(&EventTestClass::StaticMemberFunction);
        ev2.addCallback(&EventTestClass::StaticMemberFunction2);
        evVoid.addCallback(&EventTestClass::StaticVoidFunction);
        //evVoid.addCallback(&tc, &EventTestClass::StaticVoidFunction);     //nice if fails, but silent pass still produces valid result
        evVoid.addCallback(&tc, &EventTestClass::VoidFunction);
        evVoid.addCallback(&tc, &EventTestClass::ConstVoidFunction);
        //evVoid.addCallback(&ctc, &EventTestClass::VoidFunction);        //has to fail
        evVoid.addCallback(&ctc, &EventTestClass::ConstVoidFunction);


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

        evtrigger2(12, "abcdef");
        evVoidTrigger();


        //removing functions
        ev.removeObject(&tc);
        ev.removeObject(&ctc);
        ev.removeCallback(bar);
        ev.removeCallback(foo);
        ev.removeCallback(testfunc);

        //check registrations and removals
        success = this->testAddAndRemovals();
        if(!success){
            std::cout << "------------------------\n";
            return false;
        }
        //check ev and trigger sizes
        //...

        std::cout << "------------------------\n";
        return true;
    }catch(const std::invalid_argument& e){
        std::cout << "------------------------\n";
        std::cout << "Event test: Badly parameterized test case: " << e.what() << "\n";
        return false;
    }catch(const std::exception& e){
        std::cout << "------------------------\n";
        std::cout << "Event test: Unexpected exception occured: " << e.what() << "\n";
        return false;
    }catch(...){
        std::cout << "------------------------\n";
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


void testfuncVoid()
{
    std::cout << "testfuncVoid called\n";
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





void EventTestClass::
StaticMemberFunction(int a, int b)
{
    std::cout << "StaticMemberFunction called: a=" << a << ", b=" << b << "\n";
}

void EventTestClass::
StaticMemberFunction2(int a, std::string b)
{
    std::cout << "StaticMemberFunction2 called: a=" << a << ", b=\"" << b << "\"\n";
}

void EventTestClass::StaticVoidFunction()
{
    std::cout << "StaticVoidFunction called\n";
}

void EventTestClass::
PrivateFunction(int a, int b)
{
    std::cout << "PrivateFunction called: a=" << a << ", b=" << b << "\n";
}

void EventTestClass::
ConstPrivateFunction(int a, int b) const
{
    std::cout << "ConstPrivateFunction called: a=" << a << ", b=" << b << "\n";
}

void EventTestClass::
VoidFunction()
{
    std::cout << "VoidFunction called\n";
}

void EventTestClass::
ConstVoidFunction() const
{
    std::cout << "ConstVoidFunction called\n";
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


