#include "pt/config.h"

#include <stdexcept>

#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>

#include <float.h>

#include "pt/std_extension.hpp"

using namespace pt;

//--------------------------------------------------
//                private globals
//--------------------------------------------------

static const char*  _sep_keyval = "=";  //key-value separator string
static const char*  _sep_valcom = ";";  //value-comment separator string

//--------------------------------------------------
//        private shared generic functions
//--------------------------------------------------

//-------------------------
//  forward declarations
//-------------------------

bool        _isEmptyLine(const std::string& str);       //TODO: rename  "IsE..."
bool        _isValidPath(const std::string& path);      //TODO: rename  "IsV..."
bool        _isValidCharForFileName(char c);            //TODO: rename  "IsV..."
std::string _trimComments(const std::string &str);      //TODO: rename  "Trim..."

//-------------------------
//     implementations
//-------------------------

bool
_isEmptyLine(const std::string &str){
    for(char c : str){
        if( !isspace(c) ){ return false; }
    }
    return true;
}


bool
_isValidPath(const std::string &path){
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
        if( ('/' == path[0])                            //cannot start with '/' (filters linux absolute paths)
          ||(std::string::npos != path.find(':'))       //cannot contain ':'    (filters windows absolute paths)
//          ||(std::string::npos != path.find(".."))      //cannot contain ".." (may want to step back from /bin to a config dir)
        ){
            return false;
        }
    }

    //mustn't contain illegal characters
    for(char c : path){
        if( !_isValidCharForFileName(c) ){
            return false;
        }
    }
    return true;
}


bool
_isValidCharForFileName(char c){
    const char* valids = "_-./";
    size_t len = strlen(valids);
    if( isalnum(c) ){ return true; } //A-Z, a-z, 0-9
    for(size_t i=0; i<len; ++i){
        if( c == valids[i] ){ return true; }
    }
    return false;
}


std::string
_trimComments(const std::string &str){
    size_t idx_denom = str.find(_sep_valcom);
    if(std::string::npos != idx_denom){
        return str.substr(0, idx_denom);
    }
    return str;
}


//--------------------------------------------------
//                    ICONFIG
//--------------------------------------------------

IConfig::
~IConfig(){
}

//--------------------------------------------------
//                    CONFIG
//--------------------------------------------------

Config::
Config(Config &&source)
{
    _entries    = std::move(source._entries);
}


Config::
~Config()
{

}


Config &Config::
operator=(Config &&source)
{
    _entries    = std::move(source._entries);
    return *this;
}


void Config::
addKey(int eKey, const char *name)
{
    int idx = _getKeyIndex(eKey);
    if(-1 < idx){
        throw std::invalid_argument("key already contained");
    }else{
        entry e;
        e.key_id = eKey;
        e.key_str = name;
        _entries.push_back(e);
        //printf("value added: %d, %s\n", eKey, name);
    }
}


void Config::
setName(const char *name)
{
    _name = name;
}


void Config::
setName(const std::string &name)
{
    _name = name;
}


std::string Config::
getName() const
{
    return _name;
}


char Config::
getC(int eKey) const{
    std::string data = _getData(eKey);  //getData() may throw "unknown key", we don't wanna catch that
    if( data.length() != 1 ){
        data = trimWhitespaces(data);   //may be longer than 1 char because of whitespaces
        if( data.length() != 1 ){       //try again with trim
            throw std::invalid_argument( _buildErrorStringInvalidValue(eKey) );
        }
    }
    return data[0];
}


std::string Config::
getS(int eKey) const{
    return _getData(eKey);
}


bool Config::
getB(int eKey) const{
    std::string data = trimWhitespaces( _getData(eKey) ); //getData() may throw "unknown key", we don't wanna catch that
    if(       "true"  == stringToLower(data) ){ return true;
    }else if( "false" == stringToLower(data) ){ return false;
    }
    throw std::invalid_argument( _buildErrorStringInvalidValue(eKey) );
}


float Config::
getF(int eKey) const{
    std::string data = trimWhitespaces( _getData(eKey) ); //getData() may throw "unknown key", we don't wanna catch that
    try{
        return std::stof(data);
    }catch(...){
        //stof()/stod()/stoi() throw these
        //  std::invalid_argument
        //  std::out_of_range
        //but a simple error will suffice instead
        throw std::invalid_argument( _buildErrorStringInvalidValue(eKey) );
    }
}


double Config::
getD(int eKey) const{
    std::string data = trimWhitespaces( _getData(eKey) ); //getData() may throw "unknown key", we don't wanna catch that
    try{
        return std::stod(data);
    }catch(...){
        //stof()/stod()/stoi() throw these
        //  std::invalid_argument
        //  std::out_of_range
        //but a simple error will suffice instead
        throw std::invalid_argument( _buildErrorStringInvalidValue(eKey) );
    }
}


int Config::
getI(int eKey) const{
    std::string data = trimWhitespaces( _getData(eKey) ); //getData() may throw "unknown key", we don't wanna catch that
    try{
        return std::stoi(data);
    }catch(...){
        //stof()/stod()/stoi() throw these
        //  std::invalid_argument
        //  std::out_of_range
        //but a simple error will suffice instead
        throw std::invalid_argument( _buildErrorStringInvalidValue(eKey) );
    }
}


void Config::
setC(int eKey, char c)
{
    std::string& data = _getDataReference(eKey);
    data = c;
}


void Config::
setS(int eKey, const std::string &str)
{
    std::string& data = _getDataReference(eKey);
    data = str;
}


void Config::
setB(int eKey, bool b)
{
    std::string& data = _getDataReference(eKey);
    if(b){ data = "true";
    }else{ data = "false";
    }
}


void Config::
setF(int eKey, float f)
{
    std::string& data = _getDataReference(eKey);
    char buf[64];
    sprintf(buf, "%f", f);
    data = buf;
}


void Config::
setD(int eKey, double d)
{
    std::string& data = _getDataReference(eKey);
    char buf[64];
    sprintf(buf, "%lf", d);
    data = buf;
}


void Config::
setI(int eKey, int i)
{
    std::string& data = _getDataReference(eKey);
    char buf[64];
    sprintf(buf, "%d", i);
    data = buf;
}


std::string Config::
writeToString() const
{
    std::string         retval;
    std::ostringstream  oss(retval);
    _writeData(oss);
    return retval;
}


void Config::
readFromString(std::string &str)
{
    _processData(str);
}


std::string Config::
_getData(int eKey) const{
    int idx = _getKeyIndex(eKey);
    if(-1 < idx){
        return _entries[idx].val_str;
    }
    throw std::invalid_argument("unknown key");
}


std::string &Config::
_getDataReference(int eKey)
{
    int idx = _getKeyIndex(eKey);
    if(-1 < idx){
        entry& ent = _entries[idx];
        return ent.val_str;
    }
    throw std::invalid_argument("unknown key");
}


std::string Config::
_buildErrorStringInvalidValue(int eKey) const{
    std::string         strError;
    std::stringstream   ss;
    ss << "invalid config value for key(" << eKey << ") : (";
    ss << _getData(eKey) << ")";

    strError = ss.str();
    return strError;
}


int Config::
_getKeyIndex(int eKey) const{
    for(size_t i=0; i<_entries.size(); ++i){
        if( eKey == _entries[i].key_id ){ return i; }
    }
    return -1;
}


int Config::
_getKeyIndex(const std::string &str) const{
    for(size_t i=0; i<_entries.size(); ++i){
        if( str == _entries[i].key_str ){ return i; }
    }
    return -1;
}


void Config::
_processData(const std::string &data)
{
    std::stringstream       ss(data);
    std::string             line;
    std::string             cfg;
    std::string             cfg_split[2]; //[0]: key, [1]: val
    while( std::getline(ss, line) ){
        cfg = _trimComments(line);
        if( !_isEmptyLine(cfg) ){
            if( splitString(cfg_split, cfg, _sep_keyval) ){
                cfg_split[0] = trimWhitespaces(cfg_split[0]);
                int idx = _getKeyIndex( cfg_split[0] );
                if(-1 < idx){
                    _entries[idx].val_str = cfg_split[1];
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
_writeData(std::ostream &stream) const
{
    for(size_t i=0; i<_entries.size(); ++i){
        const entry& ent = _entries[i];
        stream << ent.key_str << _sep_keyval << ent.val_str << "\n";
    }
}


//--------------------------------------------------
//                 ConfigFileReader
//--------------------------------------------------

ConfigIOReadWriter::
ConfigIOReadWriter()
{

}


ConfigIOReadWriter::
ConfigIOReadWriter(const std::vector<IConfig *> configs)
{
    for(Config* current : configs){
        registerConfig(current);
    }
}


ConfigIOReadWriter::
~ConfigIOReadWriter()
{

}


void ConfigIOReadWriter::
registerConfig(Config *cfg)
{
    int idx;
    if(cfg != nullptr){
        idx = indexOfInVector(_configs, cfg);
        if(idx < 0){
            _configs.push_back(cfg);
        }
    }
}


void ConfigIOReadWriter::
readFile(const char *path)
{
    //read file into string
    //for each found Config identifier
        //switch state to write the appropriate Config
        //reset the Config path variable
    //dump temporary string
}


void ConfigIOReadWriter::
readFile(const std::string &path)
{
    readFile( path.c_str() );
}


void ConfigIOReadWriter::
writeFile(const char *path)
{
    std::string buffer;
    for(size_t i=0; i<_configs.size(); ++i){
        buffer = _configs[i]->writeS();





        if( _isValidPath(path) ){
            std::ofstream ofs(path);
            if( !ofs.good() ){
                std::cerr << "error opening file " << path << " for reading\n";
                return;
            }
            _writeData(ofs);
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

}


void ConfigIOReadWriter::
writeFile(const std::string &path)
{
    writeFile( path.c_str() );
}


void ConfigIOReadWriter::
read()
{
    if( 0 == _path.length() ){
        throw std::logic_error("no path defined");
    }
    readF(_path);
}


void ConfigIOReadWriter::
readF(const char *path)
{
    if( _isValidPath(path) ){
        std::ifstream ifs;
        ifs.open(path);
        if( !ifs.is_open() ){
            std::string errormsg = "could not open file: ";
            errormsg += path;
            throw std::invalid_argument( errormsg );
        }
        _parseData(ifs);
        ifs.close();
    }else{
        std::string errormsg = "invalid path defined";
        errormsg += path;
        throw std::invalid_argument( errormsg );
    }
}


void ConfigIOReadWriter::
readF(const std::string &path)
{
    readF( path.c_str() );
}


void ConfigIOReadWriter::
readS(const std::string &str)
{
    std::stringstream ss(str);
    _parseData(ss);
}


void ConfigIOReadWriter::
write()
{
    if( 0 == _path.length() ){
        throw std::logic_error("no path defined");
    }
    writeF(_path);
}


void ConfigIOReadWriter::
writeF(char const *path)
{
    if( _isValidPath(path) ){
        std::ofstream ofs(path);
        if( !ofs.good() ){
            std::cerr << "error opening file " << path << " for reading\n";
            return;
        }
        _writeData(ofs);
        ofs.close();
    }else{
        //std::string errormsg = "invalid path defined";
        //errormsg += path;
        //throw std::invalid_argument( errormsg );

        //not necessary to throw exception, because it isn't a crash-type problem
        std::cerr << "ERROR: invalid path defined when writing config file: " << path << "\n";
    }
}


void ConfigIOReadWriter::
writeF(const std::string &path)
{
    writeF( path.c_str() );
}


std::string ConfigIOReadWriter::
writeS()
{
    std::string         retval;
    std::ostringstream  oss(retval);
    _writeData(oss);
    return retval;
}


void ConfigIOReadWriter::
setPath(const char *path)
{
    _path = path;
}


void ConfigIOReadWriter::
setPath(const std::string &path)
{
    _path = path;
}


std::string ConfigIOReadWriter::
        getPath() const{
    return _path;
}


void ConfigIOReadWriter::
_parseData(std::istream &stream)
{
    //read file to string
    std::stringstream   ss;
    std::string         data;
    size_t              expected_size = 0;
    std::string         strbuf;

    while( std::getline(stream, strbuf) ){
        if(0 < strbuf.length()){
            ss << strbuf << "\n";
            expected_size += strbuf.size() + 1; //calculates size to avoid possible LF/CRLF errors later
        }
    }

    //running the code from QtCreator, there seems to be a bug,
    //  where ss.str() is longer than it should be and contains garbage at the end
    //  didn't find errors in the code
    //    dunno if this is because Qt runs it with different dll-s
    //    TODO: check whether the Qt dll-s write '\n' correctly in memory, or they do '\r\n'
    //this cuts off any possible bullshit from the end of the string
    data = ss.str();
    data = data.substr(0, expected_size);

//look for lines containing Config names
    //use regex   \r?\n\[([^\r\n]*)\]\r?\n
    //skip if failed and write a warning
    //otherwise start checking the Config name
        //use regex   [A-Za-z0-9\.\_\-\ ]+
        //if fails, skip and write a warning
        //otherwise set focus to appropriate Config

//start parsing the rest of the string (only indexes) until you find another Config name, or EOF
    //create a substring based on the index
    //pass it to the Config with the focus
    //set focus to new Config


    //regexes


}

