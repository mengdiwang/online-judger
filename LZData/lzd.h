//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once

#include "LZData.h"

class LZDATA_API lzd : public LZData
{
public:

    lzd(void);

    lzd(const std::string & tag);

    lzd(const std::string & tag, const std::string & value_);

    ~lzd(void);

    virtual void print(std::ostream & out, int depth=0) ;

    virtual LZDataPtr newOne(const std::string & tag, const std::string & value_);

    static LZDataPtr createFromBuffer(const char* buffer, int length);

    static LZDataPtr createFromFile(const std::string & fname);
private:
    lzd(const lzd&);
    const lzd& operator=(const lzd&);
};
