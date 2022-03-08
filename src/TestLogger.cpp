#include "TestLogger.hpp"


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

        pt::debug << "testing debug without 'pt' namespace: ASCII\n";
        pt::out   << "testing out without 'pt' namespace:   ASCII\n";
        pt::warn  << "testing warn without 'pt' namespace:  ASCII\n";
        pt::err   << "testing err without 'pt' namespace:   ASCII\n";

        printAsciiTable();

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


void TestLogger::
printAsciiTable()
{
    pt::log::debug << "printing ASCII table:\n";
    size_t count = 0;
    for(uint8_t j=0; j<32; ++j){
        for(uint8_t i=0; i<8; ++count, ++i){
            uint8_t num = j+i*32;
            char ichar = num;
            if((7 <= num) && (num <= 8)){
                pt::log::debug << "#" << num << "  : " << " ";
            }else if(num == 9){
                pt::log::debug << "#" << num << "  :" << "\\t";
            }else if(num == 10){
                pt::log::debug << "#" << num << " :" << "\\n";
            }else if(num == 13){
                pt::log::debug << "#" << num << " :" << "\\r";
            }else if(num < 10){
                pt::log::debug << "#" << num << "  : " << ichar;
            }else if(num < 100){
                pt::log::debug << "#" << num << " : " << ichar;
            }else{
                pt::log::debug << "#" << num << ": " << ichar;
            }

            if(count%8 == 7){
                pt::log::debug << "\n";
            }else{
                pt::log::debug << "   ";
            }
        }
    }
}
