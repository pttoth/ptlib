
#include "TestLogger.hpp"
#include "TestConfig.hpp"
#include "TestEvent.hpp"

#include <iostream>

using namespace std;

int main()
{
    {
        TestLogger tl;
        bool success = tl.run();

        if(success){
            std::cout << "Logger test: SUCCESS\n\n\n\n";
        }else{
            std::cout << "Logger test: FAILED\n\n\n\n";
        }
    }


    {
        TestConfig tc;
        bool success = tc.run();

        if(success){
            std::cout << "Config test: SUCCESS\n\n\n\n";
        }else{
            std::cout << "Config test: FAILED\n\n\n\n";
        }
    }



    {
        TestEvent te;
        bool success = te.run();

        if(success){
            std::cout << "Event test: SUCCESS\n\n\n\n";
        }else{
            std::cout << "Event test: FAILED\n\n\n\n";
        }
    }



    return 0;
}
