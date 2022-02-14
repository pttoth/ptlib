#pragma once

#include "Test.hpp"
#include "pt/logging.h"

#include <iostream>

class TestLogger: public Test
{
public:
    TestLogger(){}
    virtual ~TestLogger(){}
    virtual bool run() override;
};


bool TestLogger::
run()
{
    std::cout << "--------------------------------------------------\n";
    std::cout << "  Testing logger                                  \n";
    std::cout << "--------------------------------------------------\n";

    try{
        bool success = false;
        //try initializing default
        success = pt::log::Initialize("./", pt::log::AutoGenerateLogFileName());
        //success = pt::log::Initialize("./log/", pt::log::AutoGenerateLogFileName());
        //try initializing with non-existent root path (should fail)
        //try initializing with existing directories in 'filepath'
        //try initializing with non-existing directories in 'filepath' (should be created)

        if(success){
            std::cout << "Logger initialized successfully!\n";
        }else{
            std::cout << "Failed to initialize logger: Unknown reason\n";
        }

        std::cout << "-------------------------\n";
        std::cout << "Writing test strings into file: " << pt::log::GetFileName() << "\n";
        std::cout << "-------------------------\n";
        pt::log::debug << "testing debug: ASCII\n";
        pt::log::out   << "testing out:   ASCII\n";
        pt::log::warn  << "testing warn:  ASCII\n";
        pt::log::err   << "testing err:   ASCII\n";

        pt::log::debug << "printing ASCII table:\n";
        size_t count = 0;
        for(uint8_t i=0; count<256; ++count, ++i){
            char ichar = i;
            pt::log::debug << "ASCII #" << i << ": " << ichar << "\n";
        }
        pt::log::out << "\n";

        pt::log::out << "testing hungarian special characters: árvíztűrő tükörfúrógép\n";
        std::cout << "--------------------------------------------------\n";

        return true;
    }catch(const std::exception& e){
        std::cout << "Failed to initialize logger: " << e.what() << "\n";
        return false;
    }catch(...){
        std::cout << "Failed to initialize logger: UNKNOWN EXCEPTION \n";
        return false;
    }
    return false;
}


