
#include "TestLogger.hpp"
#include "TestConfig.hpp"
#include "TestEvent.hpp"
#include "TestUtility.hpp"

#include <iostream>

#include <chrono>
#include <iomanip>


#include <algorithm>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>

using namespace std;

int main()
{
    {
        const size_t length = 100*1000*1000;
        std::string source;
        std::string target;
        std::string target2;

        source.resize( length );
        target.resize( length );

        //fill source string
        size_t count = 0;
/*
        for( size_t i=0; i<length ; ++i){
            //alternating abcABC:   abc...xyzABC...XYZabc...xyz
            const char diff = 'a' - 'A';
            source[i] = 'a' + (count%2)*diff;
            count += (0 == i%26); //0 or 1
        }
*/
        /*
        for( size_t i=0; i<length ; ++i){
            //alternating abcABC:   abc...xyzABC...XYZabc...xyz
            const char diff = 'a' - 'A';
            source[i] = 'a' + (i%3)*diff;
            //count += (0 == i%26); //0 or 1
        }
*/

        source = "árvíztűrő tükörfúrógép";
        source = "ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP";
        //source = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        //source = "ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉPß$ŁíäđĐ";
        //source = pt::WStringToUTF8(L"árvíztűrő tükörfúrógép");
        //source = pt::WStringToUTF8(L"ÁRVÍZTŰRŐ TÜKÖRFÚRÓGÉP");
        //source = pt::WStringToUTF8(L"áé");
        std::cout << source << "\n";

        {
            std::chrono::time_point<std::chrono::system_clock> t_before = std::chrono::system_clock::now();
            target = pt::StringToLowerTransform( source );
            std::chrono::time_point<std::chrono::system_clock> t_after = std::chrono::system_clock::now();
            auto t_exec_ms = std::chrono::duration_cast<std::chrono::milliseconds>( t_after - t_before );
            std::cout << "time2: " << t_exec_ms.count() << "\n";
        }

        {
            std::chrono::time_point<std::chrono::system_clock> t_before = std::chrono::system_clock::now();
            target2 = pt::StringToLower( source );
            std::chrono::time_point<std::chrono::system_clock> t_after = std::chrono::system_clock::now();
            auto t_exec_ms = std::chrono::duration_cast<std::chrono::milliseconds>( t_after - t_before );
            std::cout << "time1: " << t_exec_ms.count() << "\n";
        }

        if( target == target2 ){
            std::cout << "output is identical\n";
        }else{
            std::cout << "output is invalid\n";
        }


        if( source.length() < 1024 ){
            std::cout << "  " << source << "\n";
        }
        if( target.length() < 1024 ){
            std::cout << "  " << target << "\n";
        }
        if( target2.length() < 1024 ){
            std::cout << "  " << target2 << "\n";
        }


        const char* t1 = target.c_str();
        const char* t2 = target2.c_str();

        size_t len1 = strlen( t1 );
        size_t len2 = strlen( t2 );
        size_t len = std::min( len1, len2 );
        std::cout << "lengths: " << len1 << ", " << len2 << ", min: " << len << "\n";
        for( size_t i=0; i<len; ++i ){
            if( t1[i] != t2[i] ){
                std::cout << "idx["<< i <<"]: " << t1[i] << ", " << t2[i] << "\n";
            }
        }


    }

    return 0;

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


    {
        TestUtility tu;
        bool success = tu.run();

        if(success){
            std::cout << "Utility test: SUCCESS\n\n\n\n";
        }else{
            std::cout << "Utility test: FAILED\n\n\n\n";
        }
    }

    return 0;
}
