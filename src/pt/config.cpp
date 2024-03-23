#include "pt/config.h"

#include <stdexcept>

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

#include <float.h>

#include "pt/utility.hpp"

using namespace pt;

const char* Config::mSepKeyValue = "=";
const char* Config::mSepValueComment = ";";

Config::
Config(Config &&source)
{
    mEntries    = std::move(source.mEntries);
    mPath       = std::move(source.mPath);
}

Config &Config::
operator=(Config &&source)
{
    mEntries    = std::move(source.mEntries);
    mPath       = std::move(source.mPath);
    return *this;
}

void Config::
addKey(int eKey, const char *name)
{
    int idx = getKeyIndex_(eKey);
    if(-1 < idx){
        throw std::invalid_argument("key already contained");
    }else{
        entry e;
        e.key_id = eKey;
        e.key_str = name;
        mEntries.push_back(e);
        //printf("value added: %d, %s\n", eKey, name);
    }
}

void Config::
read()
{
    if( 0 == mPath.length() ){
        throw std::logic_error("no path defined");
    }
    readF(mPath);
}

void Config::
readF(const char *path)
{
    if( isValidPath_(path) ){
        std::ifstream ifs;
        ifs.open(path);
        if( !ifs.is_open() ){
            std::string errormsg = "could not open file: ";
            errormsg += path;
            throw std::invalid_argument( errormsg );
        }
        parseData_(ifs);
        ifs.close();
    }else{
        std::string errormsg = "invalid path defined";
        errormsg += path;
        throw std::invalid_argument( errormsg );
    }
}

void Config::
readF(const std::string &path)
{
    readF( path.c_str() );
}

void Config::
readS(const std::string &str)
{
    std::stringstream ss(str);
    parseData_(ss);
}

void Config::
write()
{
    if( 0 == mPath.length() ){
        throw std::logic_error("no path defined");
    }
    writeF(mPath);
}

void Config::
writeF(char const *path)
{
    if( isValidPath_(path) ){
        std::ofstream ofs(path);
        if( !ofs.good() ){
            std::cerr << "error opening file " << path << " for reading\n";
        }
        for(size_t i=0; i<mEntries.size(); ++i){
            entry& ent = mEntries[i];
            ofs << ent.key_str << mSepKeyValue << ent.val_str << "\n";
        }
        ofs.close();
    }else{
        /*
        std::string errormsg = "invalid path defined";
        errormsg += path;
        throw std::invalid_argument( errormsg );
        */
        //not necessary to throw exception, because it isn't a crash-type problem
        std::cerr << "ERROR: invalid path defined when writing config file: " << path << "\n";
    }
}

void Config::
writeF(const std::string &path)
{
    writeF( path.c_str() );
}


void Config::
setPath(const std::string &path)
{
    mPath = path;
}

std::string Config::
getPath() const
{
    return mPath;
}

char Config::
getC(int eKey) const
{
    std::string data = getData_(eKey);  //getData() may throw "unknown key", we don't wanna catch that
    if( data.length() != 1 ){
        data = TrimWhitespaces(data);   //may be longer than 1 char because of whitespaces
        if( data.length() != 1 ){       //try again with trim
            throw std::invalid_argument( buildErrorStringInvalidValue_(eKey) );
        }
    }
    return data[0];
}

std::string Config::
getS(int eKey) const
{
    return getData_(eKey);
}

bool Config::
getB(int eKey) const
{
    std::string data = TrimWhitespaces( getData_(eKey) ); //getData() may throw "unknown key", we don't wanna catch that
    if(       "true"  == StringToLower(data) ){ return true;
    }else if( "false" == StringToLower(data) ){ return false;
    }
    throw std::invalid_argument( buildErrorStringInvalidValue_(eKey) );
}

float Config::
getF(int eKey) const
{
    std::string data = TrimWhitespaces( getData_(eKey) ); //getData() may throw "unknown key", we don't wanna catch that
    try{
        return std::stof(data);
    }catch(...){
        //stof()/stod()/stoi() throw these
        //  std::invalid_argument
        //  std::out_of_range
        //but a simple error will suffice instead
        throw std::invalid_argument( buildErrorStringInvalidValue_(eKey) );
    }
}

double Config::
getD(int eKey) const
{
    std::string data = TrimWhitespaces( getData_(eKey) ); //getData() may throw "unknown key", we don't wanna catch that
    try{
        return std::stod(data);
    }catch(...){
        //stof()/stod()/stoi() throw these
        //  std::invalid_argument
        //  std::out_of_range
        //but a simple error will suffice instead
        throw std::invalid_argument( buildErrorStringInvalidValue_(eKey) );
    }
}

int Config::
getI(int eKey) const
{
    std::string data = TrimWhitespaces( getData_(eKey) ); //getData() may throw "unknown key", we don't wanna catch that
    try{
        return std::stoi(data);
    }catch(...){
        //stof()/stod()/stoi() throw these
        //  std::invalid_argument
        //  std::out_of_range
        //but a simple error will suffice instead
        throw std::invalid_argument( buildErrorStringInvalidValue_(eKey) );
    }
}

void Config::
setC(int eKey, char c)
{
    std::string& data = getDataReference_(eKey);
    data = c;
}

void Config::
setS(int eKey, const std::string &str)
{
    std::string& data = getDataReference_(eKey);
    data = str;
}

void Config::
setB(int eKey, bool b)
{
    std::string& data = getDataReference_(eKey);
    if(b){ data = "true";
    }else{ data = "false";
    }
}

void Config::
setF(int eKey, float f)
{
    std::string& data = getDataReference_(eKey);
    char buf[64];
    sprintf(buf, "%f", f);
    data = buf;
}

void Config::
setD(int eKey, double d)
{
    std::string& data = getDataReference_(eKey);
    char buf[64];
    sprintf(buf, "%lf", d);
    data = buf;
}

void Config::
setI(int eKey, int i)
{
    std::string& data = getDataReference_(eKey);
    char buf[64];
    sprintf(buf, "%d", i);
    data = buf;
}


const std::vector<pt::Config::entry>& pt::Config::
GetEntriesRef() const
{
    return mEntries;
}


const std::string& pt::Config::
GetPathRef() const
{
    return mPath;
}


std::vector<pt::Config::entry>& pt::Config::
GetEntriesRef()
{
    return mEntries;
}


std::string& pt::Config::
GetPathRef()
{
    return mPath;
}


std::string Config::
getData_(int eKey) const
{
    int idx = getKeyIndex_(eKey);
    if(-1 < idx){
        return mEntries[idx].val_str;
    }
    throw std::invalid_argument("unknown key");
}

std::string &Config::
getDataReference_(int eKey)
{
    int idx = getKeyIndex_(eKey);
    if(-1 < idx){
        entry& ent = mEntries[idx];
        return ent.val_str;
    }
    throw std::invalid_argument("unknown key");
}

bool Config::
isEmptyLine_(const std::string &str) const
{
    for(char c : str){
        if( !isspace(c) ){ return false; }
    }
    return true;
}

std::string Config::
buildErrorStringInvalidValue_(int eKey) const
{
    std::string         strError;
    std::stringstream   ss;
    ss << "invalid config value for key(" << eKey << ") : (";
    ss << getData_(eKey) << ")";

    strError = ss.str();
    return strError;
}

bool Config::
isValidCharForFileName_(char c) const
{
    const char* valids = "_-./";
    size_t len = strlen(valids);
    if( isalnum(c) ){ return true; } //A-Z, a-z, 0-9
    for(size_t i=0; i<len; ++i){
        if( c == valids[i] ){ return true; }
    }
    return false;
}

bool Config::
isValidPath_(const std::string &path) const
{
    //the function restricts paths to these characters
    //safe
    //A-Z, a-z, 0-9
    //_, -, ., /
    //note: the function doesn't allow:
        //'space' characters
        //backslashes
            //windows-type paths must be converted

    //path cannot be empty
    if( 0 == path.length()){
        //cannot start with whitespace
        if( isspace(path[0]) ){ return false; }

        //path must be relative (not security reasons, but want to enforce it)
        if( ('/' == path[0])            //cannot start with '/'
          ||(std::string::npos != path.find(':'))       //cannot contain ':'
//          ||(std::string::npos != path.find(".."))      //cannot contain ".." (may want to step back from /bin to a config dir)
        ){
            return false;
        }
    }

    //mustn't contain illegal characters
    for(char c : path){
        if( !isValidCharForFileName_(c) ){
            return false;
        }
    }
    return true;
}

std::string Config::
trimComments_(const std::string &str) const
{
    size_t idx_denom = str.find(mSepValueComment);
    if(std::string::npos != idx_denom){
        return str.substr(0, idx_denom);
    }
    return str;
}

int Config::
getKeyIndex_(int eKey) const
{
    for(size_t i=0; i<mEntries.size(); ++i){
        if( eKey == mEntries[i].key_id ) return i;
    }
    return -1;
}

int Config::
getKeyIndex_(const std::string &str) const
{
    for(size_t i=0; i<mEntries.size(); ++i){
        if( str == mEntries[i].key_str ) return i;
    }
    return -1;
}

void Config::
processData_(const std::string &data)
{
    std::stringstream ss(data);
    std::string line;
    std::string cfg;
    std::string cfg_split[2]; //[0]: key, [1]: val
    while( std::getline(ss, line) ){
        cfg = trimComments_(line);
        if( !isEmptyLine_(cfg) ){
            if( SplitString(cfg_split, cfg, mSepKeyValue) ){
                cfg_split[0] = TrimWhitespaces(cfg_split[0]);
                int idx = getKeyIndex_( cfg_split[0] );
                if(-1 < idx){
                    mEntries[idx].val_str = cfg_split[1];
                }else{
                    std::cerr << "WARNING: unrecognized config key: " << cfg_split[0] << "\n";
                }
            }else{
                std::cerr << "WARNING: could not interpret line: " << cfg << "\n";
            }
        }
    }
}

void Config::
parseData_(std::istream &stream)
{
    std::stringstream ss;
    std::string data;
    size_t expected_size = 0;
    std::string strbuf;

    while( std::getline(stream, strbuf) ){
        if(0 < strbuf.length()){
            ss << strbuf << "\n";
            expected_size += strbuf.size() + 1; //calculates size to avoid possible LF/CRLF errors later
        }
    }

    //running the code from QtCreator, there seems to be a bug,
    //  where ss.str() is longer than it should be and contains garbage at the end
    //  couldn't find errors in the code, dunno if this is because Qt runs it with different dll-s
    //this cuts off any possible bullshit from the end of the string
    data = ss.str();
    data = data.substr(0, expected_size);

    processData_(data);
}

