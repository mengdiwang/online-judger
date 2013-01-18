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

class LZDATA_API xml : public LZData
{
public:
    xml(void);

    explicit xml(const std::string & tag);

    xml(const std::string & tag, const std::string & value);

    virtual ~xml(void);

    static LZDataPtr createFromBuffer(const char* buffer, int length);

    static LZDataPtr createFromFile(const std::string & fname);

    virtual LZDataPtr newOne(const std::string & tag, const std::string & value_);

    virtual void print(std::ostream & out, int depth=0) ;

private:
    xml(const xml&);
    const xml& operator=(const xml&);
};
