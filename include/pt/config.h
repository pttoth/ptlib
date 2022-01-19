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

class IConfig{
public:
    virtual ~IConfig();
    virtual void            setName(const char* name) = 0;
    virtual void            setName(const std::string& name) = 0;
    virtual std::string     getName() const = 0;
    virtual std::string     writeToString() const = 0;
    virtual void            readFromString(std::string& str) = 0;
};

class Config: public IConfig{
public:
    Config()                    = default;
    Config(const Config& other) = default;
    Config(Config&& source);
    virtual ~Config() override;

            Config&         operator=(const Config &other)  = default;
            Config&         operator=(Config &&source);
            bool            operator==(const Config &other) const  = delete;

            void            addKey(int eKey, char const *name); //throws std::invalid_argument

    virtual void            setName(const char* name) override;
    virtual void            setName(const std::string& name) override;
    virtual std::string     getName() const override;

    virtual std::string     writeToString() const override;
    virtual void            readFromString(std::string& str) override;

            char            getC(int eKey) const;           //throws std::invalid_argument
            std::string     getS(int eKey) const;           //throws std::invalid_argument
            bool            getB(int eKey) const;           //throws std::invalid_argument
            float           getF(int eKey) const;           //throws std::invalid_argument
            double          getD(int eKey) const;           //throws std::invalid_argument
            int             getI(int eKey) const;           //throws std::invalid_argument

            void            setC(int eKey, char c);                     //throws std::invalid_argument
            void            setS(int eKey, const std::string& str);     //throws std::invalid_argument
            void            setB(int eKey, bool b);                     //throws std::invalid_argument
            void            setF(int eKey, float f);                    //throws std::invalid_argument
            void            setD(int eKey, double d);                   //throws std::invalid_argument
            void            setI(int eKey, int i);                      //throws std::invalid_argument

private:
    struct entry{
        int             key_id;        //the key's actual enum as integer
        std::string     key_str;       //enum id as string
        std::string     val_str;       //string value associated with key
    };

    std::string         _name;      //name of the config object (used when mixing multiple configs in the same file)
    std::vector<entry>  _entries;   //the stored data

    std::string         _getData(int eKey) const;
    std::string&        _getDataReference(int eKey);

    std::string         _buildErrorStringInvalidValue(int eKey) const;

    int                 _getKeyIndex(int eKey) const;
    int                 _getKeyIndex(const std::string& str) const;

    //after the config was read from any input, we have it in a string
    //this is the common processing function, which parses the expected
    //  data from the string
    void                _processData(const std::string& data);
    void                _writeData(std::ostream& stream) const;
};

//--------------------------------------------------
//                ConfigIOReadWriter
//--------------------------------------------------
/**
 * @brief Used to mix separate Config classes into the same config file
 */
class ConfigIOReadWriter{
    std::vector<IConfig*>    _configs;
    int                     _current_index;

public:
    ConfigIOReadWriter();
    ConfigIOReadWriter(const std::vector<IConfig*> configs);

    virtual ~ConfigIOReadWriter();

    void registerConfig(IConfig* cfg);  //TODO: rename this....
    //void unregisterConfig(IConfig* cfg);   //is there a relevant use case for this?

    void            readFile(const char* path);
    void            readFile(const std::string& path);

    void            writeFile(const char* path);
    void            writeFile(const std::string& path);

    void            setPath(const char* path);
    void            setPath(const std::string& path);
    std::string     getPath() const;

    //-----
    //TODO: remove this
        void        read();                         //throws std::invalid_argument, std::logic_error
        void        readF(char const *path);        //throws std::invalid_argument
        void        readF(const std::string& path); //throws std::invalid_argument
        void        readS(const std::string& str);  //throws std::invalid_argument

        void        write();                                //throws std::invalid_argument, std::logic_error
        void        writeF(char const *path);               //throws std::invalid_argument
        void        writeF(const std::string& path);        //throws std::invalid_argument
        std::string writeS();
    //-----

private:
    std::string         _path;      //file to read from and write to

    void                _parseData(std::istream& stream);
};

} //end of namespace PT

