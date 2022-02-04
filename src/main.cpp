#include <iostream>

#include "pt/logging.h"

using namespace std;

int main()
{
    //pt::log::Initialize("../../log", pt::log::AutoGenerateLogFileName());
    pt::log::Initialize("../../log/", pt::log::AutoGenerateLogFileName());

    pt::log::out << "árvíztűrő tükörfúrógép\n";

    return 0;
}
