#pragma once

#include "Test.hpp"
#include "pt/config.h"
#include "pt/utility.hpp"

#include <stdexcept>
#include <iostream>
#include <string>

class TestConfig: public Test
{

public:
    TestConfig():Test(){}
    virtual ~TestConfig(){}
    virtual bool run() override;

    void testTrims();
    bool testTrimWhitespace(char const *str, char const *expected) const;
};


bool TestConfig::
run()
{
    pt::Config cfg;
    enum e{
        fMaxDamage,
        fHealthPoints,
        bInvulnerable,
        bNewData1,
        bNewData2,
        fInvalidKey,
        iWriteFReadI,
        fWriteIReadF,
        cChar,
        cCharWithWhiteSpaces,
        cCharTabSpaces,
        cCharString,
    };
    cfgAddKey(cfg, fMaxDamage);
    cfgAddKey(cfg, fHealthPoints);
    cfgAddKey(cfg, bInvulnerable);

    cfgAddKey(cfg, cChar);
    cfgAddKey(cfg, cCharWithWhiteSpaces);
    cfgAddKey(cfg, cCharTabSpaces);
    cfgAddKey(cfg, cCharString);


    const char* pathin = "./conf.cfg";
    const char* pathout = "./conf_out.cfg";

    try{
        cfg.readF(pathin);
    }catch(std::invalid_argument& e){
        std::cout << "exception caught when trying to read file!\n  reason: " << e.what() << "\n";
    }

    cfg.setF(fMaxDamage, 100.0f);
    cfgAddKey(cfg, bNewData1);
    cfgAddKey(cfg, bNewData2);
    cfg.setB(bNewData1, true);
    cfg.setB(bNewData2, false);

    cfgAddKey(cfg, iWriteFReadI);
    float f = 1.0001f;
    cfg.setF(iWriteFReadI, f);
    std::cout << "write value: (float) " << f << "\n";
    std::cout << "read value: (int) " << cfg.getI(iWriteFReadI) << "\n";


    cfgAddKey(cfg, fWriteIReadF);
    float i = 155;
    cfg.setF(fWriteIReadF, i);
    std::cout << "write value: (int) " << i << "\n";
    std::cout << "read value:  (float)" << cfg.getF(fWriteIReadF) << "\n";

    //cfg.setF(fInvalidKey, 0.01f);

    cfg.writeF(pathout);

    //testTrims();

    std::cout << "------------------------\n";
    std::cout << "testing char handling:\n";
    std::cout << "------------------------\n";
    const char  c_char                  = 'c';
    const char *c_charwithwhitespaces   = "  a  ";
    const char *c_chartabspaces         = " \t ";
    const char *c_charstring            = "strval";


    cfg.setC(cChar, c_char);
    cfg.setS(cCharWithWhiteSpaces, c_charwithwhitespaces);
    cfg.setS(cCharTabSpaces, c_chartabspaces);
    cfg.setS(cCharString, c_charstring);

    try{
        std::cout << "cChar:                  string value: '" << c_char                  << "',\treturned value: '" << cfg.getC(cChar) << "'\n";
        std::cout << "cCharWithWhiteSpaces:   string value: '" << c_charwithwhitespaces   << "',\treturned value: '" << cfg.getC(cCharWithWhiteSpaces) << "'\n";
        try{
            std::cout << "cCharTabSpaces:         string value: '" << c_chartabspaces   << "'\n";
            std::cout << "  this should throw std::invalid_argument...";
            {
                char c = cfg.getC(cCharTabSpaces);
                std::cout << "  FAIL\n  returned char: '" << c << "'\n\n";
            }
        }catch(std::invalid_argument& e){
            std::cout << "  SUCCESS\n  exception caught: " << e.what() << "\n\n";
        }
        try{
            std::cout << "cCharString:            string value: '" << c_charstring << "'\n";
            std::cout << "  this should throw std::invalid_argument...";
            {
                char c = cfg.getC(cCharString);
                std::cout << "  FAIL\n  returned char: '" << c << "'\n\n";
            }
        }catch(std::invalid_argument& e){
            std::cout << "  SUCCESS\n  exception caught: " << e.what() << "\n\n";
        }
    }catch(std::invalid_argument& e){
        std::cout << "\nTEST FAILED:  unexpected exception caught: " << e.what() << "\n";
    }
    std::cout << "------------------------\n";
    return true;
}


void TestConfig::
testTrims()
{
    std::cout << "-----------------------------------\n";
    std::cout << "---------testing Config------------\n";
    std::cout << "-----------------------------------\n";
    testTrimWhitespace(" \t    \t  ", "");
    testTrimWhitespace("asd", "asd");
    testTrimWhitespace("123", "123");
    testTrimWhitespace("asd123", "asd123");
    testTrimWhitespace("123asd", "123asd");
    testTrimWhitespace("asd123asd", "asd123asd");
    testTrimWhitespace("123asd123", "123asd123");
    testTrimWhitespace(" asd  asd \t", "asd  asd");
    testTrimWhitespace("  asdasd \t", "asdasd");
    testTrimWhitespace(" 123 asdasd \t", "123 asdasd");
    testTrimWhitespace("  a", "a");
    testTrimWhitespace("a  ", "a");
    testTrimWhitespace(" a ", "a");
    testTrimWhitespace("  asd", "asd");
    testTrimWhitespace("asd  ", "asd");
    testTrimWhitespace(" asd ", "asd");
    std::cout << "-----------------------------------\n";
}


bool TestConfig::
testTrimWhitespace(const char *str, const char *expected) const
{
    std::string orig(str);
    std::string ex(expected);
    std::string res = pt::TrimWhitespaces(orig);
    std::cout << "orig content: '" << orig << "'\n";
    std::cout << "orig len: " << orig.length() << "\n";
    std::cout << "expected content: '" << ex << "'\n";
    std::cout << "expected len: " << ex.length() << "\n";
    std::cout << "res content: '" << res << "'\n";
    std::cout << "res len: " << res.length() << "\n";
    if(res == ex){ std::cout << "passed\n";
    }else{ std::cout << "failed\n";
    }
    std::cout << "---\n";
    return res == ex;
}

