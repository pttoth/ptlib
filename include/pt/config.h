/** -----------------------------------------------------------------------------
  * FILE:    config.h
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Handles configuration files and their data
  * -----------------------------------------------------------------------------
  */

#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <istream>

namespace pt{

#define CfgAddKey(CFG_INSTANCE_NAME, ENUM_NAME) \
            CFG_INSTANCE_NAME.addKey(ENUM_NAME, #ENUM_NAME);

class Config
{
public:
    Config()                    = default;
    Config(const Config& other) = default;
    Config(Config&& source);
    virtual ~Config(){}
    Config& operator=(const Config &other)  = default;
    Config& operator=(Config &&source);
    bool operator==(const Config &other) const  = delete;

    void        addKey(int eKey, char const *name); //throws std::invalid_argument

    void        read();                         //throws std::invalid_argument, std::logic_error
    void        readF(char const *path);        //throws std::invalid_argument
    void        readF(const std::string& path); //throws std::invalid_argument
    void        readS(const std::string& str);  //throws std::invalid_argument

    void        write();                                //throws std::invalid_argument, std::logic_error
    void        writeF(char const *path);               //throws std::invalid_argument
    void        writeF(const std::string& path);        //throws std::invalid_argument

    void        setPath(const std::string& path);
    std::string getPath() const;

    char        getC(int eKey) const;           //throws std::invalid_argument
    std::string getS(int eKey) const;           //throws std::invalid_argument
    bool        getB(int eKey) const;           //throws std::invalid_argument
    float       getF(int eKey) const;           //throws std::invalid_argument
    double      getD(int eKey) const;           //throws std::invalid_argument
    int         getI(int eKey) const;           //throws std::invalid_argument

    void        setC(int eKey, char c);                     //throws std::invalid_argument
    void        setS(int eKey, const std::string& str);     //throws std::invalid_argument
    void        setB(int eKey, bool b);                     //throws std::invalid_argument
    void        setF(int eKey, float f);                    //throws std::invalid_argument
    void        setD(int eKey, double d);                   //throws std::invalid_argument
    void        setI(int eKey, int i);                      //throws std::invalid_argument

protected:
    static const char*  mSepKeyValue;       //key-value spearator string
    static const char*  mSepValueComment;   //value-comment spearator string

    struct entry{
        int key_id;                 //the key's actual enum as integer
        std::string  key_str;       //enum id as string
        std::string  val_str;       //string value associated with key
    };

    std::vector<entry>& GetEntriesRef();
    std::string&        GetPathRef();

private:
    std::vector<entry>  mEntries;   //the stored data
    std::string         mPath;      //file to read from and write to

    std::string     getData_(int eKey) const;
    std::string&    getDataReference_(int eKey);

    std::string     trimComments_(const std::string& str) const;
    bool            isValidCharForFileName_(char c) const;
    bool            isValidPath_(const std::string& path) const;
    bool            isEmptyLine_(const std::string& str) const;
    std::string     buildErrorStringInvalidValue_(int eKey) const;
    int             getKeyIndex_(int eKey) const;
    int             getKeyIndex_(const std::string& str) const;


    //after the config was read from any input, we have it in a string
    //this is the common processing function, which parses the expected
    //  data from the string
    void processData_(const std::string& data);
    void parseData_(std::istream& stream);
};

}

