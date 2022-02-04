#include <iostream>

#include "pt/logging.h"

using namespace std;

int main()
{
    pt::log::Initialize("../../log/", pt::log::AutoGenerateLogFileName());

    pt::log::debug << "testing debug: ASCII\n";
    pt::log::out   << "testing out:   ASCII\n";
    pt::log::warn  << "testing warn:  ASCII\n";
    pt::log::err   << "testing err:   ASCII\n";

    pt::log::out << "testing hungarian special characters: árvíztűrő tükörfúrógép\n";



    return 0;
}
