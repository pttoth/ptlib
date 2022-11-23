#include <iostream>

#include "actor.h"
#include "name.h"

using namespace std;

#include <map>
#include <unordered_map>

#include <cstring>

struct Asd{
    uint64_t        num;
    std::string     str;
    Asd():
        num(0)
    {}
    Asd(uint64_t num_, const std::string& str_):
        num(num_), str(str_)
    {}
};


void TestOrderedMultiMap()
{
    std::multimap<uint64_t, Asd> mMap;

    mMap.insert( {1, Asd( 11, "as" ) } );
    mMap.insert( {1, Asd( 12, "asd" ) } );

    mMap.insert( {2, Asd( 21, "as" ) } );
    mMap.insert( {2, Asd( 22, "asd" ) } );
    mMap.insert( {2, Asd( 23, "asdf" ) } );

    std::cout << "size   : " << mMap.size() << "\n";
    std::cout << "count 1: " << mMap.count(1) << "\n";
    std::cout << "count 2: " << mMap.count(2) << "\n";

    auto it1 = mMap.find( 1 );
    auto it2 = mMap.find( 2 );
    auto it3 = mMap.find( 3 );

    int counter = 0;
    while( it1 != mMap.cend() ){
        std::cout << "it1: " << (*it1).first
                  << "\n  | " << (*it1).second.num
                  << "\n  | " << (*it1).second.str
                  << "\n";
        ++it1;
        if( 4 < counter ){
            break;
        }
        ++counter;
    }

    counter = 0;
    while( it2 != mMap.cend() ){
        std::cout << "it2: " << (*it2).first
                  << "\n  | " << (*it2).second.num
                  << "\n  | " << (*it2).second.str
                  << "\n";
        ++it2;
        if( 4 < counter ){
            break;
        }
        ++counter;
    }

    if( it3 == mMap.cend() ) {
        std::cout << "it3 is empty\n";
    }


    //, Asd( 234, std::string("asdasd") )


}

void TestUnorderedMultiMap()
{
    std::unordered_multimap< uint64_t, Asd > map;


    map.insert( {1, Asd( 11, "as" ) } );
    map.insert( {1, Asd( 12, "asd" ) } );

    map.insert( {2, Asd( 21, "as" ) } );
    map.insert( {2, Asd( 22, "asd" ) } );
    map.insert( {2, Asd( 23, "asdf" ) } );

    std::cout << "size   : " << map.size() << "\n";
    std::cout << "count 1: " << map.count(1) << "\n";
    std::cout << "count 2: " << map.count(2) << "\n";

    auto it1 = map.find( 1 );
    auto it2 = map.find( 2 );
    auto it3 = map.find( 3 );

    int counter = 0;
    while( it1 != map.cend() ){
        std::cout << "it1: " << (*it1).first
                  << "\n  | " << (*it1).second.num
                  << "\n  | " << (*it1).second.str
                  << "\n";
        ++it1;
        if( 4 < counter ){
            break;
        }
        ++counter;
    }

    counter = 0;
    while( it2 != map.cend() ){
        std::cout << "it2: " << (*it2).first
                  << "\n  | " << (*it2).second.num
                  << "\n  | " << (*it2).second.str
                  << "\n";
        ++it2;
        if( 4 < counter ){
            break;
        }
        ++counter;
    }

    if( it3 == map.cend() ) {
        std::cout << "it3 is empty\n";
    }



}

void TestName()
{
    {
        const pt::Name defaultname;
        const pt::Name defaultname2;

        const pt::Name name1("Name1");

        const pt::Name name2("Name2");
        const pt::Name name2b("Name2");
        const pt::Name name2c("Name2");

        const pt::Name name3("Name3");
        const pt::Name name3a("Name3");


        std::cout << "performing Name-Name equality tests...\n";
        bool def1_def2 = defaultname == defaultname2;
        bool res1_def = name1 == defaultname;
        bool res1_definv = defaultname == name1;
        bool res1_2   = name1 == name2;
        bool res2_2b  = name2 == name2b;
        bool res2_2c  = name2 == name2c;
        bool res3_2   = name2 == name3;
        std::cout << "def1_def2   : expected = true  | result = " << def1_def2 << "\n";
        std::cout << "res1_def    : expected = false | result = " << res1_def << "\n";
        std::cout << "res1_definv : expected = false | result = " << res1_definv << "\n";
        std::cout << "res1_2  : expected = false | result = " << res1_2 << "\n";
        std::cout << "res2_2b : expected = true | result = " << res2_2b << "\n";
        std::cout << "res2_2c : expected = true | result = " << res2_2c << "\n";
        std::cout << "res3_2  : expected = false | result = " << res3_2 << "\n";
        std::cout << "\n";

        std::cout << "printing Names...\n";
        std::cout << "default : " << defaultname.ToString() << "\n";
        std::cout << "Name1   : " << name1.ToString() << "\n";
        std::cout << "Name2   : " << name2.ToString() << "\n";
        std::cout << "Name2b  : " << name2b.ToString() << "\n";
        std::cout << "Name3   : " << name3.ToString() << "\n";
        std::cout << "Name3a  : " << name3a.ToString() << "\n";
        std::cout << "\n";


        std::cout << "performing Name-String equality tests...\n";

        char empty_cstr = 0;
        char str[127];
        strcpy (str, "non-const char* str");

        char str1[127];
        strcpy (str1, "Name1");

        std::cout << "default vs " << "" << "(empty): " << (defaultname == "") << "\n";
        std::cout << "default vs " << "Name1" << ": " << (defaultname == "Name1") << "\n";

        std::cout << "default vs " << &empty_cstr << "(empty cstr): " << (defaultname == &empty_cstr) << "\n";
        std::cout << "default vs " << str << ": " << (defaultname == str) << "\n";

        std::cout << "Name1   vs " << "Name1" << ": " << (name1 == "Name1") << "\n";
        std::cout << "Name1   vs " << "" << "(empty): " << (name1 == "") << "\n";
        std::cout << "Name1   vs " << "non-const Name1: " << (name1 == str1) << "\n";
        std::cout << "Name2   vs " << "Name2" << ": " << (name2 == "Name2") << "\n";
        std::cout << "Name2b  vs " << "Name2" << ": " << (name2 == "Name2") << "\n";
        std::cout << "Name2c  vs " << "Name2" << ": " << (name2 == "Name2") << "\n";
        std::cout << "Name3   vs " << "Name3" << ": " << (name3 == "Name3") << "\n";
        std::cout << "Name3a  vs " << "Name3" << ": " << (name3 == "Name3") << "\n";

        std::cout << "Name1   vs " << "Name2" << "       : " << (name1 == "Name2") << "\n";
        std::cout << "Name2   vs " << "'never seen'" << ": " << (name2 == "'never seen'") << "\n";
        std::cout << "Name2   vs " << "Name3" << "       : " << (name2 == "Name3") << "\n";
        std::cout << "Name3   vs " << "Name1" << "       : " << (name3 == "Name1") << "\n";


        std::cout << "\n";
    }

    {
        std::cout << "deleted existing instances, creating new ones...\n";

        const pt::Name name2("Name2");
        const pt::Name name2b("Name2");

        const pt::Name name3("Name3");
        const pt::Name name3a("Name3");

        const pt::Name name4("Name4");
        const pt::Name name4a("Name4");

        const pt::Name name5("Name5");
        const pt::Name name5a("Name5");

        std::cout << "performing Name-Name equality tests...\n";
        bool res2_2b = name2 == name2b;
        bool res2_3  = name2 == name3;
        bool res3_3a = name3 == name3a;
        bool res4_4a = name4 == name4a;
        bool res4_5a = name4 == name5a;
        bool res5_5a = name5 == name5a;
        std::cout << "res2_2b: expected = true    | result = " << res2_2b << "\n";
        std::cout << "res2_3 : expected = false   | result = " << res2_3 << "\n";
        std::cout << "res3_3a: expected = true    | result = " << res3_3a << "\n";
        std::cout << "res4_4a: expected = true    | result = " << res4_4a << "\n";
        std::cout << "res4_5a: expected = false   | result = " << res4_5a << "\n";
        std::cout << "res5_5a: expected = true    | result = " << res5_5a << "\n";
        std::cout << "\n";

        std::cout << "printing Names...\n";
        std::cout << "name2: "  << name2.ToString()  << "\n";
        std::cout << "name2b: " << name2b.ToString() << "\n";
        std::cout << "name3: "  << name3.ToString()  << "\n";
        std::cout << "name3a: " << name3a.ToString() << "\n";
        std::cout << "name4: "  << name4.ToString()  << "\n";
        std::cout << "name4a: " << name4a.ToString() << "\n";
        std::cout << "name5: "  << name5.ToString()  << "\n";
        std::cout << "name5a: " << name5a.ToString() << "\n";
        std::cout << "\n";
    }


    //TODO: add tests for copy and move assignment operators
    //TODO: try move constructor


}

int main()
{

    //TestOrderedMultiMap();
    //TestUnorderedMultiMap();

    TestName();

    return 0;
}
