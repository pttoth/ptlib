/** -----------------------------------------------------------------------------
  * FILE:    string.h
  * AUTHOR:  ptoth
  * EMAIL:   peter.t.toth92@gmail.com
  * PURPOSE: Reference-counted, immutable string implementation.
  *          Has own memory manager that prioritizes avoiding reallocations.
  * -----------------------------------------------------------------------------
  */

// Data doesn't get freed by default. (Can call static function to free unused blocks, if needed.)
// Control blocks are bulk-allocated (64 at a time).
// Control block:
//  data ptr | data offset | data length
// Allocated data blocks are multiples of 4 kbytes.
//

#pragma once

#include <memory>

namespace pt{

class string
{
public:
    string();
    string( const string& other );
    string( string&& source );

    virtual ~string();

    string& operator=( const string& other );
    string& operator=( string&& source );

    bool operator==( const string& other ) const;
protected:
private:
};

} // end of namespace 'pt'
