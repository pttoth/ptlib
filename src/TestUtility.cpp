#include "TestUtility.hpp"

#include "pt/utility.hpp"

#include <iostream>

bool TestUtility::
run()
{
    std::cout << "--------------------------------------------------\n";
    std::cout << "  Testing Utility                                 \n";
    std::cout << "--------------------------------------------------\n";


    std::cout << "  Testing StringToLower/Upper \n";
    std::cout << "------------------------------\n";
    //StringToUpper/Lower
    std::string base = "abcdef123ABCDEF";

    std::string low = pt::StringToLower(base);
    std::string up = pt::StringToUpper(base);

    std::cout << "String:    '" << base << "'\n";
    std::cout << "Lowercase: '" << low << "'\n";
    std::cout << "Uppercase: '" << up << "'\n";




    return false;
}

