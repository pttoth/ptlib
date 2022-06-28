#include "TestGuard.hpp"

#include "pt/logging.h"

TestGuard::
TestGuard():
    mCounterGuard(216)
{
}

bool TestGuard::
run()
{
    {
        auto counter_token = mCounterGuard.Acquire();
        auto counter_token2 = mCounterGuard.Acquire();

        int counter = *counter_token;

        //pt::out << counter;
        pt::out << "counter (int): " << counter << "\n";
        pt::out << "---'\n";
    }

    //test release + reacquire
    {
        mCounterGuard.Acquire();
    }

    //-----
    auto vec_token = mVectorGuard.Acquire();
    vec_token->push_back(1);
    vec_token->push_back(2);
    vec_token->push_back(3);

    auto vec = *vec_token;

    for( auto i : vec ){
        pt::out << i << "\n";
    }

    auto mgmmm = mVectorGuard.Acquire();

    pt::out << mgmmm->size();

}
