/** -----------------------------------------------------------------------------
  * FILE:    config.h
  * AUTHOR:  pttoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Handles configuration files and their data
  * -----------------------------------------------------------------------------
  */

#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <istream>

namespace PT{

#define cfgAddKey(CFG_INSTANCE_NAME, ENUM_NAME) \
            CFG_INSTANCE_NAME.addKey(ENUM_NAME, #ENUM_NAME);

class Config{
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

private:
    struct entry{
        int key_id;                 //the key's actual enum as integer
        std::string  key_str;       //enum id as string
        std::string  val_str;       //string value associated with key
    };

    static const char*  _sep_keyval; //key-value spearator string
    static const char*  _sep_valcom; //value-comment spearator string

    std::vector<entry>  _entries;   //the stored data
    std::string         _path;      //file to read from and write to

    std::string     _getData(int eKey) const;
    std::string&    _getDataReference(int eKey);

    std::string     _trimComments(const std::string& str) const;
    bool            _isValidCharForFileName(char c) const;
    bool            _isValidPath(const std::string& path) const;
    bool            _isEmptyLine(const std::string& str) const;
    std::string     _buildErrorStringInvalidValue(int eKey) const;
    int             _getKeyIndex(int eKey) const;
    int             _getKeyIndex(const std::string& str) const;


    //after the config was read from any input, we have it in a string
    //this is the common processing function, which parses the expected
    //  data from the string
    void _processData(const std::string& data);
    void _parseData(std::istream& stream);
};

}

